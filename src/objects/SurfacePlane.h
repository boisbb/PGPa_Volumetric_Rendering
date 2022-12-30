/*
* Volumetric Renderer implementation as a project for PGPa, 2MIT, VUT FIT
* Author: Boris Burkalo (xburka00), 2MIT
*/

#pragma once

#include <vector>
#include <string>
#include <memory>

#include "VertexArray.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "ArcballCamera.h"
#include "Renderer.h"
#include "Texture.h"

class SurfacePlane
{
public:
    SurfacePlane(std::vector<unsigned char> image, glm::vec2 dims, unsigned int textureUnit);
    ~SurfacePlane() {};

    void Draw(Shader &shader, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
    void UpdateSlice(std::vector<unsigned char> data);
    void ReInitSlice(std::vector<unsigned char> data, glm::vec2 dims);
private:
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::vector<std::vector<unsigned int>> images;

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::vec2 dims;

    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<Texture> m_Texture;
};

