#include "Renderer.h"

void Renderer::init(GLFWwindow* window, SPH* solver) {
    _window = window;
    _solver = solver;

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        throw std::runtime_error("Failed to initialize GLAD");
    }

    configureBuffers();
    compileAndLoadShaders();

    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void Renderer::mainLoop() {
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);
    glUseProgram(pointsProgram);
    glDrawArrays(GL_POINTS, 0, _solver->getParticleCount());
    glBindVertexArray(0);

    glfwSwapBuffers(_window);
}

void Renderer::cleanup() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(pointsProgram);
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Renderer::configureBuffers() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _solver->getBufferId());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::compileAndLoadShaders(){
    pointsProgram = buildShaderFromSource("../shaders/points.vert", "../shaders/points.frag");
}

GLuint Renderer::buildShaderFromSource(const std::string& filenameVert, const std::string& filenameFrag){
    //Load vertex shader from file and compile via OpenGL
    std::vector<char> vertFile = readFile(filenameVert);
    vertFile.push_back('\0');
    const GLchar* vertexShaderSource = vertFile.data();

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    //Check for errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        throw std::runtime_error("Vertex shader failed to compile:\n" + std::string(infoLog));
    }

    //Load fragment shader from file and compile via OpenGL
    std::vector<char> fragFile = readFile(filenameFrag);
    fragFile.push_back('\0');
    const GLchar* fragmentShaderSource = fragFile.data();

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    //Check for errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        throw std::runtime_error("Fragment shader failed to compile:\n" + std::string(infoLog));
    }

    //Combine shaders into a shaderProgram
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    //Check for errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        throw std::runtime_error("Program failed to link shaders:\n" + std::string(infoLog));
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

std::vector<char> Renderer::readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file!");
	}

	size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}