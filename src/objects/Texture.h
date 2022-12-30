/*
* Volumetric Renderer implementation as a project for PGPa, 2MIT, VUT FIT
* Author: Boris Burkalo (xburka00), 2MIT
*/

#pragma once

#include "Renderer.h"

#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

class Texture
{
public:
    Texture() {};
    Texture(const std::string& path, unsigned int slot, int rows = 1, bool flip_on_load = true);
    Texture(std::vector<unsigned char> data, glm::ivec2 dims, unsigned int unit = 1);
    
    ~Texture();
    
    void Init(std::vector<unsigned char> data);
    void Update(std::vector<unsigned char> data);
    void ReInit(std::vector<unsigned char> data, glm::vec2 dims);
    void SetUnit(Shader& shader, std::string uniform, unsigned int unit);
    void Bind();
    void Unbind() const;

    inline int GetWidth() const{ return m_Width; }
    inline int GetHeight() const{ return m_Height; }
    inline const char* GetType() const{ return m_Type; }
    inline std::string GetPath() const {return m_FilePath; }
    inline const unsigned int GetID() const { return m_RendererID; }
    inline const int GetNumRows() const { return numOfRows; }

private:
    unsigned int m_RendererID;
    std::string m_FilePath;
    unsigned char* m_LocalBuffer;
    int m_Width, m_Height, m_BPP;
    unsigned int unit = 0;
    const char* m_Type;
    int numOfRows = 1;
};