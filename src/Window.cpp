#include "Window.h"

void Window::init(int width, int height, const char* title){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if(_window == NULL){
        throw std::runtime_error("Failed to create GLFW window");
        glfwTerminate();
    }

    glfwMakeContextCurrent(_window);
}

GLFWwindow* Window::getGLFWWindow(){
    return _window;
}

bool Window::shouldClose(){
    return glfwWindowShouldClose(_window);
}

void Window::pollEvents(){
    glfwPollEvents();
}

void Window::cleanup(){
    glfwDestroyWindow(_window);

    glfwTerminate();
}