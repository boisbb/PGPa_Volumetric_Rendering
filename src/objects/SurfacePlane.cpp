#include "SurfacePlane.h"

std::vector<Vertex> surfacePlaneVertices {
    Vertex{glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec2(0, 0)},
    Vertex{glm::vec3( 1.0f, -1.0f,  1.0f), glm::vec2(1, 0)},
    Vertex{glm::vec3( 1.0f,  1.0f,  1.0f), glm::vec2(1, 1)},
    Vertex{glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec2(0, 1)}
};

std::vector<unsigned int> surfacePlaneIndices {
    0, 1, 2,
    0, 2, 3
};

SurfacePlane::SurfacePlane(std::vector<unsigned char> image, glm::vec2 dims, unsigned int textureUnit)
    : m_Indices(surfacePlaneIndices), m_Vertices(surfacePlaneVertices), dims(dims)
{
    m_VAO = std::make_unique<VertexArray>();
    m_VAO->Bind();
    m_IndexBuffer = std::make_unique<IndexBuffer>(m_Indices);
    m_VertexBuffer = std::make_unique<VertexBuffer>(m_Vertices);
    //m_Texture = std::make_unique<Texture>("../res/textures/a.png", 0);
    m_Texture = std::make_unique<Texture>(image, dims, textureUnit);

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);
    m_VAO->AddBuffer(*m_VertexBuffer, layout);

    dims.x = dims.x / std::max(dims.x, dims.y);
    dims.y = dims.y / std::max(dims.x, dims.y);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(dims.x, dims.y, 1));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.75, 0.75, 1));
}

void SurfacePlane::Draw(Shader &shader, glm::vec3 scale)
{
    shader.Bind();
    m_VAO->Bind();
    Renderer renderer;

    glm::mat4 objectModelInit = glm::mat4(1.0f);
    objectModelInit = glm::scale(objectModelInit, scale);

    shader.SetUniformMat4f("u_ModelMatrix", modelMatrix);
    m_Texture->Bind();
    //shader.SetUniformMat4f("u_Transform", objectModelInit);

    renderer.Draw(*m_VAO, *m_IndexBuffer, shader);
}

void SurfacePlane::UpdateSlice(std::vector<unsigned char> data)
{
    m_Texture->Update(data);
}
