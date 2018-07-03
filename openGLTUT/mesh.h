//
//  mesh.h
//  openGLTUT
//
//  Created by Davan Basran on 2018-06-29.
//

#ifndef mesh_h
#define mesh_h

#include <string>
#include <vector>

#include <glad/glad.h>

#include "shader.h"

// GLM
#include <glm/glm.hpp>

enum class TextureType {
    DIFFUSE,
    SPECULAR,
    NORMAL,
    HEIGHT
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct Texture {
    unsigned int id;
    TextureType type;
    std::string path;
};

class Mesh {
public:
    std::vector<Vertex> mVerticies;
    std::vector<unsigned int> mIndicies;
    std::vector<Texture> mTextures;
    
    Mesh(const std::vector<Vertex>& verticies, const std::vector<unsigned int>& indicies,
         const std::vector<Texture>& textures)
        : mVerticies(verticies), mIndicies(indicies), mTextures(textures) {
        setUpMesh();
    }
    
    unsigned int getVao() const {
        return mVao;
    }
    
    void draw(const Shader& shader) const {
        unsigned int diffuseNbr = 1;
        unsigned int specularNbr = 1;
        unsigned int normalNbr = 1;
        unsigned int heightNbr = 1;
        
        const std::string textureDiffuse = "texture_diffuse";
        const std::string textureSpecular = "texture_specular";
        const std::string textureNormal = "texture_normal";
        const std::string textureHeight = "texture_height";
        
        for (size_t i = 0; i < mTextures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i); // activate proper numebr texture
            
            std::string number;
            std::string name;
            TextureType type = mTextures[i].type;
            if (type == TextureType::DIFFUSE) {
                number = std::to_string(diffuseNbr);
                diffuseNbr++;
                name = textureDiffuse;
            }
            else if (type == TextureType::SPECULAR) {
                number = std::to_string(specularNbr);
                specularNbr++;
                name = textureSpecular;
            }
            else if (type == TextureType::NORMAL) {
                number = std::to_string(normalNbr);
                normalNbr++;
                name = textureNormal;
            }
            else if (type == TextureType::HEIGHT) {
                number = std::to_string(heightNbr);
                heightNbr++;
                name = textureHeight;
            }
            
            shader.setFloat(("material." + name + number).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, mTextures[i].id);
        }
        glActiveTexture(GL_TEXTURE0);
        
        // draw mesh
        glBindVertexArray(mVao);
        glDrawElements(GL_TRIANGLES, mIndicies.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
private:
    unsigned int mVao;
    unsigned int mVbo;
    unsigned int mEbo;
    
    void setUpMesh() {
        glGenVertexArrays(1, &mVao);
        glGenBuffers(1, &mVbo);
        glGenBuffers(1, &mEbo);
        
        glBindVertexArray(mVao);
        glBindBuffer(GL_ARRAY_BUFFER, mVbo);
        
        glBufferData(GL_ARRAY_BUFFER, mVerticies.size() * sizeof(Vertex), &mVerticies[0], GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndicies.size() * sizeof(unsigned int), &mIndicies[0], GL_STATIC_DRAW);
        
        // vertex positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        // vertex normals
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);
        // vertex texture coordinates
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
        glEnableVertexAttribArray(2);
        // vertex tangents
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(3);
        // vertex bitangent
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
        glEnableVertexAttribArray(4);
        
        glBindVertexArray(0);
    }
};


#endif /* mesh_h */
