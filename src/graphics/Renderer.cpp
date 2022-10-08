#include "Renderer.h"
#include <iostream>

/**
 * @brief Clear the scene.
 * 
 */
void Renderer::Clear() const{
    glClear(GL_COLOR_BUFFER_BIT);
}

/**
 * @brief Draw the scene.
 * 
 * @param va 
 * @param ib 
 * @param shader 
 */
void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const{
    va.Bind();
    glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, 0);
    va.Unbind();
}