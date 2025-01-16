#ifndef SOLVER_H
#define SOLVER_H

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
    glm::vec3 position;
    glm::vec3 velocity;
};

class SPH{
public:
    void init(int particleCount);
    void mainLoop();
    void cleanup();

    GLuint getBufferId();
    int getParticleCount();
private:
    std::vector<particle> particles;
    int _particleCount;
    GLuint particleSSBO;
    GLuint updateProgram;

    void compileAndLoadShaders();
    GLuint buildShaderFromSource(const std::string& filenameComp);
    std::vector<char> readFile(const std::string& filename);
};

#endif