#include "Solver.h"

void SPH::init(){
    _particleCount = particleCount;

    //Initialize Random Particles
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    particles = std::vector<particle>(particleCount);
    
    for (particle& p : particles){
        p.position = glm::vec3(dist(mt), dist(mt), dist(mt));
        p.velocity = glm::vec3(dist(mt) * 0.1, dist(mt) * 0.1, dist(mt) * 0.1);
    }

    //Derive Grid Dimensions from h (needs to be at least h x h per grid box)
    size_t gridLength = ceil(2.0 / h);
    size_t gridSize = gridLength * gridLength * gridLength;

    //Check if GLAD properly initialized
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glGenBuffers(1, &particleSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, particles.size() * sizeof(particle), particles.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleSSBO);

    glGenBuffers(1, &gridSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, gridSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, gridSize * sizeof(uint), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, gridSSBO);

    glGenBuffers(1, &listSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, listSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, particleCount * sizeof(uint), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, listSSBO);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    compileAndLoadShaders();

    hLocation = glGetUniformLocation(updateProgram, "h");
    glUniform1f(hLocation, h);
}

void SPH::mainLoop() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleSSBO);
    glUseProgram(updateProgram);
    glDispatchCompute((_particleCount + 255) / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void SPH::cleanup(){
    glDeleteBuffers(1, &particleSSBO);
    glDeleteBuffers(1, &gridSSBO);
    glDeleteBuffers(1, &listSSBO);
    glDeleteProgram(updateProgram);
}

GLuint SPH::getBufferId(){
    return particleSSBO;
}

int SPH::getParticleCount(){
    return _particleCount;
}

void SPH::compileAndLoadShaders(){
    updateProgram = buildShaderFromSource("../shaders/update.comp");
}

GLuint SPH::buildShaderFromSource(const std::string& filenameComp){
    //Load compute shader from file and compile via OpenGL
    std::vector<char> compFile = readFile(filenameComp);
    compFile.push_back('\0');
    const GLchar* computeShaderSource = compFile.data();

    GLuint computeShader;
    computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &computeShaderSource, NULL);
    glCompileShader(computeShader);

    //Check for errors
    int success;
    char infoLog[512];
    glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(computeShader, 512, NULL, infoLog);
        throw std::runtime_error("Compute shader failed to compile:\n" + std::string(infoLog));
    }

    //Create a shaderProgram
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, computeShader);
    glLinkProgram(shaderProgram);

    //Check for errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        throw std::runtime_error("Program failed to link shaders:\n" + std::string(infoLog));
    }

    glDeleteShader(computeShader);

    return shaderProgram;
}

std::vector<char> SPH::readFile(const std::string& filename) {
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