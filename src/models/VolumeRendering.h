/*
* Volumetric Renderer implementation as a project for PGPa, 2MIT, VUT FIT
* Author: Boris Burkalo (xburka00), 2MIT
*/

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
        void OnWindowResize(glm::vec2 size) override;
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
        glm::ivec3 sliceIndices;

        float pixelOffset = 0;
        glm::ivec2 windowOffset{0, 0};

        bool takeScreenshot = false;
        bool splitScreen = false;

        std::string currentModel;
        std::string currentTransferFunc;
        glm::vec3 currentModelSize;
        glm::vec3 currentModelSizeNorm;
    };
}