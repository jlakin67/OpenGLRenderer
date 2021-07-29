#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLfloat SENSITIVITY = 1.0f;
const GLfloat SPEED = 8.0f;
const GLfloat ZOOM = 45.0f;

class Camera {
public:
    Camera(glm::vec3 pos_ = glm::vec3(0.0f), GLfloat pitch_ = 0.0f, GLfloat yaw_ = 90.0f, GLfloat sensitivity_ = SENSITIVITY, GLfloat speed_ = SPEED) :
        pos{ pos_ }, pitch{ pitch_ }, yaw{ yaw_ }, sensitivity{ sensitivity_ }, speed{ speed_ }
    {
        //default front is toward z-axis
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    }
    void processKeyboard(int key, GLfloat deltaTime);
    void processMouse(GLFWwindow* window, double deltaX, double deltaY);
    glm::vec3 pos, front;
    GLfloat pitch, yaw, sensitivity, speed;
};