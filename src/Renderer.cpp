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

    viewMatrix = glm::mat4(1.0f);
    viewMatrixLocation = glGetUniformLocation(pointsProgram, "viewMatrix");
}

void Renderer::mainLoop() {
    glClear(GL_COLOR_BUFFER_BIT);

    switch(renderMode){
        case RENDER_POINTS:
            glBindVertexArray(VAO);
            glPointSize(5.0f);
            glUseProgram(pointsProgram);
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
            glDrawArrays(GL_POINTS, 0, _solver->getParticleCount());
            glBindVertexArray(0);

            break;
        case RENDER_SSFR:
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glViewport(0, 0, _width, _height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glPointSize(5.0f);
            glUseProgram(pointsProgram);
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
            glBindVertexArray(VAO);
            glDrawArrays(GL_POINTS, 0, _solver->getParticleCount());
            glBindVertexArray(0);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glUseProgram(ssfrProgram);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, colorTexture);
            glUniform1i(glGetUniformLocation(ssfrProgram, "inputTexture"), 0);

            glBindVertexArray(quadVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            break;
        default:
            break;
    }

    glfwSwapBuffers(_window);
}

void Renderer::cleanup() {
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &quadEBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteProgram(pointsProgram);
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    _width = width;
    _height = height;
    glViewport(0, 0, width, height);
}

void Renderer::configureBuffers() {
    //Setup Particle VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _solver->getBufferId());
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, _solver->getParticleSize(), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, _solver->getParticleSize(), (void*)32);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Setup Quad VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &quadEBO);

    glBindVertexArray(quadVAO);

    float quadVertices[] = {
        // Positions // Tex Coords
        -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };

    unsigned int quadIndices[] = {
        0, 1, 2,
        0, 2, 3
    };

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer not complete!\n" << status << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::compileAndLoadShaders(){
    pointsProgram = buildShaderFromSource("../shaders/points.vert", "../shaders/points.frag");
    ssfrProgram = buildShaderFromSource("../shaders/ssfr.vert", "../shaders/ssfr.frag");
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