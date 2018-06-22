//
//  camera.h
//  openGLTUT
//
//  Created by Davan Basran on 2018-06-21.
//

#ifndef camera_h
#define camera_h

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// used to abstract directions from windowing libraries
enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default values
const float DEFAULT_YAW = -90.0f;
const float DEFAULT_PITCH = 0.0f;
const float DEFAULT_SPEED = 2.5;
const float DEFAULT_SENSITIVITY = 0.1f;
const float DEFAULT_ZOOM = 45.0f;

class Camera {
public:
    // camera attribbutes
    glm::vec3 mPosition;
    glm::vec3 mFront;
    glm::vec3 mUp;
    glm::vec3 mRight;
    glm::vec3 mWorldUp;
    
    // Euler angles
    float mYaw;
    float mPitch;
    
    // camera options
    float mMovementSpeed;
    float mMouseSensitivity;
    float mZoom;
    
    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                                float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH)
        : mFront(glm::vec3(0.0f, 0.0f, -1.0f))
        , mMovementSpeed(DEFAULT_SPEED)
        , mMouseSensitivity(DEFAULT_SENSITIVITY)
    , mZoom(DEFAULT_ZOOM) {
        mPosition = position;
        mWorldUp = up;
        mYaw = yaw;
        mPitch = pitch;
        updateCameraVectors();
    }
    
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : mFront(glm::vec3(0.0f, 0.0f, -1.0f))
        , mMovementSpeed(DEFAULT_SPEED)
        , mMouseSensitivity(DEFAULT_SENSITIVITY)
        , mZoom(DEFAULT_ZOOM) {
        mPosition = glm::vec3(posX, posY, posZ);
        mWorldUp = glm::vec3(upX, upY, upZ);
        mYaw = yaw;
        mPitch = pitch;
        updateCameraVectors();
    }
    
    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() {
        return glm::lookAt(mPosition, mPosition + mFront, mUp);
    }
    
    // Processes input received from any keyboard-like input system.
    void processKeyboard(CameraMovement direction, float deltaTime)
    {
        float velocity = mMovementSpeed * deltaTime;
        if (direction == CameraMovement::FORWARD)
            mPosition += mFront * velocity;
        if (direction == CameraMovement::BACKWARD)
            mPosition -= mFront * velocity;
        if (direction == CameraMovement::LEFT)
            mPosition -= mRight * velocity;
        if (direction == CameraMovement::RIGHT)
            mPosition += mRight * velocity;
    }
    
    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true)
    {
        xOffset *= mMouseSensitivity;
        yOffset *= mMouseSensitivity;
        
        mYaw   += xOffset;
        mPitch += yOffset;
        
        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (mPitch > 89.0f)
                mPitch = 89.0f;
            if (mPitch < -89.0f)
                mPitch = -89.0f;
        }
        
        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }
    
    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void processMouseScroll(float yOffset)
    {
        if (mZoom >= 1.0f && mZoom <= 45.0f)
            mZoom -= yOffset;
        if (mZoom <= 1.0f)
            mZoom = 1.0f;
        if (mZoom >= 45.0f)
            mZoom = 45.0f;
    }
private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        front.y = sin(glm::radians(mPitch));
        front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        mFront = glm::normalize(front);
        
        // Also re-calculate the Right and Up vector
        mRight = glm::normalize(glm::cross(mFront, mWorldUp));
        mUp = glm::normalize(glm::cross(mRight, mFront));
    }

};


#endif /* camera_h */
