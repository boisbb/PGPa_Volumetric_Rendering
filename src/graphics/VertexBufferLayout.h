/*
* Volumetric Renderer implementation as a project for PGPa, 2MIT, VUT FIT
* Author: Boris Burkalo (xburka00), 2MIT
*/

#pragma once

#include <vector>
#include <GL/glew.h>
#include <iostream>

using namespace std;

struct VertexBufferElement{
  unsigned int type;
  unsigned int count;
  unsigned char normalized;

  static unsigned int GetSizeOfType(unsigned int type){
    switch(type){
      case GL_FLOAT: return 4;
      case GL_UNSIGNED_INT: return 4;
      case GL_UNSIGNED_BYTE: return 1;
    }
    return 0;
  }
};


class VertexBufferLayout{
private:
  std::vector<VertexBufferElement> m_Elements;
  unsigned int m_Stride;
public:
  VertexBufferLayout()
    : m_Stride(0) {}

  template<typename T>
  void Push(unsigned int count){
  }


  inline const vector<VertexBufferElement> GetElements() const { return m_Elements; }
  inline unsigned int GetStride() const { return m_Stride; }
};
