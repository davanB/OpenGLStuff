//
//  shader.h
//  openGLTUT
//
//  Created by Davan Basran on 2018-06-07.
//

#ifndef shader_h
#define shader_h

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    // shader program ID
    unsigned int programID;
    
    // constructor reads and compiles the shaders
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {
        // 1. Read the source files from disk
        std::ifstream vertexShader(vertexPath);
        std::ifstream fragmentShader(fragmentPath);
        if(!fragmentShader.is_open() || !vertexShader.is_open()){
            std::cerr << "Cannot find fragment shader file" << std::endl;
        }
        
        std::string vertexShaderSrc(std::istreambuf_iterator<char>(vertexShader), (std::istreambuf_iterator<char>()));
        const char* vertexShaderSrcPtr = vertexShaderSrc.c_str();
        
        std::string fragmentShaderSrc(std::istreambuf_iterator<char>(fragmentShader), (std::istreambuf_iterator<char>()));
        const char* fragmentShaderSrcPtr = fragmentShaderSrc.c_str();
        
        // 2. Compile the shader source code
        
        // compile the vertex shader code
        unsigned int vertextShaderID = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertextShaderID, 1, &vertexShaderSrcPtr, NULL);
        glCompileShader(vertextShaderID);
        checkCompileErrors(programID, false);
        
        // compile the fragment shader code
        unsigned int fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShaderID, 1, &fragmentShaderSrcPtr, NULL);
        glCompileShader(fragmentShaderID);
        checkCompileErrors(programID, false);
        
        // 3. Link the shader program
        programID = glCreateProgram();
        glAttachShader(programID, vertextShaderID);
        glAttachShader(programID, fragmentShaderID);
        glLinkProgram(programID);
        checkCompileErrors(programID, true);
        
        // cleanup shaders now that they are linked to program
        glDeleteShader(vertextShaderID);
        glDeleteShader(fragmentShaderID);
    }
    
    // activate the shader
    void use() {
        glUseProgram(programID);
    }
    
    // utility functions
    void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(programID, name.c_str()), static_cast<int>(value));
    }
    
    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
    }
    
    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
    }
    
    void setMat4(const std::string& name, const glm::mat4& matrix) const {
        unsigned int loc = glGetUniformLocation(programID, name.c_str());
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
    }
    
    void setVec3(const std::string& name, glm::vec3 vec3) const {
        unsigned int loc = glGetUniformLocation(programID, name.c_str());
        glUniform3fv(loc, 1, glm::value_ptr(vec3));
    }

private:
    void checkCompileErrors(unsigned int shaderID, bool isProgram) {
        int success;
        unsigned int errorBuffSize = 512;
        char infoLog[errorBuffSize];
        
        if (isProgram) {
            glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(programID, errorBuffSize, NULL, infoLog);
                std::cerr << "ERROR SHADER COMPILATION FAILED" << std::endl << infoLog << std::endl;
            }
        }
        else {
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shaderID, errorBuffSize, NULL, infoLog);
                std::cerr << "ERROR SHADER PROGRAM LINKING FAILED" << std::endl << infoLog << std::endl;
            }
        }
    }
};

#endif /* shader_h */
