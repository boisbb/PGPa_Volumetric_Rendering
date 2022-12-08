#include "VolumeRendering.h"

#include "imgui.h"

#define VOLUME_W 256
#define VOLUME_H 256
#define VOLUME_D 256

#define HEAD_ORIG 0
#define FOOT 1
#define BONSAI 2
#define TEAPOT 3
#define HEAD 4
#define MANIX 5
#define SKULL 6
#define TOOTH 7

#define MODEL TOOTH

#if MODEL == HEAD_ORIG
#define VOLUME_D 109
#define MODEL_NAME "head256x256x109"
#define TRANSFER_FUNC_TYPE "human"

#elif MODEL == FOOT
#define MODEL_NAME "foot.raw"
#define TRANSFER_FUNC_TYPE "human"

#elif MODEL == BONSAI
#define MODEL_NAME "bonsai.raw"
#define TRANSFER_FUNC_TYPE "other"

#elif MODEL == TEAPOT
#define VOLUME_D 178
#define MODEL_NAME "teapot.raw"
#define TRANSFER_FUNC_TYPE "teapot"

#elif MODEL == HEAD
#define VOLUME_D 225
#define MODEL_NAME "head.raw"
#define TRANSFER_FUNC_TYPE "human"

#elif MODEL == MANIX
#define VOLUME_W 128
#define VOLUME_H 128
#define VOLUME_D 115
#define MODEL_NAME "manix.raw"
#define TRANSFER_FUNC_TYPE "human"

#elif MODEL == SKULL
#define VOLUME_D 68
#define MODEL_NAME "skull.raw"
#define TRANSFER_FUNC_TYPE "human"

#elif MODEL == TOOTH
#define VOLUME_D 151
#define MODEL_NAME "tooth.raw"
#define TRANSFER_FUNC_TYPE "human"
#endif

#define NORM_W ((float)VOLUME_W * 1.f) / 256.f
#define NORM_H ((float)VOLUME_H * 1.f) / 256.f
#define NORM_D ((float)VOLUME_D * 1.f) / 256.f

namespace test_model
{
    VolumeRendering::VolumeRendering(GLFWwindow* window, int width, int height)
        : m_ClearColor {0.0f, 0.0f, 0.0f, 1.0f}, WIDTH(width), HEIGHT(height), m_Window(window)             
    {
        fs::create_directory("../screenshots");

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_ArcCamera = std::make_unique<ArcballCamera>(WIDTH, HEIGHT, glm::vec3(0, 0, -3));
        m_SimpleShader = std::make_unique<Shader>("../res/shaders/isometricTransferF.shader");
        m_LightShader = std::make_unique<Shader>("../res/shaders/lightCube.shader");
        m_UnitCube = std::make_unique<UnitCube>();
        m_LightCube = std::make_unique<UnitCube>();
        m_VolumeTexture = std::make_unique<VolumeTexture>("../res/textures/" + (std::string)MODEL_NAME, glm::vec3(VOLUME_W, VOLUME_H, VOLUME_D), true);
        m_TransferFunc = std::make_unique<TransferFunction>(TRANSFER_FUNC_TYPE);
        m_TransferFunc->createTexture();

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

        //m_Camera->Input(m_Window);
        //m_Camera->updateMatrix(FOV, 0.1f, 1000.0f);
        m_ArcCamera->Input(m_Window);
        m_ArcCamera->updateMatrix(FOV, 0.1f, 100.f);

        glm::mat4 view = m_ArcCamera->GetView();
        glm::mat4 projection = m_ArcCamera->GetProjection();
        glm::mat4 camMetrix = m_ArcCamera->GetMatrix();
        glm::mat4 modelMatrix = m_UnitCube->GetModelMatrix();
        glm::mat4 modelView = modelMatrix * view;
        glm::mat4 viewModel = glm::inverse(modelView);
        glm::mat3 modelView3x3 = glm::mat3(modelView);
        glm::mat3 model3x3 = glm::mat3(modelMatrix);
        glm::mat3 normal = glm::transpose(glm::inverse(model3x3));
        glm::vec3 viewPos = glm::vec3(viewModel[3][0], viewModel[3][1], viewModel[3][2]);

        // light testing
        //m_LightCube->SetModelMatrix(glm::scale(glm::translate(glm::mat4(1.0f), lightPos), glm::vec3(0.5)));

        m_SimpleShader->Bind();
        m_SimpleShader->SetUniformMat4f("ViewMatrix", view);
        m_SimpleShader->SetUniformMat3f("NormalMatrix", normal);
        m_SimpleShader->SetUniform1f("focal_length", m_ArcCamera->GetFocalLength());
        m_SimpleShader->SetUniform1f("aspect_ratio", WIDTH / HEIGHT);
        m_SimpleShader->SetUniform1f("dither_ratio", dither_ratio);
        m_SimpleShader->SetUniform2f("viewport_size", WIDTH, HEIGHT);
        m_SimpleShader->SetUniform3f("ray_origin", viewPos.x, viewPos.y, viewPos.z);
        m_SimpleShader->SetUniform3f("top", (float)NORM_W, (float)NORM_H, (float)NORM_D);
        m_SimpleShader->SetUniform3f("bottom", -(float)NORM_W, -(float)NORM_H, -(float)NORM_D);
        m_SimpleShader->SetUniform3f("background_colour", m_ClearColor[0], m_ClearColor[1], m_ClearColor[2]);
        m_SimpleShader->SetUniform3f("material_colour", 1.0, 1.0, 1.0);
        // why tf is this not working
        lightPos = viewPos;
        // std::cout<<lightPos.x << " " << lightPos.y << " " << lightPos.z << std::endl
        m_SimpleShader->SetUniform3f("light_position", lightPos.x, lightPos.y, lightPos.z);

        m_SimpleShader->SetUniform1f("step_length", stepLen);
        m_SimpleShader->SetUniform1f("threshold", intensityThresh);
        m_SimpleShader->SetUniform1f("gamma", gamma);
        
        m_ArcCamera->Matrix(*m_SimpleShader, "u_CameraMatrix");
        m_VolumeTexture->Bind();
        m_TransferFunc->Bind();
        m_UnitCube->Draw(*m_SimpleShader, *m_ArcCamera);

        // light testing
        m_LightShader->Bind();
        m_ArcCamera->Matrix(*m_LightShader, "u_CameraMatrix");
        //m_LightCube->Draw(*m_LightShader, *m_ArcCamera);

        if (takeScreenshot)
        {
            TakeScreenshot();
            takeScreenshot = false;
        }
    }


    void VolumeRendering::OnImGuiRender() 
    {
        ImGui::Begin("Test");
        ImGui::ColorEdit4("Clear Color", m_ClearColor);
        ImGui::End();

        if (ImGui::Button("Take screenshot"))
        {
            takeScreenshot = true;
        }

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
            ImGui::SliderFloat("Step Lenght", &stepLen, 0.001, 0.02);
            ImGui::SliderFloat("Dithering ratio", &dither_ratio, 0.5, 30);
            ImGui::SliderFloat("Gamma", &gamma, 0.001, 2.0);
            
            if (ImGui::CollapsingHeader("Color Control Points"))
            {
                std::vector<glm::vec4> currentControlPoints = m_TransferFunc->getColorPoints();
                for (int i = 0; i < currentControlPoints.size(); i++)
                {
                    // TODO: possibly very inefficient
                    glm::vec4 currentControlPoint = currentControlPoints.at(i);
                    float controlPoint[3] = {currentControlPoint.x, currentControlPoint.y, currentControlPoint.z};
                    float threshold = currentControlPoint.w;
                    //controlPoints.push_back({ currentControlPoint.x, currentControlPoint.y, currentControlPoint.z });
                    ImGui::ColorEdit3(("Color Point #" + std::to_string(i)).c_str(), controlPoint);
                    ImGui::SliderFloat(("Threshold #" + std::to_string(i)).c_str(), &threshold, 0, 255);

                    if (controlPoint[0] != currentControlPoint.x 
                    || controlPoint[1] != currentControlPoint.y 
                    || controlPoint[2] != currentControlPoint.z
                    || threshold != currentControlPoint.w)
                    {
                        m_TransferFunc->setColorPointAtIdx(i, glm::vec4(controlPoint[0], controlPoint[1], controlPoint[2], threshold));
                        m_TransferFunc->createTexture();
                    }
                }

            }

            if (ImGui::CollapsingHeader("Alpha Control Points"))
            {
                std::vector<glm::vec2> currentControlPoints = m_TransferFunc->getAlphaPoints();
                for (int i = 0; i < currentControlPoints.size(); i++)
                {
                    // TODO: possibly very inefficient
                    glm::vec2 currentControlPoint = currentControlPoints.at(i);
                    float controlPoint = currentControlPoint.x;
                    float threshold = currentControlPoint.y;
                    //controlPoints.push_back({ currentControlPoint.x, currentControlPoint.y, currentControlPoint.z });
                    ImGui::SliderFloat(("Alpha #" + std::to_string(i)).c_str(), &controlPoint, 0, 1.0f);
                    ImGui::SliderFloat(("AThreshold #" + std::to_string(i)).c_str(), &threshold, 0, 255);

                    if (controlPoint != currentControlPoint.x 
                    || threshold != currentControlPoint.y)
                    {
                        m_TransferFunc->setAlphaPointAtIdx(i, glm::vec2(controlPoint, threshold));
                        m_TransferFunc->createTexture();
                    }
                }
            }
            
            ImGui::Unindent();
        }
    }

    void VolumeRendering::TakeScreenshot()
    {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream nowSS;
        nowSS << std::put_time(localtime(&now), "%F-%H-%M-%S");

        std::string fn = "../screenshots/" + nowSS.str() + ".tga";
        int* buffer = new int[WIDTH * HEIGHT * 3];
        glReadPixels( 0, 0, WIDTH, HEIGHT, GL_BGR, GL_UNSIGNED_BYTE, buffer );

        FILE   *out = fopen(fn.c_str(), "w");
        short TGAhead[] = {0, 2, 0, 0, 0, 0, WIDTH, HEIGHT, 24};
        fwrite(&TGAhead, sizeof(TGAhead), 1, out);
        fwrite(buffer, 3 * WIDTH * HEIGHT, 1, out);
        fclose(out);
        delete buffer;
    }
}