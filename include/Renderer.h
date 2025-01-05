#ifndef RENDERER_H
#define RENDERER_H

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Renderer{
public:
    void init(GLFWwindow* window);
    void mainLoop();
    void cleanup();
private:
    GLFWwindow* _window;

    GLuint VBO, EBO, VAO = 0;
    GLuint fluidProgram = 0;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void loadGeometry();
    void compileAndLoadShaders();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static std::vector<char> readFile(const std::string& filename);

    void generateGrid();
    GLuint buildShaderFromSource(const std::string& filenameFrag, const std::string& filenameVert);
};

#endif