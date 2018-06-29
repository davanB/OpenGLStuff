//
//  model.h
//  openGLTUT
//
//  Created by Davan Basran on 2018-06-29.
//

#ifndef model_h
#define model_h

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.h"
#include "mesh.h"

#include <vector>
#include <string>

class Model {
public:
    Model(const std::string& path) {
        loadModel(path);
    }
    
    void Draw(const Shader& shader) {
        for (const Mesh& m : mMeshes) {
            m.Draw(shader);
        }
    }
private:
    void loadModel(const std::string& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }
        directory = path.substr(0, path.find_last_on('/'));
        
        processNode(scene->mRootNode, scene);
    }
    
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
                                              aiTextureType type, std::string typeName);
    
    std::vector<Mesh> mMeshes;
    std::string mDirectory;
};


#endif /* model_h */
