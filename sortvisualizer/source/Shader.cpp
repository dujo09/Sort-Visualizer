#include <fstream>
#include <sstream>
#include <iostream>

#include "../include/Shader.h"


Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open files
        vShaderFile.open(vertexShaderPath);
        fShaderFile.open(fragmentShaderPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n";
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    unsigned int vertexShaderId, fragmentShaderId;
    int success;
    char infoLog[512];

    // vertex Shader
    vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderId, 1, &vShaderCode, NULL);
    glCompileShader(vertexShaderId);
    checkCompileErrors(vertexShaderId, "VERTEX");
    // fragment Shader
    fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderId, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShaderId);
    checkCompileErrors(fragmentShaderId, "FRAGMENT");

    // shader Program
    m_ShaderProgramId = glCreateProgram();
    glAttachShader(m_ShaderProgramId, vertexShaderId);
    glAttachShader(m_ShaderProgramId, fragmentShaderId);
    glLinkProgram(m_ShaderProgramId);
    checkCompileErrors(m_ShaderProgramId, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
}

Shader::~Shader() {
    glDeleteProgram(m_ShaderProgramId);
}

void Shader::use() {
    glUseProgram(m_ShaderProgramId);
}

void Shader::setBool(const char* name, bool value) const 
{
    glUniform1i(glGetUniformLocation(m_ShaderProgramId, name), (int)value);
}

void Shader::setInt(const char* name, int value) const 
{
    glUniform1i(glGetUniformLocation(m_ShaderProgramId, name), value);
}

void Shader::setFloat(const char* name, float value) const 
{
    glUniform1f(glGetUniformLocation(m_ShaderProgramId, name), value);
}

void Shader::setVec2(const char* name, const glm::vec2& value) const 
{
    glUniform2fv(glGetUniformLocation(m_ShaderProgramId, name), 1, &value[0]);
}
void Shader::setVec2(const char* name, float x, float y) const 
{
    glUniform2f(glGetUniformLocation(m_ShaderProgramId, name), x, y);
}

void Shader::setVec3(const char* name, const glm::vec3& value) const 
{
    glUniform3fv(glGetUniformLocation(m_ShaderProgramId, name), 1, &value[0]);
}
void Shader::setVec3(const char* name, float x, float y, float z) const 
{
    glUniform3f(glGetUniformLocation(m_ShaderProgramId, name), x, y, z);
}

void Shader::setVec4(const char* name, const glm::vec4& value) const 
{
    glUniform4fv(glGetUniformLocation(m_ShaderProgramId, name), 1, &value[0]);
}
void Shader::setVec4(const char* name, float x, float y, float z, float w) const 
{
    glUniform4f(glGetUniformLocation(m_ShaderProgramId, name), x, y, z, w);
}

void Shader::setMat2(const char* name, const glm::mat2& mat) const 
{
    glUniformMatrix2fv(glGetUniformLocation(m_ShaderProgramId, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const char* name, const glm::mat3& mat) const 
{
    glUniformMatrix3fv(glGetUniformLocation(m_ShaderProgramId, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const char* name, const glm::mat4& mat) const 
{
    glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgramId, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::checkCompileErrors(GLuint shaderId, std::string type) 
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shaderId, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- \n" << std::endl;
        }
    } else {
        glGetProgramiv(shaderId, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderId, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- \n" << std::endl;
        }
    }
}
