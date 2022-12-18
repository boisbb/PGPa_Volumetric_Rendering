#pragma once
#include <set>
#include <filesystem>
#include <fstream>
#include <omp.h>
#include <algorithm>

#include "Renderer.h"
#include "SurfacePlane.h"

#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

enum SliceAxis { Z, Y, X };

namespace fs = std::filesystem;

class VolumeTexture
{
public:
    VolumeTexture() {}
    VolumeTexture(const std::string& path, glm::vec3 dimensions, bool binFile = false);
    ~VolumeTexture() {}

    void Bind();
    void Unbind() const;
    void UpdateSPSlice(SliceAxis sliceAxis, int sliceId);

    std::unique_ptr<SurfacePlane> surfacePlaneZ;
    std::unique_ptr<SurfacePlane> surfacePlaneY;
    std::unique_ptr<SurfacePlane> surfacePlaneX;
private:
    void LoadFromBin(const std::string path);
    void LoadFromPNGs(const std::string path);
    void LoadFilenames(const std::string path);
    void ConstructImageVector(const std::vector<unsigned char> volumeBuffer);
    unsigned int m_TextureID;
    unsigned char* m_LocalBuffer;
    std::set<fs::path> imageFiles;
    std::vector<std::vector<unsigned char>> xImages;
    std::vector<std::vector<unsigned char>> yImages;
    std::vector<std::vector<unsigned char>> zImages;
    glm::vec3 volumeDim;
};