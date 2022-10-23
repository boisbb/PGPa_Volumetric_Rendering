#include "VolumeRendering.h"

#include "imgui.h"

#define VOLUME_W 256
#define VOLUME_H 256
#define VOLUME_D 109

#define NORM_W ((float)VOLUME_W * 1.f) / 256.f
#define NORM_H ((float)VOLUME_H * 1.f) / 256.f
#define NORM_D ((float)VOLUME_D * 1.f) / 256.f

namespace test_model
{
    VolumeRendering::VolumeRendering(GLFWwindow* window, int width, int height)
        : m_ClearColor {0.1f, 0.1f, 0.1f, 1.0f}, WIDTH(width), HEIGHT(height), m_Window(window)             
    {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glm::vec3 camPos = glm::vec3(0, 0, 4);
        m_Camera = std::make_unique<Camera>(WIDTH, HEIGHT, camPos,
                                            glm::vec3(0, 0, -1),
                                            glm::vec3(0, 1, 0));
        m_SimpleShader = std::make_unique<Shader>("../res/shaders/isometricTransferF.shader");
        m_LightShader = std::make_unique<Shader>("../res/shaders/lightCube.shader");
        m_UnitCube = std::make_unique<UnitCube>();
        m_LightCube = std::make_unique<UnitCube>();
        m_VolumeTexture = std::make_unique<VolumeTexture>("../res/textures/head256x256x109", glm::vec3(VOLUME_W, VOLUME_H, 108), true);
        m_TransferFunc = std::make_unique<TransferFunction>();

        m_UnitCube->SetModelMatrix(glm::scale(m_UnitCube->GetModelMatrix(), glm::vec3(NORM_W, NORM_H, NORM_D)));

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
        float FOV = 45.0f;

        m_Camera->Input(m_Window);
        m_Camera->updateMatrix(FOV, 0.1f, 1000.0f);

        glm::mat4 view = m_Camera->GetView();
        glm::vec3 viewPos = m_Camera->GetPosition();
        glm::mat4 projection = m_Camera->GetProjection();
        glm::mat4 camMetrix = m_Camera->GetMatrix();
        glm::mat4 modelMatrix = m_UnitCube->GetModelMatrix();
        glm::vec3 eye = m_Camera->GetPosition();
        glm::mat4 modelView = modelMatrix * view;
        glm::mat3 modelView3x3 = glm::mat3(modelView);
        glm::mat3 normal = glm::inverse(modelView3x3);

        // light testing
        // m_LightCube->SetModelMatrix(glm::translate(glm::mat4(1.0f), lightPos));

        m_SimpleShader->Bind();
        m_SimpleShader->SetUniformMat4f("ViewMatrix", view);
        m_SimpleShader->SetUniformMat3f("NormalMatrix", normal);
        m_SimpleShader->SetUniform1f("focal_length", m_Camera->GetFocalLength());
        m_SimpleShader->SetUniform1f("aspect_ratio", WIDTH / HEIGHT);
        m_SimpleShader->SetUniform2f("viewport_size", WIDTH, HEIGHT);
        m_SimpleShader->SetUniform3f("ray_origin", viewPos.x, viewPos.y, viewPos.z);
        m_SimpleShader->SetUniform3f("top", (float)NORM_W, (float)NORM_H, (float)NORM_D);
        m_SimpleShader->SetUniform3f("bottom", -(float)NORM_W, -(float)NORM_H, -(float)NORM_D);
        m_SimpleShader->SetUniform3f("background_colour", m_ClearColor[0], m_ClearColor[1], m_ClearColor[2]);
        m_SimpleShader->SetUniform3f("material_colour", 1.0, 1.0, 1.0);
        // why tf is this not working
        // lightPos = viewPos;
        // std::cout<<lightPos.x << " " << lightPos.y << " " << lightPos.z << std::endl
        m_SimpleShader->SetUniform3f("light_position", lightPos.x, lightPos.y, lightPos.z);

        m_SimpleShader->SetUniform1f("step_length", stepLen);
        m_SimpleShader->SetUniform1f("threshold", intensityThresh);
        m_SimpleShader->SetUniform1f("gamma", gamma);
        
        m_Camera->Matrix(*m_SimpleShader, "u_CameraMatrix");
        m_VolumeTexture->Bind();
        m_TransferFunc->Bind();
        m_UnitCube->Draw(*m_SimpleShader, *m_Camera);

        // light testing
        //m_LightShader->Bind();
        //m_Camera->Matrix(*m_LightShader, "u_CameraMatrix");
        //m_LightCube->Draw(*m_LightShader, *m_Camera);
    }


    void VolumeRendering::OnImGuiRender() 
    {
        ImGui::Begin("Test");
        ImGui::ColorEdit4("Clear Color", m_ClearColor);
        ImGui::End();

        if (ImGui::CollapsingHeader("Light")){
            ImGui::Indent();
		    ImGui::SliderFloat("Intensity", &lightStrength, 0, 20);
            ImGui::SliderFloat3("Light Position", &lightPos.x, -3, 3);
            ImGui::Unindent();
        }

        if (ImGui::CollapsingHeader("Data"))
        {
            ImGui::Indent();
            ImGui::SliderFloat("Threshold", &intensityThresh, 0.0, 1.0);
            ImGui::SliderFloat("Step Lenght", &stepLen, 0.001, 1.0);
            ImGui::SliderFloat("Gamma", &gamma, 0.001, 2.0);
            ImGui::Unindent();
        }
    }
}