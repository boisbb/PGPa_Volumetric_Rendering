#include "Texture.h"


/**
 * @brief Construct a new Texture:: Texture object
 * 
 * @param path File path.
 * @param slot Slot of the texture.
 */
Texture::Texture(const std::string& path, unsigned int slot, int rows, bool flip_on_load)
    : m_FilePath(path), unit(slot)
{
    numOfRows = rows;

    if (flip_on_load)
    {
        stbi_set_flip_vertically_on_load(1);
    }
    m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

    glGenTextures(1, &m_RendererID);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);

	glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    if (m_LocalBuffer)
    {
        stbi_image_free(m_LocalBuffer);
    }

    Unbind();
}

Texture::Texture(std::vector<unsigned char> data, glm::ivec2 dims, unsigned int unit)
    : m_FilePath(""), unit(unit), m_Width(dims.x), m_Height(dims.y), imageData(data)
{
    glGenTextures(1, &m_RendererID);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_Width, m_Height, 0, GL_RED, GL_UNSIGNED_BYTE, &imageData[0]);

	glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    std::vector<unsigned char> firstBuff;
    for (int i = 0; i < m_Width*m_Height; i++)
    {
        firstBuff.push_back(imageData.at(i));
    }
    stbi_write_jpg("test.jpg", m_Width, m_Height, 1, &firstBuff[0], 100);

    Unbind();
}

Texture::~Texture() 
{
    
}


void Texture::Update(std::vector<unsigned char> data)
{
    std::cout << "updated slice" << std::endl;
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, GL_RED, GL_UNSIGNED_BYTE, data.data());

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    Unbind();
}


/**
 * @brief Set the unit of a texture for shader.
 * 
 * @param shader 
 * @param uniform 
 * @param unit 
 */
void Texture::SetUnit(Shader& shader, std::string uniform, unsigned int unit) 
{
    shader.Bind();
    shader.SetUniform1i(uniform, unit);
}


/**
 * @brief Bind the texture.
 * 
 */
void Texture::Bind()
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
}


/**
 * @brief Unbind the texture.
 * 
 */
void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
