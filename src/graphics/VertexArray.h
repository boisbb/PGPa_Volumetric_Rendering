/*
* Volumetric Renderer implementation as a project for PGPa, 2MIT, VUT FIT
* Author: Boris Burkalo (xburka00), 2MIT
*/

#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray{
private:
  unsigned int m_RendererID;
public:
  VertexArray();
  ~VertexArray();

  void Bind() const;
  void Unbind() const;

  void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

  unsigned int GetId() const { return m_RendererID; }
};

