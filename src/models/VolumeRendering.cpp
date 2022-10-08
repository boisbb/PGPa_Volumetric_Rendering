#include "VolumeRendering.h"

#include "imgui.h"

namespace test_model
{
    VolumeRendering::VolumeRendering(GLFWwindow* window, int width, int height)
        : m_ClearColor {0.2f, 0.3f, 0.8f, 1.0f}, WIDTH(width), HEIGHT(height), m_Window(window)             
    {
        glEnable(GL_DEPTH_TEST);

        glm::vec3 camPos = glm::vec3(3, 0,0);
        m_Camera = std::make_unique<Camera>(WIDTH, HEIGHT, camPos,
                                            glm::vec3(-1, 0, 0),
                                            glm::vec3(0, 1, 0));
        m_SimpleShader = std::make_unique<Shader>("../res/shaders/volumeRendering.shader");

        m_UnitCube = std::make_unique<UnitCube>();
        //m_Cube = std::make_unique<Model>("../res/models/box/box.obj");
    }

    VolumeRendering::~VolumeRendering()
    {

    }

    void VolumeRendering::OnUpdate(float deltaTime)
    {

    }

    void VolumeRendering::OnRender()
    {
        glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_Camera->Input(m_Window);
        m_Camera->updateMatrix(45.0f, 0.1f, 1000.0f);

        glm::mat4 view = m_Camera->GetView();
        glm::vec3 viewPos = m_Camera->GetPosition();
        glm::mat4 projection = m_Camera->GetProjection();
        glm::mat4 camMetrix = m_Camera->GetMatrix();


        m_SimpleShader->Bind();
        m_Camera->Matrix(*m_SimpleShader, "u_CameraMatrix");
        m_UnitCube->Draw(*m_SimpleShader, *m_Camera);
        // m_Cube->Draw(*m_SimpleShader, *m_Camera);
    }


    void VolumeRendering::OnImGuiRender() 
    {
        ImGui::Begin("Test");
        ImGui::ColorEdit4("Clear Color", m_ClearColor);
        ImGui::End();

        if (ImGui::CollapsingHeader("Light")){
            ImGui::Indent();
		    ImGui::SliderFloat("Intensity", &lightStrength, 0, 20);
            ImGui::SliderFloat3("Light Position", &lightPos.x, 0, 800);
            ImGui::Unindent();
        }
    }
}