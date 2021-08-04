#pragma once
#include <vector>
#include <iostream>
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Generic.h"
#include "Model.h"
#include "Shader.h"
#include "UI.h"

extern double currentFrame, lastFrame, deltaTime;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
    GLsizei length, const char* message, const void* userParam);

void processInput(GLFWwindow* window, Camera& camera);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);