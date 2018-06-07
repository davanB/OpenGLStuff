//
//  main.cpp
//  openGLTUT
//
//  Created by Davan Basran on 2018-06-04.
//  Copyright © 2018 Davan Basran. All rights reserved.
//

#include <iostream>

//GLAD
#include <glad/glad.h>

//GLFW
#include <GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <streambuf>
#include <cmath>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// callback to resize the viewport to match the new dimentions after window resize.
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// process input, check if espace key was pressed to exit window.
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

// helper function to read shader code from external files into strings
static std::string loadShaderFile(const std::string& fileName) {
    std::ifstream sourceFileShader(fileName);
    if(!sourceFileShader.is_open()){
        std::cerr << "Cannot find shader file" << std::endl;
        throw;
    }
    std::string shaderSrc(std::istreambuf_iterator<char>(sourceFileShader), (std::istreambuf_iterator<char>()));
    
    return shaderSrc;
}

int main(int argc, const char * argv[]) {
    // init and configure glfw
    if (glfwInit() != GL_TRUE) {
        std::cerr << "GLFW Failed to init" << std::endl;
        return 1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Davan's Window", NULL, NULL);
    if (!window) {
        std::cout << "failed to create glfw window" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // glad, load all openGL functions pointers into memory
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD" << std::endl;
        return 1;
    }
    
    // read in the source code for the vertext shader
    std::string vertexShaderSrc = loadShaderFile("/Users/davanb/Documents/School/Learning/openGLTUT/openGLTUT/vertexShader.vert");
    const char* vertexShaderSrcPtr = vertexShaderSrc.c_str();
    
    // read in the source code for the fragment shader
    std::string fragmentShaderSrc = loadShaderFile("/Users/davanb/Documents/School/Learning/openGLTUT/openGLTUT/fragmentShader.frag");
    const char* fragmentShaderSrcPtr = fragmentShaderSrc.c_str();
    
    // compile the vertex shader code
    unsigned int vertextShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertextShader, 1, &vertexShaderSrcPtr, NULL);
    glCompileShader(vertextShader);
    
    int success;
    unsigned int errorBuffSize = 512;
    char infoLog[errorBuffSize];
    
    glGetShaderiv(vertextShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertextShader, errorBuffSize, NULL, infoLog);
        std::cerr << "ERROR VERTEX SHADER COMPILATION FAILED" << std::endl << infoLog << std::endl;
        return 1;
    }
    
    // compile the fragment shader code
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrcPtr, NULL);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(vertextShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, errorBuffSize, NULL, infoLog);
        std::cerr << "ERROR FRAGMENT SHADER COMPILATION FAILED" << std::endl << infoLog << std::endl;
        return 1;
    }
    
    // link shaders to program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertextShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, errorBuffSize, NULL, infoLog);
        std::cerr << "ERROR SHADER PROGRAM LINKING FAILED" << std::endl << infoLog << std::endl;
        return 1;
    }
    
    // cleanup shaders now that they are linked to program
    glDeleteShader(vertextShader);
    glDeleteShader(fragmentShader);
    
    // 4 points of a rectangle
    float verticies[] = {
         0.0f,  0.0f, 0.0f,
         -0.5f, 1.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        // second triangle
        0.5f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f
    };
    
    // construct 2 triangles to form a rectangle
    unsigned int indicies[] = {
        0, 1, 2, // first triangle
        0, 3, 4  // second triangle
    };
    
    // generate a vertex array object
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    
    // generate a vertex buffer object
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    
    // generate element buffer object
    unsigned int ebo;
    glGenBuffers(1, &ebo);
    
    // 1. bind the vertex array object first
    glBindVertexArray(vao);
    
    // 2. bind and set vertex buffer objects
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
    
    // 3. bind and set element buffer obbjects
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
    
    // 4. configure vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // safe to unbind buffer since call to glVertexAttribPointer registers vbo as attributes VBO.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // render loop
    while (!glfwWindowShouldClose(window)) {
        // check if esc key was pressed
        processInput(window);
        
        // clear whatever colour was currently displayed
        glClear(GL_COLOR_BUFFER_BIT);
        
        // draw the 2 triangles to make a rectangle
        glUseProgram(shaderProgram);
        
        float time = glfwGetTime();
        float red = sin(time)/2.0f + 0.5f;
        float green = sin(time)/2.0f + 0.3f;
        float blue = sin(time)/2.0f + 0.5f;
        int vertexColourLocation = glGetUniformLocation(shaderProgram, "ourColour");
        glUniform4f(vertexColourLocation, red, green, blue, 1.0f);
        
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        // poll events and swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    // cleanup GL objects
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    
    glfwTerminate();
    return 0;
}
