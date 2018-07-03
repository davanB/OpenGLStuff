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
#include <unordered_map>
#include <string>

#include "stb_image.h"

class Model {
public:
    Model(const std::string& path) {
        loadModel(path);
    }
    
    void draw(const Shader& shader) const {
        for (const Mesh& m : mMeshes) {
            m.draw(shader);
        }
    }
private:
    void loadModel(const std::string& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
                                                 aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }
        mDirectory = path.substr(0, path.find_last_of('/'));
        
        processNode(scene->mRootNode, scene);
    }
    
    void processNode(aiNode* node, const aiScene* scene) {
        // process nodes meshes
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            // the node contains only idices to index the actual objects in the scene.
            // scene contains all data and node only contains data to keep relations between nodes
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            mMeshes.push_back(processMesh(mesh, scene));
        }
        // process children
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }
    
    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        
        for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            // positions
            glm::vec3 vector(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.position = vector;
            // normals
            vector = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            vertex.normal = vector;
            // texture coordinates
            if (mesh->mTextureCoords[0]) {
                glm::vec2 vec(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
                vertex.texCoords = vec;
            }
            else {
                vertex.texCoords = glm::vec2(0.0f, 0.0f);
            }
            // tangents
            vector = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            vertex.tangent = vector;
            // bitangent
            vector = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
            vertex.bitangent = vector;
            vertices.push_back(vertex);
        }
        // process each of the meshes faces and get vertex indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
        // process material
        // 1. Diffuse maps
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
                                                                aiTextureType_DIFFUSE,
                                                                TextureType::DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. Specular maps
        std::vector<Texture> specularMaps = loadMaterialTextures(material,
                                                                aiTextureType_SPECULAR,
                                                                TextureType::SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material,
                                                                 aiTextureType_HEIGHT,
                                                                 TextureType::NORMAL);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material,
                                                                 aiTextureType_AMBIENT,
                                                                 TextureType::HEIGHT);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
        return Mesh(vertices, indices, textures);
        
    }
    
    unsigned int textureFromFile(const char* path, const std::string& directory) {
        std::string fileName = std::string(path);
        fileName = directory + '/' + fileName;
        
        size_t f = fileName.find("\\");
        fileName.replace(f, std::string("\\").length(), "/");
        
        std::cout << fileName << std::endl;
        
        unsigned int textureID;
        glGenTextures(1, &textureID);
        
        int width;
        int height;
        int nrComponents;
        unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0);
        if (data) {
            GLenum format = NULL;
            if (nrComponents == 1) {
                format = GL_RED;
            }
            else if (nrComponents == 3) {
                format = GL_RGB;
            }
            else if (nrComponents == 4) {
                format = GL_RGBA;
            }
            
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            stbi_image_free(data);
        }
        else {
            std::cerr << "Failed to load texture" << std::endl;
            stbi_image_free(data);
        }
        
        return textureID;
    }
    
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
                                              aiTextureType type, TextureType typeName) {
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            const auto it = mLoadedTextures.find(str.C_Str());
            if (it != mLoadedTextures.end()) {
                skip = true;
                textures.push_back(it->second);
            }
            if (!skip) {
                Texture texture;
                texture.id = textureFromFile(str.C_Str(), mDirectory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                mLoadedTextures.emplace(str.C_Str(), texture);
            }
        }
        return textures;
    }

    std::vector<Mesh> mMeshes;
    std::unordered_map<std::string, Texture> mLoadedTextures;
    std::string mDirectory;
};


#endif /* model_h */
