#pragma once
#include <string>
#include <vector>

#include "VertexArray.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Camera.h"
#include "Texture.h"
#include "Renderer.h"

#include "glm/gtx/string_cast.hpp"


class UnitCube
{
public:
    UnitCube();
    ~UnitCube() {}

    void Draw(Shader &shader, Camera &camera, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 translate = glm::vec3(0.0f, 0.0f, 0.0f));
    void SetModelMatrix(glm::mat4 model_matrix);
private:
    std::vector<glm::vec3> m_Vertices;
    std::vector<unsigned int> m_Indices;

    glm::mat4 model_matrix = glm::mat4(1.0f);

    VertexArray *m_VAO;
    IndexBuffer *m_IndexBuffer;
    VertexBuffer *m_VertexBuffer;
};