#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <glad/glad.h>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Shader.h"
#include "Renderer_constants.h"

struct Vertex {
    glm::vec3 position; //layout (location = 0) in vec3 aPos;
    glm::vec3 normal; //layout (location = 1) in vec3 aNormal;
    glm::vec2 texCoords; //layout (location = 2) in vec2 aTexCoords;
    glm::vec3 tangent; //layout (location = 3) in vec3 aTangent;
    glm::vec3 bitangent; //layout (location = 4) in vec3 aBitangent;
};

//texture_{ambient,diffuse,specular, normal}{0,1,2,...}
struct Texture {
    GLuint id;
    std::string path;
    aiTextureType type;
};

class Mesh {
public:
    GLuint vao, vbo, ebo;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
    bool hasTexCoords;
    void draw(Shader& shader, bool useTexture, glm::vec4 color, bool instanced = false, GLuint numInstances = 1,
        bool shadowMap = false);
};

class Model {
public:
    Model();
    void loadModel(std::string path, bool hasSingleMesh = false);
    void draw(Shader& shader, glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), bool shadowMap = false);
    void setUpInstances(std::vector<glm::vec4>& positions);
    void drawInstances(Shader& shader, GLuint numInstances, glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 
        bool shadowMap = false);
    std::string directory;
    std::vector<Mesh> meshes;
    bool hasTexture;
    glm::mat4 model;
    GLuint instanceBuf;
private:
    void loadTexture(aiTextureType type, Mesh& mesh, const aiScene* scene, unsigned int mMaterialIndex);
    void setUpMesh(aiMesh* ai_mesh, Mesh& mesh);
    void setUpBuffers(Mesh& mesh);
    std::unordered_map<std::string, Texture> loadedTextures;
};