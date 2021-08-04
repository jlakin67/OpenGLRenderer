#pragma once
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum class UBOType { FLOAT, INT, BOOL, VEC2, VEC3, VEC4, ARRAY, MATRIX, STRUCT };
struct UBOData {
    UBOData(UBOType type, GLsizei size = 1) : type{type}, size{size} {}
    UBOType type;
    GLsizei size;
};

GLsizeiptr calculateOffsets(std::vector<GLsizeiptr>& offsets, const std::vector<UBOData>& types);

constexpr GLsizeiptr sizeofUBOType(UBOType type);

class UBO {
public:
    UBO() { id = 0; index = 0; }
    void initialize(std::vector<UBOData> UBOTypes, GLuint bp);
    void set(unsigned int index, GLvoid* data);
    void use() { glBindBuffer(GL_UNIFORM_BUFFER, id); }
    std::vector<UBOData> types;
    std::vector<GLsizeiptr> offsets;
    GLuint id, index;
};