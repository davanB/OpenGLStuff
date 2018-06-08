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

#include "shader.h"

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

void draw(GLFWwindow* window, unsigned int vao) {
    // check if esc key was pressed
    processInput(window);
    
    // clear whatever colour was currently displayed
    glClear(GL_COLOR_BUFFER_BIT);
    
    // draw the 2 triangles to make a rectangle
    glBindVertexArray(vao);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // poll events and swap buffers
    glfwPollEvents();
    glfwSwapBuffers(window);
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
    
    Shader shader("/Users/davanb/Documents/School/Learning/openGLTUT/openGLTUT/vertexShader.vert", "/Users/davanb/Documents/School/Learning/openGLTUT/openGLTUT/fragmentShader.frag");
    
    // 4 points of a rectangle
    float verticies[] = {
        // position         colour
         0.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
        -0.5f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
        // second triangle
        0.0f,  0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
        0.5f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
        1.0f,  0.0f, 0.0f,   0.0f, 0.0f, 1.0f
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // safe to unbind buffer since call to glVertexAttribPointer registers vbo as attributes VBO.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // render loop
    while (!glfwWindowShouldClose(window)) {
        shader.use();
        draw(window, vao);
    }
    
    // cleanup GL objects
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    
    glfwTerminate();
    return 0;
}
