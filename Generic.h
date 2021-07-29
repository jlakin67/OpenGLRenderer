#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Generic {
public:
    Generic(const GLfloat* vertices, GLsizei count, const GLuint* indices = NULL, GLsizei indexCount = 0);
    void draw();
    const GLfloat* vertices;
    GLsizei count;
    const GLuint* indices;
    GLsizei indexCount;
    GLuint vao;
    glm::mat4 model;
};