#include "CSJGLProgram.h"

#include <iostream>

#include "CSJCommonTool/CSJCommonTool.h"

using std::string;

CSJGLProgram::CSJGLProgram() {

}

CSJGLProgram::~CSJGLProgram() {
}

CSJGLProgram::CSJGLProgram(std::string & vertexShader, std::string & fragmentShader, bool createByFile) {
    std::string vertexShaderString = "";
    std::string fragmentShaderString = "";
    if (createByFile) {
        vertexShaderString = CSJCommonTool::readFile(vertexShader);
        fragmentShaderString = CSJCommonTool::readFile(fragmentShader);
    } else {
        vertexShaderString = vertexShader;
        fragmentShaderString = fragmentShader;
    }

    bool vertexCreated = createVertexShader(vertexShaderString);
    bool fragmentCreated = createFragmentShader(fragmentShaderString);

    if (vertexCreated && fragmentCreated) {
        GLuint vfprogram = glCreateProgram();
        glAttachShader(vfprogram, this->m_vVertexShader);
        glAttachShader(vfprogram, this->m_vFragmentShader);

        glLinkProgram(vfprogram);
        this->m_vRenderingProgram = vfprogram;
        m_bProgramUsable = true;
    } else {
        // Create program failed.
    }
}

void CSJGLProgram::useProgram() {
    glUseProgram(m_vRenderingProgram);
}

bool CSJGLProgram::createVertexShader(const std::string & vertexShaderString) {
    bool createVertShaderRes = false;
    if (vertexShaderString.size() == 0) {
        return createVertShaderRes;
    }

    GLint success;
    GLchar infoLog[512];

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertShaderSrc = vertexShaderString.c_str();
    glShaderSource(vertexShader, 1, &vertShaderSrc, NULL);
    glCompileShader(vertexShader);
    // get the errors;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << std::endl;
    } else {
        createVertShaderRes = true;
        this->m_vVertexShader = vertexShader;
    }

    return createVertShaderRes;
}

bool CSJGLProgram::createFragmentShader(const std::string & fragmentShaderString) {
    bool createFragShaderRes = false;
    if (fragmentShaderString.size() == 0) {
        return createFragShaderRes;
    }

    GLint success;
    GLchar infoLog[512];

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fragShaderSrc = fragmentShaderString.c_str();
    glShaderSource(fragmentShader, 1, &fragShaderSrc, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << std::endl;
    } else {
        createFragShaderRes = true;
        this->m_vFragmentShader = fragmentShader;
    }

    return createFragShaderRes;
}
