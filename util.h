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

const GLfloat cube_vertices[]{
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
};

extern GLuint SCR_WIDTH, SCR_HEIGHT;
extern double currentFrame, lastFrame, deltaTime;
extern GLint renderMode, matrixMode;
extern int renderCascadeFrustum;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
    GLsizei length, const char* message, const void* userParam);

void processInput(GLFWwindow* window, Camera& camera);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

void drawLights(std::vector<glm::vec4>& positions, std::vector<glm::vec4>& colors, Generic& generic, 
    Shader& shader);

void drawLights(std::vector<glm::vec4>& positions, std::vector<glm::vec4>& colors, Model& model,
    Shader& shader);

float depthSplitFuncLinear(float near, float far, int numShadowCascades, int i);
float customDepthSplitFunc(float near, float far, int numShadowCascades, int i);

class ShadowCascadeTest {
public:
    ShadowCascadeTest() : vao{ 0 }, vbo{ 0 } {}
    void setupBuffer(int numShadowCascades);
    void fillBuffer(int numShadowCascades, glm::vec3* cascadedShadowBounds);
    void draw();
    GLuint vao, vbo;
    std::vector<GLuint> indices;
};