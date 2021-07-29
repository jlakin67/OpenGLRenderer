#include "Camera.h"

void Camera::processMouse(GLFWwindow* window, double deltaX, double deltaY)
{
    //if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
    //    deltaX = 0;
    //    deltaY = 0;
    //}
    //double xpos, ypos;
    //glfwGetCursorPos(window, &xpos, &ypos);
    //if (xpos == lastX) deltaX = 0;
    //if (ypos == lastY) deltaY = 0;
    yaw += SENSITIVITY * ((float)deltaX);
    pitch -= SENSITIVITY * ((float)deltaY);
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
}

void Camera::processKeyboard(int key, GLfloat deltaTime) {
    GLfloat velocity = speed * deltaTime;
    if (key == GLFW_KEY_W) pos += front * velocity;
    else if (key == GLFW_KEY_S) pos -= front * velocity;
    else {
        glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
        if (key == GLFW_KEY_D) pos += right * velocity;
        if (key == GLFW_KEY_A) pos -= right * velocity;
    }
}