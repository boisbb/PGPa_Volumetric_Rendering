#include "VertexArray.h"

VertexArray::VertexArray(){
  glGenVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray(){
}


/**
 * @brief Add a Vertex Buffer to Vertex Array Object.
 * 
 * @param vb Vertex buffer to be added.
 * @param layout Layout of the added vertex buffer.
 */
void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout){
  Bind();
  vb.Bind();
  const auto& elements = layout.GetElements();
  unsigned int offset = 0;
  for (unsigned int i = 0; i < elements.size(); i++){
    const auto& element = elements[i];
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (void*)(uintptr_t)(offset));
    offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
  }
}

/**
 * @brief Bind the VAO.
 * 
 */
void VertexArray::Bind() const {
  glBindVertexArray(m_RendererID);
}


/**
 * @brief Unbind the VAO.
 * 
 */
void VertexArray::Unbind() const {
  glBindVertexArray(0);
}
