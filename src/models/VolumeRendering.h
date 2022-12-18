#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include "glm/mat4x4.hpp"

#include "TestModel.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VolumeTexture.h"
#include "ArcballCamera.h"
#include "UnitCube.h"
#include "TransferFunction.h"

namespace fs = std::filesystem;

namespace test_model {
    class VolumeRendering : public TestModel
    {
    public:
        VolumeRendering() {};
        VolumeRendering(GLFWwindow* window, int width, int height);
        ~VolumeRendering();

        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnImGuiRender() override;
    private:
        void TakeScreenshot();

        GLFWwindow* m_Window;
        float m_ClearColor[4];

        std::unique_ptr<ArcballCamera> m_ArcCamera;
        std::unique_ptr<Shader> m_SimpleShader;

        std::unique_ptr<UnitCube> m_UnitCube;
        std::unique_ptr<VolumeTexture> m_VolumeTexture;
        std::unique_ptr<TransferFunction> m_TransferFunc;
        
        // light testing
        std::unique_ptr<Shader> m_SurfacePlaneShader;

        glm::vec3 lightPos = glm::vec3(1.5f,0.0f,1.5f);
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        float lightStrength = 0.5f;

        glm::ivec2 windowSize;
        glm::ivec2 splitScreenWindowSize;
        glm::ivec2 currentWindowSize;
        float intensityThresh = 0.3;
        float stepLen = 0.001;
        float gamma = 0.4;
        float dither_ratio = 1.f;
        int sliceZIdx;
        int sliceYIdx;
        int sliceXIdx;

        float pixelOffset = 0;

        bool takeScreenshot = false;
        bool splitScreen = false;

        GLuint vbo;
    };
}