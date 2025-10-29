#pragma once
#include <string>
#include <vector>
#include "mesh.h"
#include <assimp/material.h>
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

class Model {
public:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh>    meshes;
    std::string          directory;
    bool                 gammaCorrection;

    explicit Model(const std::string& path, bool gamma = false);
    void Draw(Shader& shader);

private:
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
        aiTextureType type, const std::string& typeName);
};
