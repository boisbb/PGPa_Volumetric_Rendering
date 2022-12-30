/*
* Volumetric Renderer implementation as a project for PGPa, 2MIT, VUT FIT
* Author: Boris Burkalo (xburka00), 2MIT
*/

#pragma once

#include <array>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"


struct Vertex
{
  glm::vec3 position;
  glm::vec2 texCoords;
};

class VertexBuffer{
private:
  unsigned int m_RendererID;
public:
  VertexBuffer(std::vector<Vertex>& vertices);
  VertexBuffer(std::vector<glm::vec3>& vertices);
  ~VertexBuffer();

  void Bind() const;
  void Unbind() const;
};


