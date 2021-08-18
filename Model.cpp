#include "Model.h"

Model::~Model()
{
    for (Mesh& mesh : meshes) {
        glDeleteBuffers(1, &mesh.ebo);
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteVertexArrays(1, &mesh.vao);
        for (Texture& texture : mesh.textures) {
            glDeleteTextures(1, &texture.id);
        }
    }
    glDeleteBuffers(1, &instanceBuf);
}

bool Model::loadModel(std::string path, bool hasSingleMesh, bool flipUVs) {
    //directory = path.substr(0, path.find_last_of('/'));
    std::string newPath(path);
    std::replace(newPath.begin(), newPath.end(), '\\', '/');
    directory = newPath.substr(0, newPath.find_last_of('/'));
    Assimp::Importer importer;
    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
    unsigned int importFlags = aiProcess_CalcTangentSpace | aiProcess_Triangulate
        | aiProcess_GenSmoothNormals | aiProcess_PreTransformVertices | aiProcess_SortByPType;
    if (flipUVs) importFlags = importFlags | aiProcess_FlipUVs;
    const aiScene* scene = importer.ReadFile(newPath, importFlags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return false;
    }
    Mesh singleMesh;
    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* ai_mesh = scene->mMeshes[i];
        if (!hasSingleMesh) {
            Mesh mesh;
            setUpMesh(ai_mesh, mesh, scene, ai_mesh->mMaterialIndex);
            //separate buffers for each mesh
            setUpBuffers(mesh);
            if (!mesh.hasTexCoords) {
                meshes.push_back(mesh);
            }
            else {
                //load textures
                loadTexture(aiTextureType_DIFFUSE, mesh, scene, ai_mesh->mMaterialIndex);
                loadTexture(aiTextureType_SPECULAR, mesh, scene, ai_mesh->mMaterialIndex);
                loadTexture(aiTextureType_AMBIENT, mesh, scene, ai_mesh->mMaterialIndex);
                loadTexture(aiTextureType_NORMALS, mesh, scene, ai_mesh->mMaterialIndex);
                loadTexture(aiTextureType_OPACITY, mesh, scene, ai_mesh->mMaterialIndex);
                if (mesh.textures.empty()) {
                    mesh.hasTexCoords = false;
                    mesh.hasTexture = false;
                }
                else mesh.hasTexture = true;
                meshes.push_back(mesh);
            }
        }
        else {
            setUpMesh(ai_mesh, singleMesh, scene, ai_mesh->mMaterialIndex);
            //load textures for single mesh
            loadTexture(aiTextureType_DIFFUSE, singleMesh, scene, ai_mesh->mMaterialIndex);
            loadTexture(aiTextureType_SPECULAR, singleMesh, scene, ai_mesh->mMaterialIndex);
            loadTexture(aiTextureType_AMBIENT, singleMesh, scene, ai_mesh->mMaterialIndex);
            loadTexture(aiTextureType_NORMALS, singleMesh, scene, ai_mesh->mMaterialIndex);
            loadTexture(aiTextureType_OPACITY, singleMesh, scene, ai_mesh->mMaterialIndex);
            if (singleMesh.textures.empty()) {
                singleMesh.hasTexCoords = false;
                singleMesh.hasTexture = false;
            }
        
        }
    }
    if (hasSingleMesh) {
        setUpBuffers(singleMesh);
        meshes.push_back(singleMesh);
    }
    return true;
}

void Model::loadTexture(aiTextureType type, Mesh& mesh, const aiScene* scene, unsigned int mMaterialIndex) {
    aiMaterial* material = scene->mMaterials[mMaterialIndex];
    for (int i = 0; i < material->GetTextureCount(type); i++) {
        aiString ai_path;
        material->GetTexture(type, i, &ai_path);
        std::string path(ai_path.C_Str());
        std::replace(path.begin(), path.end(), '\\', '/');
        if (!(isalpha(path.at(0)) && path.at(1) == ':' && path.at(2) == '/')) {//not absolute path
            path = directory + "/" + path;
        }
        auto it = loadedTextures.find(path);
        if (it == loadedTextures.end()) {
            Texture texture;
            texture.path = path;
            texture.type = type;
            //load texture from file and create tex object and set data and tex parameters
            int textureWidth, textureHeight, numChannels;
            unsigned char* image = stbi_load(path.c_str(), &textureWidth, &textureHeight, &numChannels, 4);
            if (!image) {
                std::cout << "Unable to load image from file:   " << path << std::endl;
                texture.id = 0;
                texture.type = aiTextureType_NONE;
            }
            else {
                GLuint id;
                glGenTextures(1, &id);
                texture.id = id;
                glBindTexture(GL_TEXTURE_2D, id);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
                stbi_image_free(image);
                glGenerateMipmap(GL_TEXTURE_2D);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            std::pair<std::string, Texture> loadedTexture(path, texture);
            loadedTextures.insert(loadedTexture);
            mesh.textures.push_back(texture);
        }
        else {
            mesh.textures.push_back(it->second);
        }
    }
}

void Model::setUpMesh(aiMesh* ai_mesh, Mesh& mesh, const aiScene* scene, unsigned int mMaterialIndex)
{
    mesh.hasTexCoords = true;
    aiVector3D* texCoords = ai_mesh->mTextureCoords[0];
    if (!texCoords) {
        mesh.hasTexCoords = false;
        mesh.hasTexture = false;
    }
    aiMaterial* material = scene->mMaterials[mMaterialIndex];
    float alpha = 1.0f;
    aiColor3D inColor(0.6f, 0.6f, 0.6f);
    if (material) material->Get(AI_MATKEY_COLOR_DIFFUSE, inColor);
    if (material) material->Get(AI_MATKEY_OPACITY, alpha);
    mesh.color_diffuse = glm::vec4(inColor.r, inColor.g, inColor.b, alpha);
    aiColor3D inSpecular(1.0f, 1.0f, 1.0f);
    if (material) material->Get(AI_MATKEY_COLOR_SPECULAR, inSpecular);
    mesh.color_specular = glm::vec3(inSpecular.r, inSpecular.g, inSpecular.b);
    float inSpecularHighlight = 0.0f;
    if (material) material->Get(AI_MATKEY_SHININESS, inSpecularHighlight);
    mesh.specularHighlight = inSpecularHighlight;
    //fill vertex array
    for (int j = 0; j < ai_mesh->mNumVertices; j++) {
        Vertex vertex;
        aiVector3D position = ai_mesh->mVertices[j];
        vertex.position = glm::vec3(position.x, position.y, position.z);
        aiVector3D normal = ai_mesh->mNormals[j];
        vertex.normal = glm::vec3(normal.x, normal.y, normal.z);
        if (!texCoords) {
            vertex.texCoords = glm::vec2(-1.0f);
        }
        else {
            vertex.texCoords = glm::vec2(texCoords[j].x, texCoords[j].y);
        }
        if (ai_mesh->mTangents && ai_mesh->mBitangents) {
            aiVector3D tangent = ai_mesh->mTangents[j];
            vertex.tangent = glm::vec3(tangent.x, tangent.y, tangent.z);
            aiVector3D bitangent = ai_mesh->mBitangents[j];
            vertex.bitangent = glm::vec3(bitangent.x, bitangent.y, bitangent.z);
        }
        else {
            vertex.tangent = glm::vec3(0, 0, 0);
            vertex.bitangent = glm::vec3(0, 0, 0);
        }
        mesh.vertices.push_back(vertex);
    }
    //fill indices array
    for (int j = 0; j < ai_mesh->mNumFaces; j++) {
        aiFace face = ai_mesh->mFaces[j];
        for (int k = 0; k < face.mNumIndices; k++) {
            mesh.indices.push_back(face.mIndices[k]);
        }
    }
}

void Model::setUpBuffers(Mesh& mesh)
{
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(GLuint), &mesh.indices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    glBindVertexArray(0);
    //glFinish();
    mesh.numIndices = mesh.indices.size();
    mesh.vertices.resize(0);
    mesh.indices.resize(0);
}

void Mesh::draw(Shader& shader, bool instanced, GLuint numInstances) {
    //must compile shader first
    //set model, view, projection uniforms before calling this function
    glBindVertexArray(vao);
    shader.useProgram();
    shader.setBool("containsDiffuse", GL_FALSE);
    shader.setBool("containsSpecular", GL_FALSE);
    shader.setBool("containsAmbient", GL_FALSE);
    shader.setBool("containsNormal", GL_FALSE);
    shader.setBool("containsAlpha", GL_FALSE);
    shader.setFloat("specularHighlight", specularHighlight);
    if (hasTexture && hasTexCoords) {
        int diffuseNum = 0, specularNum = 0, ambientNum = 0, numTextures = 0, normalNum = 0;
        for (Texture& texture : textures) {
            std::string texName;
            switch (texture.type) {
            case aiTextureType_DIFFUSE:
                texName = "texture_diffuse" + std::to_string(diffuseNum);
                glActiveTexture(GL_TEXTURE0 + numTextures);
                glBindTexture(GL_TEXTURE_2D, texture.id);
                shader.setInt(texName, numTextures);
                shader.setBool("containsDiffuse", GL_TRUE);
                diffuseNum++;
                numTextures++;
                break;
            case aiTextureType_SPECULAR:
                texName = "texture_specular" + std::to_string(specularNum);
                glActiveTexture(GL_TEXTURE0 + numTextures);
                glBindTexture(GL_TEXTURE_2D, texture.id);
                shader.setInt(texName, numTextures);
                shader.setBool("containsSpecular", GL_TRUE);
                specularNum++;
                numTextures++;
                break;
            case aiTextureType_AMBIENT:
                texName = "texture_ambient" + std::to_string(ambientNum);
                glActiveTexture(GL_TEXTURE0 + numTextures);
                glBindTexture(GL_TEXTURE_2D, texture.id);
                shader.setInt(texName, numTextures);
                shader.setBool("containsAmbient", GL_TRUE);
                ambientNum++;
                numTextures++;
                break;
            case aiTextureType_NORMALS:
                texName = "texture_normal" + std::to_string(normalNum);
                glActiveTexture(GL_TEXTURE0 + numTextures);
                glBindTexture(GL_TEXTURE_2D, texture.id);
                shader.setInt(texName, numTextures);
                shader.setBool("containsNormal", GL_TRUE);
                normalNum++;
                numTextures++;
                break;
            case aiTextureType_OPACITY:
                texName = "texture_alpha";
                glActiveTexture(GL_TEXTURE0 + numTextures);
                glBindTexture(GL_TEXTURE_2D, texture.id);
                shader.setInt(texName, numTextures);
                shader.setBool("containsAlpha", GL_TRUE);
                numTextures++;
                break;
            }
        }
    }
    else {
        shader.setVec4("color", glm::value_ptr(color_diffuse));
        shader.setVec3("specularColor", glm::value_ptr(color_specular));
    }
    if (instanced) glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0, numInstances);
    else glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Model::draw(Shader& shader) {
    //must compile shader first
    //set model, view, projection uniforms before calling this function
    shader.useProgram();
    glm::mat4 model = glm::scale(glm::mat4(1.0f), scale);
    glm::vec3 ypr(yaw, pitch, roll);
    glm::mat4 rotation = glm::orientate4(ypr);
    model = rotation * model;
    model = glm::translate(model, glm::vec3(position));
    shader.setMat4("model", glm::value_ptr(model));
    shader.setBool("instanced", false);
    for (Mesh& mesh : meshes) {
        mesh.draw(shader, false, 1);
    }
}

void Model::setUpInstances(std::vector<glm::mat4>& models)
{
    if (instanceBuf != 0) glDeleteBuffers(1, &instanceBuf);
    glGenBuffers(1, &instanceBuf);
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuf);
    glBufferData(GL_ARRAY_BUFFER, models.size() * sizeof(glm::vec4),
        glm::value_ptr(models[0]), GL_STATIC_DRAW);
    glBindVertexArray(meshes.at(0).vao);
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glVertexAttribDivisor(9, 1);
    glEnableVertexAttribArray(10);
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glVertexAttribDivisor(10, 1);
    glEnableVertexAttribArray(11);
    glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2*sizeof(glm::vec4)));
    glVertexAttribDivisor(11, 1);
    glEnableVertexAttribArray(12);
    glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
    glVertexAttribDivisor(12, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Model::drawInstances(Shader& shader, GLuint numInstances)
{
    //must compile shader first
    //set model, view, projection uniforms before calling this function
    //must call setUpInstances first
    //must only have one mesh
    if (meshes.size() != 1) {
        std::cout << "\nError::Model: Must only have one mesh to draw instanced.\n";
        return;
    }
    shader.useProgram();
    glm::mat4 model = glm::scale(glm::mat4(1.0f), scale);
    glm::vec3 ypr(yaw, pitch, roll);
    glm::mat4 rotation = glm::orientate4(ypr);
    model = rotation * model;
    model = glm::translate(model, glm::vec3(position));
    shader.setMat4("model", glm::value_ptr(model));
    shader.setBool("instanced", true);
    meshes.at(0).draw(shader, true, numInstances);
}
