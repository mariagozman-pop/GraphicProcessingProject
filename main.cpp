#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp" //glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp" //glm extension for computing inverse matrices
#include "glm/gtc/type_ptr.hpp" //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"
#include "Rain.hpp"

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 0.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.5f;

GLboolean pressedKeys[1024];

// models
gps::Model3D scene;
GLfloat angle;

// shaders
gps::Shader myBasicShader;
gps::Shader skyboxShader;
gps::Shader rainShader;

// tour
struct TourPoint{
    glm::vec3 position;
    glm::vec3 rotation;
};  
std::vector<TourPoint> tourPoints = {
    {{0.15f, 3.00f, -5.76f}, {0.0f, 0.0f, 0.0f}},
    {{2.81f, 3.00f, -25.92f}, {0.0f, 0.5f, 0.0f}},
    {{3.54f, 3.00f, -41.18f}, {0.0f, 0.5f, 0.0f}},
    {{16.77f, 3.00f, -40.59f}, {0.0f, 0.0f, 0.0f}},
    {{30.0f, 3.0f, -40.0f}, {0.0f, 1.0f, 0.0f}},
    {{35.0f, 3.0f, -30.0f}, {0.0f, 0.0f, 0.0f}},
    {{50.0f, 3.0f, -20.0f}, {0.0f, 0.0f, 0.0f}},
    {{50.0f, 3.0f, -8.0f}, {0.0f, 0.5f, 0.0f}},
    {{35.0f, 3.0f, -3.50f}, {0.0f, 0.0f, 0.0f}},
    {{10.0f, 3.0f, -1.0f}, {0.0f, 1.0f, 0.0f}}
};
bool isTourActive = false; 
float tourSpeed = 0.01f;   
int currentPoint = 0;      
float t = 0.0f;            

// sun
bool lightEnabled = true;

// point lights
bool lightBulbEnabled = true;
bool lightSource1Enabled = true;
bool lightSource2Enabled = true;
bool lightSource3Enabled = true;

// house
gps::Model3D houseModel;
glm::vec3 housePosition(0.0f, 0.0f, 0.0f);

// light bulb
gps::Model3D lightBulbModel;
glm::vec3 lightBulbPosition(0.15f, 4.00f, -3.70f);

glm::vec3 lightBulbPos;
GLint lightBulbPosLoc; 

glm::vec3 lightBulbColor;
GLint lightBulbColorLoc; 

glm::vec3 isLightBulb;
GLint isLightBulbLoc; 

glm::vec3 emissiveColor;
GLint emissiveColorLoc; 

// wireframe and point view
bool isWireframe = false;
bool isPointView = false;

//mouse callback
double lastX, lastY;            
bool firstMouse = true;       
bool isDragging = false;      
float zoomSensitivity = 10.0f;   
float rotationSensitivity = 0.1f; 
bool doubleClick = false;     
double lastClickTime = 0.0;  

// sky box
gps::SkyBox mySkyBox;;
std::vector<const GLchar*> faces;

// fog 
bool enableFog;
GLint enableFogLoc;
glm::vec3 fogColor;
GLint fogColorLoc;
float fogDensity;
GLint fogDensityLoc;
GLint enableFogLocSky;
GLint fogColorLocSky;

// rain
bool enableRain = false;
float lastTimeStamp = 0.0;
RainSystem gRain(1000, rainShader);

// transparent objects
GLint transparentLoc;

gps::Model3D bottleModel;
glm::vec3 bottlePosition(32.0f, 0.0f, 2.0f);

gps::Model3D windowModel;
glm::vec3 windowPosition(3.9f, 2.5f, -9.0f);
glm::vec3 windowPosition2(3.9f, 2.5f, -13.0f);

gps::Model3D glassModel;
glm::vec3 glassPosition(32.0f, 0.65f, 0.0f);

gps::Model3D waterModel;
glm::vec3 waterPosition(0.0f, 0.3f, 0.0f);

// source lights
gps::Model3D lanternModel;
gps::Model3D candleModel;
glm::vec3 lightSource1Position(32.0f, 5.0f, -2.0f);
glm::vec3 lightSource2Position(20.0f, 5.0f, -12.0f);
glm::vec3 lightSource3Position(8.0f, 5.0f, -8.0f);

//windmill animation
gps::Model3D bladesModel;
gps::Model3D windmillBaseModel;
glm::vec3 windmillPosition(-32.0f, 0.0f, -40.0f);
glm::vec3 windmillBladesPosition(-38.5f, -5.4f, -32.0f);


GLenum glCheckError_(const char* file, int line, const char* functionName)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        default:
            error = "UNKNOWN_ERROR";
            break;
        }

        // Output more specific error information
        std::cout << error << " in function: " << functionName << " | "
            << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}

// Macro to automatically pass the function name
#define glCheckError() glCheckError_(__FILE__, __LINE__, __func__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);

    glViewport(0, 0, width, height);

    projection = glm::perspective(glm::radians(45.0f),
        (float)width / (float)height,
        0.1f, 200.0f);

    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void updateViewMatrix() {
    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    if (isDragging) {
        float xOffset = (xpos - lastX) * rotationSensitivity;
        float yOffset = (lastY - ypos) * rotationSensitivity; 

        myCamera.rotate(yOffset, xOffset);
        updateViewMatrix();
    }

    lastX = xpos;
    lastY = ypos;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            double currentTime = glfwGetTime();

            if (currentTime - lastClickTime < 0.3) {
                doubleClick = true;
            }
            else {
                doubleClick = false;
            }

            lastClickTime = currentTime;

            if (doubleClick) {
                myCamera.zoom(-zoomSensitivity); 
                updateViewMatrix();
            }
            else {
                isDragging = true; 
            }
        }
        else if (action == GLFW_RELEASE) {
            isDragging = false;
        }
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        double currentTime = glfwGetTime();

        if (currentTime - lastClickTime < 0.3) {
            myCamera.zoom(zoomSensitivity);
            updateViewMatrix();
        }

        lastClickTime = currentTime;
    }
}

void processKeyInputs() {
    float rotationStep = 2.0f;
    float lightSpeed = 0.1f;  
    float altitudeSpeed = 0.05f; 

    if (pressedKeys[GLFW_KEY_T]) {
        isTourActive = !isTourActive;
        if (isTourActive) {
			myCamera.cameraFrontDirection = glm::vec3(0.0f, 0.0f, 0.0f);
            t = 0.0f;
            currentPoint = 0;
        }
    }

    if (pressedKeys[GLFW_KEY_0]) {
        lightEnabled = !lightEnabled; 
    }

    if (pressedKeys[GLFW_KEY_9]) {
        lightBulbEnabled = !lightBulbEnabled;
    }

    if (pressedKeys[GLFW_KEY_8]) {
		lightSource1Enabled = !lightSource1Enabled;
    }

    if (pressedKeys[GLFW_KEY_7]) {
		lightSource2Enabled = !lightSource2Enabled;
    }
    
    if (pressedKeys[GLFW_KEY_6]) {
		lightSource3Enabled = !lightSource3Enabled;
    }

    if (pressedKeys[GLFW_KEY_1]) { 
        isWireframe = !isWireframe;
    }

    if (pressedKeys[GLFW_KEY_2]) { 
        isPointView = !isPointView;
    }

    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        updateViewMatrix();
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        updateViewMatrix();
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        updateViewMatrix();
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        updateViewMatrix();
    }

    if (pressedKeys[GLFW_KEY_DOWN]) {
        myCamera.rotate(-rotationStep, 0.0f);
        updateViewMatrix();
    }

    if (pressedKeys[GLFW_KEY_UP]) {
        myCamera.rotate(rotationStep, 0.0f);
        updateViewMatrix();
    }

    if (pressedKeys[GLFW_KEY_LEFT]) {
        myCamera.rotate(0.0f, -rotationStep);
        updateViewMatrix();
    }

    if (pressedKeys[GLFW_KEY_RIGHT]) {
        myCamera.rotate(0.0f, rotationStep);
        updateViewMatrix();
    }

	if (pressedKeys[GLFW_KEY_F]) {
		enableFog = !enableFog;
	}

	if (pressedKeys[GLFW_KEY_R]) {
		enableRain = true;
        gRain.initialize();
	}

    if (pressedKeys[GLFW_KEY_E]) {
        enableRain = false;
        gRain.cleanup();
    }

    if (pressedKeys[GLFW_KEY_EQUAL]) {  
        lightDir.x += lightSpeed; 
        lightDir.y += altitudeSpeed;  
    }

    if (pressedKeys[GLFW_KEY_MINUS]) {  
        lightDir.x -= lightSpeed; 
        lightDir.y -= altitudeSpeed;  
    }

    lightDir = glm::normalize(lightDir);

    if (lightDir.y > 1.0f) lightDir.y = 1.0f; 
    if (lightDir.y < -1.0f) lightDir.y = -1.0f; 
}

void processTour() {
    if (!isTourActive) return;

    TourPoint startPoint = tourPoints[currentPoint];
    TourPoint endPoint = tourPoints[(currentPoint + 1) % tourPoints.size()];

    glm::vec3 interpolatedPos = startPoint.position * (1.0f - t) + endPoint.position * t;
    glm::vec3 interpolatedRotation = startPoint.rotation * (1.0f - t) + endPoint.rotation * t;

    myCamera.cameraPosition = interpolatedPos;
    myCamera.rotate(interpolatedRotation.x, interpolatedRotation.y);
    myCamera.updateCameraVectors();

    updateViewMatrix();

    t += tourSpeed;

    if (t >= 1.0f) {
        t = 0.0f; 
        currentPoint = (currentPoint + 1) % tourPoints.size();
    }
}

void initOpenGLWindow() {
    myWindow.Create(1024, 768, "OpenGL Project Core");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetMouseButtonCallback(myWindow.getWindow(), mouseButtonCallback);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    //glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
    myCamera.cameraPosition = glm::vec3(0.15f, 2.00f, -5.76f);
}

void initModels() {
    scene.LoadModel("models/scene.obj");
    lightBulbModel.LoadModel("models/light_bulb/low_poly_lamp.obj");
	houseModel.LoadModel("models/house/house.obj");
    bottleModel.LoadModel("models/bottle/bottle.obj");
	glassModel.LoadModel("models/glass/glass.obj.obj");
	windowModel.LoadModel("models/window/Square_Plate_Large_obj/Square_Plate_Large_obj.obj");
	waterModel.LoadModel("models/water/water.obj");
	lanternModel.LoadModel("models/lantern/lantern.obj");
	candleModel.LoadModel("models/lantern/candle.obj");
	bladesModel.LoadModel("models/windmill/windmill_blades.obj");
	windmillBaseModel.LoadModel("models/windmill/windmill_base.obj");
}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
	skyboxShader.loadShader(
        "shaders/skyboxShader.vert",
        "shaders/skyboxShader.frag");
	rainShader.loadShader(
		"shaders/rainShader.vert",
		"shaders/rainShader.frag");
    gRain = RainSystem(1000, rainShader);
}

glm::mat4 computeLightSpaceMatrix() {
    lightDir = glm::normalize(lightDir);

    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 50.0f);

    glm::mat4 lightView = glm::lookAt(lightDir, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 lightSpaceMatrix1 = lightProjection * lightView;

    return lightSpaceMatrix1;
}

void initUniforms() {
    myBasicShader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 200.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    lightBulbPos = glm::vec3(5.0f, 5.0f, 5.0f);
    lightBulbPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightBulbPos");
    glUniform3fv(lightBulbPosLoc, 1, glm::value_ptr(lightBulbPos));

    lightBulbColor = glm::vec3(1.0f, 1.0f, 0.0f);
    lightBulbColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightBulbColor");
    glUniform3fv(lightBulbColorLoc, 1, glm::value_ptr(lightBulbColor));

    isLightBulbLoc = glGetUniformLocation(myBasicShader.shaderProgram, "isLightBulb");
    glUniform1i(isLightBulbLoc, 0);

    emissiveColor = glm::vec3(1.0f, 1.0f, 0.0f); 
    emissiveColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "emissiveColor");
    glUniform3fv(emissiveColorLoc, 1, glm::value_ptr(emissiveColor));

    enableFogLoc = glGetUniformLocation(myBasicShader.shaderProgram, "enableFog");
    glUniform1i(enableFogLoc, false); 

    fogColor = glm::vec3(0.6f, 0.6f, 0.6f); 
    fogColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogColor");
    glUniform3fv(fogColorLoc, 1, glm::value_ptr(fogColor));

    fogDensity = 0.04f; 
    fogDensityLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
    glUniform1f(fogDensityLoc, fogDensity);

    transparentLoc = glGetUniformLocation(myBasicShader.shaderProgram, "isTransparent");
    glUniform1i(transparentLoc, false);

	skyboxShader.useShaderProgram();

    enableFogLocSky = glGetUniformLocation(skyboxShader.shaderProgram, "enableFog");
    glUniform1i(enableFogLocSky, false);

    fogColor = glm::vec3(0.6f, 0.6f, 0.6f); 
    fogColorLocSky = glGetUniformLocation(skyboxShader.shaderProgram, "fogColor");
    glUniform3fv(fogColorLocSky, 1, glm::value_ptr(fogColor));
}

void renderObject(gps::Shader shader) {
    shader.useShaderProgram();


    GLint isLightBulbLoc = glGetUniformLocation(shader.shaderProgram, "isLightBulb");
    glUniform1i(isLightBulbLoc, 0);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    glUniform1i(transparentLoc, false);

    scene.Draw(shader);
}

void renderLightBulb(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lightBulbPosition);
    model = glm::scale(model, glm::vec3(3.0f));

    GLint modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    GLint lightBulbPosLoc = glGetUniformLocation(shader.shaderProgram, "lightBulbPos");
    glUniform3fv(lightBulbPosLoc, 1, glm::value_ptr(lightBulbPosition));

    GLint lightBulbColorLoc = glGetUniformLocation(shader.shaderProgram, "lightBulbColor");

    GLint emissiveColorLoc = glGetUniformLocation(shader.shaderProgram, "emissiveColor");

    if (lightBulbEnabled) {
        glUniform3fv(lightBulbColorLoc, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.5f)));  
        glUniform3fv(emissiveColorLoc, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.0f))); 
    }
    else {
        glUniform3fv(lightBulbColorLoc, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));  
        glUniform3fv(emissiveColorLoc, 1, glm::value_ptr(glm::vec3(0.1f, 0.1f, 0.1f))); 
    }

    GLint isLightBulbLoc = glGetUniformLocation(shader.shaderProgram, "isLightBulb");
    glUniform1i(isLightBulbLoc, 1);

    glUniform1i(transparentLoc, false);

    lightBulbModel.Draw(shader);
}

void renderLightSource(gps::Shader shader, glm::vec3 position, glm::vec3 color, bool enable, int lightIndex) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    GLint modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    GLint normalMatrixLoc = glGetUniformLocation(shader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    GLint lightPosLoc = 0;
    GLint lightColorLoc = 0;

    if (lightIndex == 0) {
        lightPosLoc = glGetUniformLocation(shader.shaderProgram, "lightSource1Pos");
        lightColorLoc = glGetUniformLocation(shader.shaderProgram, "lightSource1Color");
    }
    else if (lightIndex == 1) {
        lightPosLoc = glGetUniformLocation(shader.shaderProgram, "lightSource2Pos");
        lightColorLoc = glGetUniformLocation(shader.shaderProgram, "lightSource2Color");
    }
    else if (lightIndex == 2) {
        lightPosLoc = glGetUniformLocation(shader.shaderProgram, "lightSource3Pos");
        lightColorLoc = glGetUniformLocation(shader.shaderProgram, "lightSource3Color");
    }

    glUniform3fv(lightPosLoc, 1, glm::value_ptr(position));

    GLint emissiveColorLoc = glGetUniformLocation(shader.shaderProgram, "emissiveColor");

    if (enable) {
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(color));
        glUniform3fv(emissiveColorLoc, 1, glm::value_ptr(color));
    }
    else {
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
        glUniform3fv(emissiveColorLoc, 1, glm::value_ptr(glm::vec3(0.1f, 0.1f, 0.1f)));
    }

    GLint isLightBulbLoc = glGetUniformLocation(shader.shaderProgram, "isLightBulb");
    glUniform1i(isLightBulbLoc, 1);

    glUniform1i(transparentLoc, false);

    candleModel.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    normalMatrixLoc = glGetUniformLocation(shader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    isLightBulbLoc = glGetUniformLocation(shader.shaderProgram, "isLightBulb");
    glUniform1i(isLightBulbLoc, 0);

    glUniform1i(transparentLoc, false);

    lanternModel.Draw(shader);
}

void renderHouse(gps::Shader shader) {
    shader.useShaderProgram();

    GLint isLightBulbLoc = glGetUniformLocation(shader.shaderProgram, "isLightBulb");
    glUniform1i(isLightBulbLoc, 0);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, housePosition);

    GLint modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    GLint normalMatrixLoc = glGetUniformLocation(shader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    glUniform1i(transparentLoc, false);

    houseModel.Draw(shader);
}

void renderBottle(gps::Shader shader) {
    shader.useShaderProgram();

    glm::vec3 bottleTintColor = glm::vec3(1.0f, 0.0f, 0.0f); 
    GLint tintColorLoc = glGetUniformLocation(shader.shaderProgram, "tintColor");
    glUniform3fv(tintColorLoc, 1, glm::value_ptr(bottleTintColor));

    GLint isTransparentLoc = glGetUniformLocation(shader.shaderProgram, "isTransparent");
    glUniform1i(isTransparentLoc, true); 

    GLint isLightBulbLoc = glGetUniformLocation(shader.shaderProgram, "isLightBulb");
    glUniform1i(isLightBulbLoc, 0);

    glm::mat4 model = glm::mat4(1.0f);  
    model = glm::translate(model, bottlePosition);  
    model = glm::scale(model, glm::vec3(0.2f));

    GLint modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    GLint normalMatrixLoc = glGetUniformLocation(shader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    bottleModel.Draw(shader);
}

void renderGlass(gps::Shader shader) {

    shader.useShaderProgram();

    glm::vec3 glassTintColor = glm::vec3(1.0f, 0.0f, 0.0f);
    GLint tintColorLoc = glGetUniformLocation(shader.shaderProgram, "tintColor");
    glUniform3fv(tintColorLoc, 1, glm::value_ptr(glassTintColor));

    GLint isTransparentLoc = glGetUniformLocation(shader.shaderProgram, "isTransparent");
    glUniform1i(isTransparentLoc, true);

    GLint isLightBulbLoc = glGetUniformLocation(shader.shaderProgram, "isLightBulb");
    glUniform1i(isLightBulbLoc, 0); 

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glassPosition);
    model = glm::scale(model, glm::vec3(0.1f));

    GLint modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    GLint normalMatrixLoc = glGetUniformLocation(shader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    glassModel.Draw(shader);
}

void renderWindow(glm::vec3 windowPos, gps::Shader shader) {

    shader.useShaderProgram();

    glm::vec3 windowTintColor = glm::vec3(0.0f, 0.0f, 0.0f);
    GLint tintColorLoc = glGetUniformLocation(shader.shaderProgram, "tintColor");
    glUniform3fv(tintColorLoc, 1, glm::value_ptr(windowTintColor));

    GLint isTransparentLoc = glGetUniformLocation(shader.shaderProgram, "isTransparent");
    glUniform1i(isTransparentLoc, true); 

    GLint isLightBulbLoc = glGetUniformLocation(shader.shaderProgram, "isLightBulb");
    glUniform1i(isLightBulbLoc, 0); 

    glm::mat4 model = glm::mat4(1.0f);  

    model = glm::translate(model, windowPos); 

    model = glm::scale(model, glm::vec3(0.1f, 0.15f, 0.1f));

    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    GLint modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    GLint normalMatrixLoc = glGetUniformLocation(shader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    windowModel.Draw(shader);
}

void renderWindmill(gps::Shader shader, float deltaTime) {

    shader.useShaderProgram();

    GLint isLightBulbLoc = glGetUniformLocation(shader.shaderProgram, "isLightBulb");
    glUniform1i(isLightBulbLoc, 0);


    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, windmillPosition);

    GLint modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    GLint normalMatrixLoc = glGetUniformLocation(shader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));


    glUniform1i(transparentLoc, false);

    windmillBaseModel.Draw(shader);

    static float bladesAngle = 0.0f;
    bladesAngle += 90.0f * deltaTime;
    if (bladesAngle >= 360.0f) {
        bladesAngle -= 360.0f;
    }

    glm::mat4 bladesModelMatrix = glm::mat4(1.0f);
    bladesModelMatrix = glm::rotate(bladesModelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    bladesModelMatrix = glm::translate(bladesModelMatrix, windmillBladesPosition);
    bladesModelMatrix = glm::scale(bladesModelMatrix, glm::vec3(0.08f));
    bladesModelMatrix = glm::rotate(bladesModelMatrix, glm::radians(bladesAngle), glm::vec3(1.0f, 0.0f, 0.0f));

    GLint bladesModelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    glUniformMatrix4fv(bladesModelLoc, 1, GL_FALSE, glm::value_ptr(bladesModelMatrix));

    glm::mat3 bladesNormalMatrix = glm::transpose(glm::inverse(glm::mat3(bladesModelMatrix)));
    GLint bladesNormalMatrixLoc = glGetUniformLocation(shader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(bladesNormalMatrixLoc, 1, GL_FALSE, glm::value_ptr(bladesNormalMatrix));

    bladesModel.Draw(shader);
}

void renderWater(gps::Shader shader) {
    shader.useShaderProgram();

    glm::vec3 waterTintColor = glm::vec3(0.0f, 0.0f, 1.0f);
    GLint tintColorLoc = glGetUniformLocation(shader.shaderProgram, "tintColor");
    glUniform3fv(tintColorLoc, 1, glm::value_ptr(waterTintColor));

    GLint isTransparentLoc = glGetUniformLocation(shader.shaderProgram, "isTransparent");
    glUniform1i(isTransparentLoc, true);

    GLint isLightBulbLoc = glGetUniformLocation(shader.shaderProgram, "isLightBulb");
    glUniform1i(isLightBulbLoc, 0);

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, waterPosition);

    GLint modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    GLint normalMatrixLoc = glGetUniformLocation(shader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    waterModel.Draw(shader);
}

void renderScene(float deltaTime) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glm::vec3 lightColor(1.0f, 1.0f, 0.9f);

    mySkyBox.Draw(skyboxShader, view, projection);

    myBasicShader.useShaderProgram();

    if (lightEnabled) {
        GLint lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
        GLint lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    }
    else {
        lightColor = glm::vec3(0.0f, 0.0f, 0.0f);
        GLint lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    }

    if (isWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        if (isPointView) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glPointSize(5.0f);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

	if (enableFog) {
		glUniform1i(enableFogLoc, true);
	}
    else {
		glUniform1i(enableFogLoc, false);
	}

    renderHouse(myBasicShader);
    renderObject(myBasicShader);
	renderWindmill(myBasicShader, deltaTime/1000);
    renderLightBulb(myBasicShader);
	renderLightSource(myBasicShader, lightSource1Position, glm::vec3(1.0, 0.0, 0.0), lightSource1Enabled, 0);
    renderLightSource(myBasicShader, lightSource2Position, glm::vec3(0.0, 1.0, 0.0), lightSource2Enabled, 1);
    renderLightSource(myBasicShader, lightSource3Position, glm::vec3(0.0, 0.0, 1.0), lightSource3Enabled, 2);

	skyboxShader.useShaderProgram();

    if (enableFog || enableRain) {
        glUniform1i(enableFogLocSky, true);
    }
    else {
        glUniform1i(enableFogLocSky, false);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthMask(GL_FALSE);

    renderBottle(myBasicShader);
	renderGlass(myBasicShader); 
	renderWindow(windowPosition, myBasicShader);
    renderWater(myBasicShader);
    renderWindow(windowPosition2, myBasicShader);

    glDepthMask(GL_TRUE);

    glDisable(GL_BLEND);

    if (enableRain) {

        gRain.update(deltaTime);

        gRain.uploadToGPU();

        GLint isLightBulbLoc = glGetUniformLocation(myBasicShader.shaderProgram, "isLightBulb");
        glUniform1i(isLightBulbLoc, 1);

        gRain.render(projection, view);
    }
}

void cleanup() {
    glfwDestroyWindow(myWindow.getWindow());
    glfwTerminate();
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
	initModels();
	initShaders();
	initUniforms();
    setWindowCallbacks();

    faces.push_back("models/skybox/field-skyboxes/Meadow/posx.jpg");
    faces.push_back("models/skybox/field-skyboxes/Meadow/negx.jpg");
    faces.push_back("models/skybox/field-skyboxes/Meadow/posy.jpg");
    faces.push_back("models/skybox/field-skyboxes/Meadow/negy.jpg");
    faces.push_back("models/skybox/field-skyboxes/Meadow/posz.jpg");
    faces.push_back("models/skybox/field-skyboxes/Meadow/negz.jpg");
    mySkyBox.Load(faces);

	glCheckError();
    lastTimeStamp = glfwGetTime();

	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processKeyInputs();
        processTour();


        double currentTimeStamp = glfwGetTime();
        float deltaTime = static_cast<float>(currentTimeStamp - lastTimeStamp);
	    renderScene(deltaTime);


		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
