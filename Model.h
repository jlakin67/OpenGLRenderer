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
    Mesh() : vao(0), vbo(0), ebo(0), numIndices(0), hasTexCoords(true), hasTexture(true), color_diffuse(0.6f, 0.6f, 0.6f, 1.0f),
        color_specular(0.0f, 0.0f, 0.0f), specularHighlight(0.0f), minBox(0.0f), maxBox(0.0f), occlusionQueryID(0), boundingBoxModel(0.0f) {}
    GLuint vao, vbo, ebo;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    GLuint numIndices;
    std::vector<Texture> textures;
    bool hasTexCoords;
    bool hasTexture;
    glm::vec4 color_diffuse;
    glm::vec3 color_specular;
    float specularHighlight;
    glm::vec3 minBox, maxBox;
    GLuint occlusionQueryID;
    glm::mat4 boundingBoxModel;
    void draw(Shader& shader, bool instanced = false, GLuint numInstances = 1);
};

struct ViewCompare {
    ViewCompare(glm::mat4 view) { this->view = view; }
    bool operator() (Mesh& m1, Mesh& m2) {
        glm::vec4 minBoxView1 = view * glm::vec4(m1.minBox, 1.0f);
        glm::vec4 maxBoxView1 = view * glm::vec4(m1.maxBox, 1.0f);
        glm::vec4 minBoxView2 = view * glm::vec4(m2.minBox, 1.0f);
        glm::vec4 maxBoxView2 = view * glm::vec4(m2.maxBox, 1.0f);
        if (-minBoxView1.z < -maxBoxView2.z && -minBoxView1.z < -minBoxView2.z) return true;
        if (-maxBoxView1.z < -maxBoxView2.z && -maxBoxView1.z < -minBoxView2.z) return true;
        if (-minBoxView2.z < -maxBoxView1.z && -minBoxView2.z < -minBoxView1.z) return false;
        if (-maxBoxView2.z < -maxBoxView1.z && -maxBoxView2.z < -minBoxView1.z) return false;
        return false;
    }
    glm::mat4 view;
};

class Model {
public:
    Model() : directory(""), instanceBuf{ 0 }, position(0.0f, 0.0f, 0.0f, 1.0f), 
        yaw(0.0f), pitch(0.0f), roll(0.0f), scale(1.0f), minBox(0.0f), maxBox(0.0f) {}
    ~Model();
    bool loadModel(std::string path, bool hasSingleMesh = false, bool flipUVs = true);
    void draw(Shader& shader);
    void setUpInstances(std::vector<glm::mat4>& models);
    void setupOcclusionQueries();
    void beginOcclusionQueries(Shader& shader, GLuint boxVAO, glm::mat4& view);
    void drawOcclusionCulling(Shader& shader, GLuint boxVAO, glm::mat4& view, Shader& occlusionShader);
    void drawInstances(Shader& shader, GLuint numInstances);
    void drawBoundingBoxes(Shader& shader, GLuint boxVAO);
    std::string directory;
    std::vector<Mesh> meshes;
    glm::vec4 position;
    float yaw, pitch, roll;
    glm::vec3 scale;
    GLuint instanceBuf;
    std::vector<GLuint> occlusionQueryIDs;
    glm::vec3 minBox, maxBox;
private:
    void loadTexture(aiTextureType type, Mesh& mesh, const aiScene* scene, unsigned int mMaterialIndex);
    void setUpMesh(aiMesh* ai_mesh, Mesh& mesh, const aiScene* scene, unsigned int mMaterialIndex);
    void setUpBuffers(Mesh& mesh);
    std::unordered_map<std::string, Texture> loadedTextures;
};