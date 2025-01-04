#ifndef WINDOW_H
#define WINDOW_H

#include <stdexcept>

#include <GLFW/glfw3.h>

class Window{
public:
    void init(int width, int height, const char* title);
    void cleanup();

    void makeContextCurrent();
    bool shouldClose();
    void pollEvents();

    GLFWwindow* getGLFWWindow();
    
private:
    GLFWwindow* _window = nullptr;
};

#endif