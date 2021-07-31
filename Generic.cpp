#include "Generic.h"

Generic::Generic(const GLfloat* vertices, GLsizei count, const GLuint* indices, GLsizei indexCount)
    : vertices{ vertices }, count{ count }, indices{ indices }, indexCount{ indexCount } {
    model = glm::mat4(1.0);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 3ull * count * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    if (indices) {
        GLuint ebo;
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLuint), indices, GL_STATIC_DRAW);
    }
    glBindVertexArray(0);
}

void Generic::draw() {
    //shader separate, call shader.use() before calling this function
    glBindVertexArray(vao);
    if (indices) {
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, count);
    }
    glBindVertexArray(0);
}