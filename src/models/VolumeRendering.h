#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>

#include "TestModel.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "VolumeTexture.h"
#include "Camera.h"
#include "UnitCube.h"

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
        GLFWwindow* m_Window;
        float m_ClearColor[4];

        //std::unique_ptr<Model> m_UnitCube;
        std::unique_ptr<Camera> m_Camera;
        std::unique_ptr<Shader> m_SimpleShader;

        //std::unique_ptr<Model> m_Cube;
        std::unique_ptr<UnitCube> m_UnitCube;
        std::unique_ptr<VolumeTexture> m_VolumeTexture;

        glm::vec3 lightPos = glm::vec3(1.5f,0.0f,1.5f);
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        float lightStrength = 0.5f;

        float intensityThresh = 0.4;
        float stepLen = 0.05;
        float gamma = 0.02;

        int WIDTH;
        int HEIGHT;

        GLuint vbo;
    };
}