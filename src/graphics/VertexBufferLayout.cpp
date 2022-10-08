#include "VertexBufferLayout.h"

/**
 * @brief Push float into layout.
 * 
 * @tparam  
 * @param count 
 */
template<>
void VertexBufferLayout::Push<float>(unsigned int count)
{
  m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
  m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
}


/**
 * @brief Push uint into layout.
 * 
 * @tparam  
 * @param count 
 */
template<>
void VertexBufferLayout::Push<unsigned int>(unsigned int count)
{
  m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
  m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
}

/**
 * @brief Push int into layout.
 * 
 * @tparam  
 * @param count 
 */
template<>
void VertexBufferLayout::Push<int>(unsigned int count)
{
  m_Elements.push_back({ GL_INT, count, GL_FALSE });
  m_Stride += count * VertexBufferElement::GetSizeOfType(GL_INT);
}


/**
 * @brief Push unsigned char into layout.
 * 
 * @tparam  
 * @param count 
 */
template<>
void VertexBufferLayout::Push<unsigned char>(unsigned int count)
{
  m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
  m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
}
