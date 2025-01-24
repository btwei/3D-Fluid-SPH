#ifndef SOLVER_H
#define SOLVER_H

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct particle{
    glm::vec4 position;
    glm::vec4 velocity;
    glm::vec4 properties;
};

class SPH{
public:
    static constexpr float h = 0.2;
    static constexpr int particleCount = 1000;
    static constexpr std::chrono::duration<double> fixedTimeStep = std::chrono::duration<double>(1.0f / 60.0f);

    void init();
    void mainLoop();
    void cleanup();

    GLuint getBufferId();
    int getParticleCount();
    size_t getParticleSize();
private:
    std::vector<particle> particles;
    int _particleCount;
    GLuint hLocation;
    GLuint particleSSBO, gridSSBO, listSSBO;
    GLuint updateProgram;
    std::chrono::duration<double, std::nano> accumulator;
    std::chrono::time_point<std::chrono::high_resolution_clock> currentTime;
    bool firstLoop;

    void compileAndLoadShaders();
    GLuint buildShaderFromSource(const std::string& filenameComp);
    void initializeFirstLoop();
    std::vector<char> readFile(const std::string& filename);
};

#endif