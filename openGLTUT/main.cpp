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
#include "model.h"

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
    
//    Model model("/Users/davanb/Documents/School/Learning/nanosuit/nanosuit.obj");
    Model model("/Users/davanb/Documents/School/Learning/sponza_obj/sponza.obj");
    
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
        shader.setFloat("material.shininess", 32.0f);
        
        shader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        shader.setVec3("dirLight.lightProp.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        shader.setVec3("dirLight.lightProp.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        shader.setVec3("dirLight.lightProp.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        
        shader.setVec3("pointLights[0].position", glm::vec3( 0.7f,  0.2f,  2.0f));
        shader.setVec3("pointLights[0].lightProp.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        shader.setVec3("pointLights[0].lightProp.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        shader.setVec3("pointLights[0].lightProp.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setFloat("pointLights[0].attenuation.constant", 1.0f);
        shader.setFloat("pointLights[0].attenuation.linear", 0.09);
        shader.setFloat("pointLights[0].attenuation.quadratic", 0.032);
        
        shader.setVec3("pointLights[1].position", glm::vec3( 2.3f, -3.3f, -4.0f));
        shader.setVec3("pointLights[1].lightProp.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        shader.setVec3("pointLights[1].lightProp.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        shader.setVec3("pointLights[1].lightProp.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setFloat("pointLights[1].attenuation.constant", 1.0f);
        shader.setFloat("pointLights[1].attenuation.linear", 0.09);
        shader.setFloat("pointLights[1].attenuation.quadratic", 0.032);
        
        shader.setVec3("spotLight.position", camera.mPosition);
        shader.setVec3("spotLight.direction", camera.mFront);
        shader.setVec3("spotLight.lightProp.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        shader.setVec3("spotLight.lightProp.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("spotLight.lightProp.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setFloat("spotLight.attenuation.constant", 1.0f);
        shader.setFloat("spotLight.attenuation.linear", 0.09);
        shader.setFloat("spotLight.attenuation.quadratic", 0.032);
        shader.setFloat("spotLight.cutoff", glm::cos(glm::radians(12.5f)));
        shader.setFloat("spotLight.outerCutoff", glm::cos(glm::radians(15.0f)));

        glm::mat4 projection = glm::perspective<float>(camera.mZoom,
                                                       static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                                                       0.1f, 5000.0f);
        shader.setMat4("projection", projection);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);
        
        // render model
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(0.0, -1.75f, 0.0f));
        modelMat = glm::scale(modelMat, glm::vec3(0.2f, 0.2f, 0.2f));
        shader.setMat4("model" ,modelMat);
        model.draw(shader);
        
        // poll events and swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}
