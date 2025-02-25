#version 410 core

in vec3 textureCoordinates;
out vec4 color;

uniform samplerCube skybox; 
uniform bool enableFog;      
uniform vec3 fogColor;  

void main()
{
    if (enableFog) {
        color = vec4(fogColor, 1.0f); 
    } else {
        color = texture(skybox, textureCoordinates);
    }
}
