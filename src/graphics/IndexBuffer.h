#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

class IndexBuffer{
private:
  unsigned int m_RendererID;
  unsigned int m_Count;
public:
  IndexBuffer(const unsigned int* data, unsigned int count);
  IndexBuffer(std::vector<unsigned int>& indices);
  ~IndexBuffer();

  void Bind() const;
  void Unbind() const;

  inline unsigned int GetCount() const { return m_Count; }
};
