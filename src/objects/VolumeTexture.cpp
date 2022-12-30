/*
* Volumetric Renderer implementation as a project for PGPa, 2MIT, VUT FIT
* Author: Boris Burkalo (xburka00), 2MIT
*/

#include "VolumeTexture.h"

VolumeTexture::VolumeTexture(const std::string& path, glm::vec3 dimensions, bool binFile)
    : volumeDim(dimensions), 
    zImages((int)dimensions.z, std::vector((int)(dimensions.x * dimensions.y), (unsigned char)0)),
    yImages((int)dimensions.y, std::vector((int)(dimensions.z * dimensions.x), (unsigned char)0)),
    xImages((int)dimensions.x, std::vector((int)(dimensions.z * dimensions.y), (unsigned char)0))
{
    if (binFile)
    {
        LoadFromBin(path);
    }
    else
    {
        LoadFromPNGs(path);
    }

    surfacePlaneZ = std::make_unique<SurfacePlane>(zImages[(int)(volumeDim.z / 2)], glm::vec2(volumeDim.x, volumeDim.y), 0);
    surfacePlaneY = std::make_unique<SurfacePlane>(yImages[(int)(volumeDim.y / 2)], glm::vec2(volumeDim.x, volumeDim.z), 0);
    surfacePlaneX = std::make_unique<SurfacePlane>(xImages[(int)(volumeDim.x / 2)], glm::vec2(volumeDim.y, volumeDim.z), 0);
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

void VolumeTexture::UpdateSPSlice(SliceAxis sliceAxis, int sliceId)
{
    switch(sliceAxis)
    {
        case Z:
            surfacePlaneZ->UpdateSlice(zImages[sliceId]);
            break;
        case Y:
            surfacePlaneY->UpdateSlice(yImages[sliceId]);
            break;
        case X:
            surfacePlaneX->UpdateSlice(xImages[sliceId]);
            break;
    }
}

void VolumeTexture::UpdateFromBin(const std::string &path, const glm::vec3 &dimensions)
{
    volumeDim = dimensions;
    zImages = std::vector((int)dimensions.z, std::vector((int)(dimensions.x * dimensions.y), (unsigned char)0));
    yImages = std::vector((int)dimensions.y, std::vector((int)(dimensions.z * dimensions.x), (unsigned char)0));
    xImages = std::vector((int)dimensions.x, std::vector((int)(dimensions.z * dimensions.y), (unsigned char)0));
    
    glDeleteTextures(1, &m_TextureID);

    LoadFromBin(path);

    surfacePlaneX->ReInitSlice(xImages[(int)(volumeDim.x / 2)], glm::vec2(volumeDim.y, volumeDim.z));
    surfacePlaneY->ReInitSlice(yImages[(int)(volumeDim.y / 2)], glm::vec2(volumeDim.x, volumeDim.z));
    surfacePlaneZ->ReInitSlice(zImages[(int)(volumeDim.z / 2)], glm::vec2(volumeDim.x, volumeDim.y));
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
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // The array on the host has 1 byte alignment
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, volumeDim.x, volumeDim.y, volumeDim.z, 0, GL_RED, GL_UNSIGNED_BYTE, &buffer[0]);
    // glGenerateMipmap(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, 0);

    ConstructImageVector(buffer);

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

    for (auto& filename : imageFiles)
    {
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
    for (const auto& entry : fs::directory_iterator(path))
    {
        imageFiles.insert(entry.path());
    }
}

void VolumeTexture::ConstructImageVector(const std::vector<unsigned char> volumeBuffer)
{

    // Construct going along Z axis
    #pragma omp parallel for
    for (int z = 0; z < (int)volumeDim.z; z++)
    {
        for (int y = 0; y < (int)volumeDim.y; y++)
        {
            for (int x = 0; x < (int)volumeDim.x; x++)
            {
                int xyIdx = y * volumeDim.x + x;
                int xzIdx = z * volumeDim.x + x;
                int yzIdx = z * volumeDim.y + y;
                int zIdx = z * volumeDim.y * volumeDim.x + xyIdx;
                zImages[z][xyIdx] = volumeBuffer[zIdx];
                yImages[y][xzIdx] = volumeBuffer[zIdx];
                xImages[x][yzIdx] = volumeBuffer[zIdx];
            }
        }
    }

    int dimsMax = std::max(std::max(volumeDim.z, volumeDim.y), volumeDim.x);

    for (int i = 0; i < dimsMax; i++)
    {
        if (i < (int)volumeDim.z)
        {
            std::reverse(zImages[i].begin(), zImages[i].end());
        }

        if (i < (int)volumeDim.y)
        {
            std::reverse(yImages[i].begin(), yImages[i].end());
        }

        if (i < (int)volumeDim.x)
        {
            std::reverse(xImages[i].begin(), xImages[i].end());
        }
    }
}
