#include "Rain.hpp"

#include <cstdlib>
#include <GL/glew.h>
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include "Shader.hpp"

RainSystem::RainSystem(int maxRaindrops, gps::Shader shaderProgram)
    : maxRaindrops(maxRaindrops), shader(shaderProgram), isInitialized(false)
{
    raindrops.resize(maxRaindrops);
}

void RainSystem::initialize() {
    if (isInitialized) return;

    for (int i = 0; i < maxRaindrops; i++) {
        raindrops[i].position = generateRandomPositionAbove();

        float speed = 300.0f + static_cast<float>(rand() % 61);
        raindrops[i].velocity = glm::vec3(0.0f, -speed, 0.0f);

        raindrops[i].delayUntilFall = static_cast<float>(rand() % 10000) / 1000.0f;
        raindrops[i].isActive = false;
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER,
        2 * maxRaindrops * sizeof(glm::vec3),
        nullptr,
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    isInitialized = true;
}

void RainSystem::update(float deltaTime) {
    if (!isInitialized) return;

    for (int i = 0; i < maxRaindrops; i++) {
        auto& drop = raindrops[i];

        if (!drop.isActive) {
            drop.delayUntilFall -= deltaTime;
            if (drop.delayUntilFall <= 0.0f) {
                drop.isActive = true;
            }
        }

        if (drop.isActive) {
            drop.position += drop.velocity * deltaTime;

            if (drop.position.y < -1.5f) {
                drop.position = generateRandomPositionAbove();

                float speed = 200.0f + static_cast<float>(rand() % 201);
                drop.velocity = glm::vec3(0.0f, -speed, 0.0f);

                drop.delayUntilFall = static_cast<float>(rand() % 3000) / 1000.0f;
                drop.isActive = false;
            }
        }
    }
}

void RainSystem::uploadToGPU() {
    if (!isInitialized) return;

    std::vector<glm::vec3> dropData(2 * maxRaindrops);

    for (int i = 0; i < maxRaindrops; i++) {
        glm::vec3 start = raindrops[i].position;

        float dropLength = glm::length(raindrops[i].velocity) / 50.0f;
        glm::vec3 end = start + glm::vec3(0.0f, -dropLength, 0.0f);

        dropData[2 * i] = start;
        dropData[2 * i + 1] = end;
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferSubData(GL_ARRAY_BUFFER,
        0,
        dropData.size() * sizeof(glm::vec3),
        dropData.data());

    glBindVertexArray(0);
}

void RainSystem::render(const glm::mat4& projection, const glm::mat4& view) {
    if (!isInitialized) return;

    shader.useShaderProgram();

    GLint rainColorLoc = glGetUniformLocation(shader.shaderProgram, "rainColor");
    glUniform3f(rainColorLoc, 0.6f, 0.6f, 1.0f);

    GLint projLoc = glGetUniformLocation(shader.shaderProgram, "projection");
    GLint viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, 2 * maxRaindrops);
    glBindVertexArray(0);
}

void RainSystem::cleanup() {
    if (!isInitialized) return;

    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    if (vbo) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }

    raindrops.clear();
    raindrops.resize(maxRaindrops);
    isInitialized = false;
}

glm::vec3 RainSystem::generateRandomPositionAbove() {
    float minX = -20.0f;
    float maxX = 70.0f;
    float minZ = -60.0f;
    float maxZ = 30.0f;

    float x = minX + static_cast<float>(rand()) / RAND_MAX * (maxX - minX);
    float z = minZ + static_cast<float>(rand()) / RAND_MAX * (maxZ - minZ);
    float y = 6.0f + static_cast<float>(rand() % 10);

    return glm::vec3(x, y, z);
}