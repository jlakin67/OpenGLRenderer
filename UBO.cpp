#include "UBO.h"

constexpr GLsizeiptr sizeofUBOType(UBOType type) {
    //struct is handled separately, array and matrix gives size of each element
    switch (type) {
    case UBOType::BOOL:
    case UBOType::INT:
    case UBOType::FLOAT:
        return 4;
    case UBOType::VEC2:
        return 8;
    case UBOType::VEC3:
    case UBOType::VEC4:
    case UBOType::ARRAY:
    case UBOType::MATRIX:
        return 16;
    default:
        return 0;
    }
}

GLsizeiptr calculateOffsets(std::vector<GLsizeiptr>& offsets, const std::vector<UBOData>& types) {
    GLsizeiptr size = 0;
    for (UBOData type : types) {
        GLsizeiptr offset = (size / sizeofUBOType(type.type)) * sizeofUBOType(type.type);
        if (offset < size) offset += sizeofUBOType(type.type);
        if (type.type == UBOType::MATRIX) size = offset + 4 * sizeofUBOType(type.type);
        else size = offset + type.size * sizeofUBOType(type.type);
        offsets.push_back(offset);
    }
    return size;
}

void UBO::initialize(std::vector<UBOData> UBOTypes, GLuint bp) {
    types = UBOTypes;
    index = bp;
    glGenBuffers(1, &id);
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    GLsizeiptr size = calculateOffsets(offsets, UBOTypes);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, bp, id);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::set(unsigned int index, GLvoid* data) {
    UBOType type = types.at(index).type;
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    bool* b; int i; GLfloat vec4_[4];
    switch (type) {
    case UBOType::BOOL:
        b = static_cast<bool*>(data);
        i = *b;
        glBufferSubData(GL_UNIFORM_BUFFER, offsets.at(index), sizeof(int), &i);
        break;
    case UBOType::INT:
    case UBOType::FLOAT:
        glBufferSubData(GL_UNIFORM_BUFFER, offsets.at(index), sizeofUBOType(UBOType::INT), data);
        break;
    case UBOType::VEC2:
        glBufferSubData(GL_UNIFORM_BUFFER, offsets.at(index), sizeofUBOType(UBOType::VEC2), data);
        break;
    case UBOType::VEC3:
        memcpy(vec4_, data, 12);
        vec4_[3] = 0.0f;
        glBufferSubData(GL_UNIFORM_BUFFER, offsets.at(index), sizeofUBOType(UBOType::VEC3), vec4_);
        break;
    case UBOType::VEC4:
        glBufferSubData(GL_UNIFORM_BUFFER, offsets.at(index), sizeofUBOType(UBOType::VEC4), data);
        break;
    case UBOType::MATRIX:
        glBufferSubData(GL_UNIFORM_BUFFER, offsets.at(index), sizeof(glm::mat4), data);
        break;
    case UBOType::ARRAY:
        glBufferSubData(GL_UNIFORM_BUFFER, offsets.at(index), types.at(index).size*sizeofUBOType(UBOType::ARRAY), data);
        break;
    default:
        std::cout << "Data type at index " << index << " not implemented.\n";
        break;
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}