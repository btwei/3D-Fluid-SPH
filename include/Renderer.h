#ifndef RENDERER_H
#define RENDERER_H

#include <stdexcept>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Renderer{
public:
    void init(GLFWwindow* window);
    void mainLoop();
private:
    GLFWwindow* _window;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};

#endif