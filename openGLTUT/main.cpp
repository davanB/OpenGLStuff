//
//  main.cpp
//  openGLTUT
//
//  Created by Davan Basran on 2018-06-04.
//

#include <iostream>

//GLAD
#include <glad/glad.h>

//GLFW
#include <GLFW/glfw3.h>

#include "shader.h"
#include "camera.h"

#include <string>
#include <fstream>
#include <streambuf>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float currentAlpha = 1.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


// callback to resize the viewport to match the new dimentions after window resize.
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// process input, check if espace key was pressed to exit window.
inline void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        currentAlpha += 0.05;
        if (currentAlpha > 1.0) {
            currentAlpha = 1.0f;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        currentAlpha -= 0.05;
        if (currentAlpha < 0.0) {
            currentAlpha = 0.0f;
        }
    }
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.processKeyboard(CameraMovement::LEFT, deltaTime);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    
    camera.processMouseMovement(xOffset, yOffset);
}

void scroll_callabck(GLFWwindow* window, double xOffest, double yOffset) {
    camera.processMouseScroll(yOffset);
}

GLFWwindow* initGLFW() {
    // init and configure glfw
    if (glfwInit() != GL_TRUE) {
        std::cerr << "GLFW Failed to init" << std::endl;
        return nullptr;
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
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callabck);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // glad, load all openGL functions pointers into memory
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD" << std::endl;
        return nullptr;
    }
    glEnable(GL_DEPTH_TEST);
    
    return window;
}

unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width;
    int height;
    int nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = NULL;
        if (nrComponents == 1) {
            format = GL_RED;
        }
        else if (nrComponents == 3) {
            format = GL_RGB;
        }
        else if (nrComponents == 4) {
            format = GL_RGBA;
        }
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load texture" << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}

int main(int argc, const char * argv[]) {
    
    // init GLFW and load OpenGL functions into memory
    GLFWwindow* window = initGLFW();
    if (window == nullptr) {
        return 1;
    }
    
    // initialize our shaders
    Shader shader("/Users/davanb/Documents/School/Learning/openGLTUT/openGLTUT/vertexShader.vert",
                  "/Users/davanb/Documents/School/Learning/openGLTUT/openGLTUT/fragmentShader.frag");
    
    Shader lampShader("/Users/davanb/Documents/School/Learning/openGLTUT/openGLTUT/vertexShader.vert",
                          "/Users/davanb/Documents/School/Learning/openGLTUT/openGLTUT/lightSourceShader.frag");
    
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };
    
    // cube object has its own VAO
    unsigned int cubeVao;
    glGenVertexArrays(1, &cubeVao);
    
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(cubeVao);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // lamp object will have its own VAO
    unsigned int lampVao;
    glGenVertexArrays(1, &lampVao);
    glBindVertexArray(lampVao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    unsigned int diffuseMap = loadTexture("/Users/davanb/Documents/School/Learning/container2.png");
    unsigned int specularMap = loadTexture("/Users/davanb/Documents/School/Learning/container2_specular.png");
    
    shader.use();
    shader.setVec3("lightPos", lightPos);
    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 1);
    shader.setFloat("material.shininess", 64.0f);
    
    // render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // check if esc key was pressed
        processInput(window);
        
        // clear whatever colour was currently displayed
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.use();
        shader.setFloat("alpha", currentAlpha);
        shader.setVec3("viewPos", camera.mPosition);
        
        // directional lighting uniforms
        shader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        shader.setVec3("dirLight.lightProp.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        shader.setVec3("dirLight.lightProp.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        shader.setVec3("dirLight.lightProp.specular", glm::vec3(0.5f, 0.5f, 0.5f));

        // point lighting uniforms
        for (int i = 0; i < 4; i++)
        {
            std::string number = std::to_string(i);

            shader.setVec3("pointLights[" + number + "].position", pointLightPositions[i]);
            shader.setFloat("pointLights[" + number + "].attenuation.constant", 1.0f);
            shader.setFloat("pointLights[" + number + "].attenuation.linear", 0.09f);
            shader.setFloat("pointLights[" + number + "].attenuation.quadratic", 0.032f);
            shader.setVec3("pointLights[" + number + "].lightProp.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
            shader.setVec3("pointLights[" + number + "].lightProp.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
            shader.setVec3("pointLights[" + number + "].lightProp.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        }

        // spotlight uniforms
        shader.setVec3("spotLight.position", camera.mPosition);
        shader.setVec3("spotLight.direction", camera.mFront);
        shader.setFloat("spotLight.cutoff", glm::cos(glm::radians(12.5f)));
        shader.setFloat("spotLight.outerCutoff", glm::cos(glm::radians(17.5f)));
        shader.setFloat("spotLight.attenuation.constant", 1.0f);
        shader.setFloat("spotLight.attenuation.linear", 0.09f);
        shader.setFloat("spotLight.attenuation.quadratic", 0.032f);
        shader.setVec3("spotLight.lightProp.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        shader.setVec3("spotLight.lightProp.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("spotLight.lightProp.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        
        glm::mat4 projection = glm::perspective<float>(camera.mZoom,
                                                       static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                                                       0.1f, 100.0f);
        shader.setMat4("projection", projection);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);
        
        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        
        glBindVertexArray(cubeVao);
        for (int i = 0; i < 10; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);
            
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        
        glBindVertexArray(lampVao);
        for (int i = 0; i < 4; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lampShader.setMat4("model", model);
            
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        // poll events and swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    // cleanup GL objects
    glDeleteVertexArrays(1, &cubeVao);
    glDeleteVertexArrays(1, &lampVao);
    glDeleteBuffers(1, &vbo);
    
    glfwTerminate();
    return 0;
}
