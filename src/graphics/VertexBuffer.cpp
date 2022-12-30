/*
* Volumetric Renderer implementation as a project for PGPa, 2MIT, VUT FIT
* Author: Boris Burkalo (xburka00), 2MIT
*/

#include "VertexBuffer.h"

#include <iostream>

/**
 * @brief Construct a new Vertex Buffer:: Vertex Buffer object
 * 
 * @param vertices Vector of Vertex objects.
 */
VertexBuffer::VertexBuffer(std::vector<Vertex>& vertices) 
{
  glGenBuffers(1, &m_RendererID);
  glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}


/**
 * @brief Construct a new Vertex Buffer:: Vertex Buffer object
 * 
 * @param vertices Vector of vec3s.
 */
VertexBuffer::VertexBuffer(std::vector<glm::vec3>& vertices) 
{
  glGenBuffers(1, &m_RendererID);
  glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer(){
}

/**
 * @brief Bind the VB.
 * 
 */
void VertexBuffer::Bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}


/**
 * @brief Unbind the VB.
 * 
 */
void VertexBuffer::Unbind() const {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
