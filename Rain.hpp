#ifndef RAIN_HPP
#define RAIN_HPP

#include <vector>
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "Shader.hpp"

#include <vector>
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "Shader.hpp"

struct RainDrop {
    glm::vec3 position;
    glm::vec3 velocity;
    float delayUntilFall;
    bool isActive;
};

class RainSystem {
public:
    RainSystem(int maxRaindrops, gps::Shader shaderProgram);

    void initialize();
    void update(float deltaTime);
    void uploadToGPU();

    void render(const glm::mat4& projection, const glm::mat4& view);

    void cleanup();

private:
    glm::vec3 generateRandomPositionAbove();

    gps::Shader shader;
    std::vector<RainDrop> raindrops;

    GLuint vao = 0;
    GLuint vbo = 0;
    int maxRaindrops;
    bool isInitialized;
};

#endif
