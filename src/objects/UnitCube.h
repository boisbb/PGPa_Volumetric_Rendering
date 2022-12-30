/*
* Volumetric Renderer implementation as a project for PGPa, 2MIT, VUT FIT
* Author: Boris Burkalo (xburka00), 2MIT
*/

#pragma once

#include <string>
#include <vector>
#include <memory>

#include "VertexArray.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "ArcballCamera.h"
#include "Renderer.h"

#include "glm/gtx/string_cast.hpp"


class UnitCube
{
public:
    UnitCube();
    ~UnitCube() {}

    void Draw(Shader &shader, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 translate = glm::vec3(0.0f, 0.0f, 0.0f));
    void DrawAxisPlane(int idxZ);
    void SetModelMatrix(glm::mat4 model_matrix);

    glm::mat4 GetModelMatrix() const { return model_matrix; }
private:
    std::vector<glm::vec3> m_Vertices;
    std::vector<unsigned int> m_Indices;

    glm::mat4 model_matrix = glm::mat4(1.0f);

    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
};