/*
* Volumetric Renderer implementation as a project for PGPa, 2MIT, VUT FIT
* Author: Boris Burkalo (xburka00), 2MIT
*/

#include "UnitCube.h"

std::vector<glm::vec3> vertices = 
{
    glm::vec3(-1.0f, -1.0f,  1.0f),
    glm::vec3( 1.0f, -1.0f,  1.0f),
    glm::vec3( 1.0f,  1.0f,  1.0f),
    glm::vec3(-1.0f,  1.0f,  1.0f),
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3( 1.0f, -1.0f, -1.0f),
    glm::vec3( 1.0f,  1.0f, -1.0f),
    glm::vec3(-1.0f,  1.0f, -1.0f)
};

std::vector<unsigned int> indices =
{
    // front
    0, 1, 2,
    0, 2, 3,
    // right
    1, 5, 6,
    1, 6, 2,
    // back
    5, 4, 7,
    5, 7, 6,
    // left
    4, 0, 3,
    4, 3, 7,
    // top
    2, 6, 7,
    2, 7, 3,
    // bottom
    4, 5, 1,
    4, 1, 0,
};

UnitCube::UnitCube()
    : m_Indices(indices), m_Vertices(vertices)
{
    m_VAO = std::make_unique<VertexArray>();
    m_VAO->Bind();
    m_IndexBuffer = std::make_unique<IndexBuffer>(m_Indices);
    m_VertexBuffer = std::make_unique<VertexBuffer>(m_Vertices);

    VertexBufferLayout layout;
    layout.Push<float>(3);
    m_VAO->AddBuffer(*m_VertexBuffer, layout);
}

void UnitCube::Draw(Shader &shader, glm::vec3 scale, glm::vec3 translate)
{
    shader.Bind();
    m_VAO->Bind();
    Renderer renderer;

    glm::mat4 objectModelInit = glm::mat4(1.0f);
    objectModelInit = glm::translate(objectModelInit, translate);
    objectModelInit = glm::scale(objectModelInit, scale);

    shader.SetUniformMat4f("u_ModelMatrix", model_matrix);
    shader.SetUniformMat4f("u_Transform", objectModelInit);

    renderer.Draw(*m_VAO, *m_IndexBuffer, shader);
}

void UnitCube::SetModelMatrix(glm::mat4 model_matrix)
{
    this->model_matrix = model_matrix;
}

