#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
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
    glm::vec4 color_diffuse;
    glm::vec3 color_specular;
    float specularHighlight;
    void draw(Shader& shader, bool useTexture, bool instanced = false, GLuint numInstances = 1);
};

class Model {
public:
    Model();
    ~Model();
    void loadModel(std::string path, bool hasSingleMesh = false);
    void draw(Shader& shader);
    void setUpInstances(std::vector<glm::mat4>& models);
    void drawInstances(Shader& shader, GLuint numInstances);
    std::string directory;
    std::vector<Mesh> meshes;
    bool hasTexture;
    glm::vec4 position;
    float yaw, pitch, roll;
    glm::vec3 scale;
    GLuint instanceBuf;
private:
    void loadTexture(aiTextureType type, Mesh& mesh, const aiScene* scene, unsigned int mMaterialIndex);
    void setUpMesh(aiMesh* ai_mesh, Mesh& mesh, const aiScene* scene, unsigned int mMaterialIndex);
    void setUpBuffers(Mesh& mesh);
    std::unordered_map<std::string, Texture> loadedTextures;
};