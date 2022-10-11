#pragma once
#include <set>
#include <filesystem>
#include <fstream>

#include "Renderer.h"

#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

namespace fs = std::filesystem;

class VolumeTexture
{
public:
    VolumeTexture() {}
    VolumeTexture(const std::string& path, glm::vec3 dimensions, bool binFile = false);
    ~VolumeTexture() {}

    void Bind();
    void Unbind() const;
    void LoadFromBin(const std::string path);
    void LoadFromPNGs(const std::string path);
    void LoadFilenames(const std::string path);
private:
    unsigned int m_TextureID;
    unsigned char* m_LocalBuffer;
    std::set<fs::path> imageFiles;
    glm::vec3 volumeDim;
};