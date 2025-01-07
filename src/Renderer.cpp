#include "Renderer.h"

void Renderer::init(GLFWwindow* window) {
    _window = window;

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        throw std::runtime_error("Failed to initialize GLAD");
    }

    loadGeometry();
    compileAndLoadShaders();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void Renderer::mainLoop() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(fluidProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(_window);
}

void Renderer::cleanup() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &propertyVBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(fluidProgram);
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Renderer::loadGeometry() {
    generateGrid();
    generateProperties();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &propertyVBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, propertyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * properties.size(), properties.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::generateGrid() {

    for(float i=0; i <= 60; i++){
        for(float j=0; j <= 80; j++){
            vertices.push_back(j/40.0f-1.0f);
            vertices.push_back(1.0f-i/30.0f);;
        }
    }

    for(int i=0; i < 60; i++) {
        for(int j=0; j < 80; j++) {
            unsigned int tl = j+i*81;
            unsigned int tr = j+i*81+1;
            unsigned int bl = j+(i+1)*81;
            unsigned int br = j+(i+1)*81+1;

            indices.push_back(tr);
            indices.push_back(tl);
            indices.push_back(bl);

            indices.push_back(tr);
            indices.push_back(bl);
            indices.push_back(br);
        }
    }

}

void Renderer::generateProperties() {
    for(float i=0; i <= 60; i++){
        for(float j=0; j <= 80; j++){
            properties.push_back(0.5f);
            properties.push_back(0.0f);
        }
    }
}

void Renderer::compileAndLoadShaders(){
    fluidProgram = buildShaderFromSource("../shaders/fluid.vert", "../shaders/fluid.frag");
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