#include "VolumeTexture.h"

VolumeTexture::VolumeTexture(const std::string& path, glm::vec3 dimensions, bool binFile)
    : volumeDim(dimensions)
{
    if (binFile)
    {
        LoadFromBin(path);
    }
    else
    {
        LoadFromPNGs(path);
    }
}

/**
 * @brief Bind the texture.
 * 
 */
void VolumeTexture::Bind()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, m_TextureID);
}


/**
 * @brief Unbind the texture.
 * 
 */
void VolumeTexture::Unbind() const
{
    glBindTexture(GL_TEXTURE_3D, 0);
}

void VolumeTexture::LoadFromBin(const std::string path)
{
    stbi_set_flip_vertically_on_load(1);
    std::ifstream input(path, std::ios::binary);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
    
    glGenTextures(1, &m_TextureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, m_TextureID);

    // set the texture parameters
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // The array on the host has 1 byte alignment
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, volumeDim.x, volumeDim.y, volumeDim.z, 0, GL_RED, GL_UNSIGNED_BYTE, &buffer[0]);
    glGenerateMipmap(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, 0);

    /*
    std::vector<unsigned char> firstBuff;
    for (int i = 0; i < 256*256; i++)
    {
        firstBuff.push_back(buffer.at(i));
    }
    stbi_write_jpg("test.jpg", 256, 256, 1, &firstBuff[0], 100);
    */

}

void VolumeTexture::LoadFromPNGs(const std::string path)
{
    stbi_set_flip_vertically_on_load(1);
    LoadFilenames(path); 

    glGenTextures(1, &m_TextureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, m_TextureID);

    // set the texture parameters
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // The array on the host has 1 byte alignment

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, volumeDim.x, volumeDim.y, volumeDim.z, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    int depth = 0;

    // TODO: for now loading from files, extend to raw files
    for (auto& filename : imageFiles)
    {
        // std::cout << filename.c_str() << std::endl;
        int w, h, bpp;
        m_LocalBuffer = stbi_load(filename.c_str(), &w, &h, &bpp, 1);

        glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, depth, w, h, 1, GL_RED, GL_UNSIGNED_BYTE, &m_LocalBuffer[0]);

        // std::string out = filename.string() + ".jpg";
        // stbi_write_jpg(out.c_str(), w, h, 1, m_LocalBuffer, 100);

        if (m_LocalBuffer)
        {
            stbi_image_free(m_LocalBuffer);
        }

        depth++;
    }

    glGenerateMipmap(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, 0);
}

void VolumeTexture::LoadFilenames(const std::string path)
{
    std::cout << path << std::endl;
    for (const auto& entry : fs::directory_iterator(path))
    {
        imageFiles.insert(entry.path());
    }
}
