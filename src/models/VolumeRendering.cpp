/*
* Volumetric Renderer implementation as a project for PGPa, 2MIT, VUT FIT
* Author: Boris Burkalo (xburka00), 2MIT
*/

#include "VolumeRendering.h"

#include "imgui.h"

std::vector<std::string> models
{
    "foot.raw",
    "bonsai.raw",
    "teapot.raw",
    "head.raw",
    "manix.raw",
    "tooth.raw",
};

std::vector<glm::vec3> sizes
{
    glm::vec3(256, 256, 256),
    glm::vec3(256, 256, 256),
    glm::vec3(256, 256, 178),
    glm::vec3(256, 256, 225),
    glm::vec3(128, 128, 115),
    glm::vec3(256, 256, 151),
};

std::vector<std::string> tfs
{
    "human",
    "teapot"
};

namespace test_model
{
    VolumeRendering::VolumeRendering(GLFWwindow* window, int width, int height)
        : m_ClearColor {0.0f, 0.0f, 0.0f, 1.0f}, windowSize(width, height), m_Window(window),
        currentWindowSize(width, height), currentModel(models[3]), currentTransferFunc(tfs[0]), currentModelSize(sizes[3]),
        currentModelSizeNorm(sizes[3].x / 256.f, sizes[3].y / 256.f, sizes[3].z / 256.f), sliceIndices(sizes[3].x / 2, sizes[3].y / 2, sizes[3].z / 2)
    {
        fs::create_directory("../screenshots");

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_ArcCamera = std::make_unique<ArcballCamera>(currentWindowSize.x, currentWindowSize.y, glm::vec3(0, 0, -3));
        m_SimpleShader = std::make_unique<Shader>("../res/shaders/isometricTransferF.shader");
        m_SurfacePlaneShader = std::make_unique<Shader>("../res/shaders/surfacePlane.shader");
        m_UnitCube = std::make_unique<UnitCube>();
        m_VolumeTexture = std::make_unique<VolumeTexture>("../res/textures/" + currentModel, currentModelSize, true);
        m_TransferFunc = std::make_unique<TransferFunction>(currentTransferFunc);
        m_TransferFunc->createTexture();

        m_UnitCube->SetModelMatrix(glm::scale(m_UnitCube->GetModelMatrix(), currentModelSizeNorm));

    }

    VolumeRendering::~VolumeRendering()
    {

    }

    void VolumeRendering::OnUpdate(float deltaTime)
    {

    }

    void VolumeRendering::OnRender()
    {
        glViewport(0 + windowOffset.x, pixelOffset + windowOffset.y, currentWindowSize.x, currentWindowSize.y);
        glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float FOV = 45.0f;

        m_ArcCamera->Input(m_Window, windowOffset);
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
        lightPos = viewPos;

        m_SimpleShader->Bind();
        m_SimpleShader->SetUniformMat4f("ViewMatrix", view);
        m_SimpleShader->SetUniformMat3f("NormalMatrix", normal);
        m_SimpleShader->SetUniform1f("focal_length", m_ArcCamera->GetFocalLength());
        m_SimpleShader->SetUniform1f("aspect_ratio", currentWindowSize.x / currentWindowSize.y);
        m_SimpleShader->SetUniform1f("pixel_offset", pixelOffset);
        m_SimpleShader->SetUniform2f("windowOffset", windowOffset.x, windowOffset.y);
        m_SimpleShader->SetUniform2f("viewport_size", currentWindowSize.x, currentWindowSize.y);
        m_SimpleShader->SetUniform3f("ray_origin", viewPos.x, viewPos.y, viewPos.z);
        m_SimpleShader->SetUniform3f("top", (float)currentModelSizeNorm.x, (float)currentModelSizeNorm.y, (float)currentModelSizeNorm.z);
        m_SimpleShader->SetUniform3f("bottom", -(float)currentModelSizeNorm.x, -(float)currentModelSizeNorm.y, -(float)currentModelSizeNorm.z);
        m_SimpleShader->SetUniform3f("background_colour", 1.0, m_ClearColor[1], m_ClearColor[2]);
        m_SimpleShader->SetUniform3f("material_colour", 1.0, 1.0, 1.0);
        m_SimpleShader->SetUniform3f("light_position", lightPos.x, lightPos.y, lightPos.z);

        m_SimpleShader->SetUniform1f("step_length", stepLen);
        m_SimpleShader->SetUniform1f("threshold", intensityThresh);
        
        m_ArcCamera->Matrix(*m_SimpleShader, "u_CameraMatrix");
        m_VolumeTexture->Bind();
        m_TransferFunc->Bind();
        m_UnitCube->Draw(*m_SimpleShader);

        // Surface Planes rendering
        if (splitScreen)
        {
            m_SurfacePlaneShader->Bind();
            m_SimpleShader->SetUniformMat4f("u_CameraMatrix", glm::mat4(1.f));

            glViewport(0 + windowOffset.x, 0 + windowOffset.y, currentWindowSize.x, currentWindowSize.y);
            m_VolumeTexture->surfacePlaneZ->Draw(*m_SurfacePlaneShader);

            glViewport(currentWindowSize.x + windowOffset.x, 0 + windowOffset.y, currentWindowSize.x, currentWindowSize.y);
            m_VolumeTexture->surfacePlaneY->Draw(*m_SurfacePlaneShader);

            glViewport(currentWindowSize.x + windowOffset.x, currentWindowSize.x + windowOffset.y, currentWindowSize.x, currentWindowSize.y);
            m_VolumeTexture->surfacePlaneX->Draw(*m_SurfacePlaneShader);
        }

        if (takeScreenshot)
        {
            TakeScreenshot();
            takeScreenshot = false;
        }
    }


    void VolumeRendering::OnImGuiRender() 
    {
        ImGui::Begin("Volumetric Rendering App");

        if (ImGui::Checkbox("Show Slices", &splitScreen))
        {
            currentWindowSize = (currentWindowSize.x == windowSize.x) ? windowSize / 2 : windowSize;

            pixelOffset = currentWindowSize.x % windowSize.x;
            m_ArcCamera->SetDimensions(currentWindowSize);
        }

        if (ImGui::Button("Take screenshot"))
        {
            takeScreenshot = true;
        }

        if (ImGui::CollapsingHeader("General Settings")){
            ImGui::Indent();
            ImGui::ColorEdit4("Background Color", m_ClearColor);
            ImGui::SliderFloat("Raycasting Threshold", &intensityThresh, 0.0, 1.0);
            ImGui::SliderFloat("Sample Step Lenght", &stepLen, 0.001, 0.02);
            ImGui::Unindent();
        }

        if (ImGui::CollapsingHeader("Model Selection"))
        {
            ImGui::Indent();
            std::string prevModel = currentModel;

            if (ImGui::BeginCombo("##combo", currentModel.c_str()))
            {
                for (int n = 0; n < models.size(); n++)
                {
                    bool is_selected = (currentModel == models[n]);
                    if (ImGui::Selectable(models[n].c_str(), is_selected))
                        currentModel = models[n];

                    if (is_selected){
                        
                        ImGui::SetItemDefaultFocus();
                    }

                    if (currentModel != prevModel)
                    {
                        prevModel = currentModel;
                        int idx = find(models.begin(), models.end(), currentModel) - models.begin();
                        currentModelSize = sizes[idx];
                        int maxSize = std::max(currentModelSize.x, std::max(currentModelSize.y, currentModelSize.z));
                        currentModelSizeNorm = glm::vec3(currentModelSize.x / maxSize, currentModelSize.y / maxSize, currentModelSize.z / maxSize);
                        m_VolumeTexture->UpdateFromBin("../res/textures/" + currentModel, currentModelSize);
                        m_UnitCube->SetModelMatrix(glm::scale(glm::mat4(1.f), currentModelSizeNorm));
                    }
                    
                }
                ImGui::EndCombo();
            }
            ImGui::Unindent();
        }

        if (splitScreen)
        {
            if (ImGui::CollapsingHeader("Slices Settings"))
            {
                ImGui::Indent();
                if (ImGui::SliderInt("Slice X", &sliceIndices.x, 0, (int)currentModelSize.x - 1))
                {
                    m_VolumeTexture->UpdateSPSlice(X, sliceIndices.x);
                }
                if (ImGui::SliderInt("Slice Y", &sliceIndices.y, 0, (int)currentModelSize.y - 1))
                {
                    m_VolumeTexture->UpdateSPSlice(Y, sliceIndices.y);
                }
                if (ImGui::SliderInt("Slice Z", &sliceIndices.z, 0, (int)currentModelSize.z - 1))
                {
                    m_VolumeTexture->UpdateSPSlice(Z, sliceIndices.z);
                }
                ImGui::Unindent();
            }
        }

        if (ImGui::CollapsingHeader("Transfer Function Settings"))
        {
            ImGui::Indent();

            std::string prevTf = currentTransferFunc;
            if (ImGui::BeginCombo("##combo2", currentTransferFunc.c_str()))
            {
                for (int n = 0; n < tfs.size(); n++)
                {
                    bool is_selected = (currentTransferFunc == tfs[n]);
                    if (ImGui::Selectable(tfs[n].c_str(), is_selected))
                        currentTransferFunc = tfs[n];

                    if (is_selected){
                        
                        ImGui::SetItemDefaultFocus();
                    }

                    if (currentTransferFunc != prevTf)
                    {
                        m_TransferFunc.reset();
                        m_TransferFunc = std::make_unique<TransferFunction>(currentTransferFunc);
                        m_TransferFunc->createTexture();
                    }
                    
                }
                ImGui::EndCombo();
            }
            
            if (ImGui::CollapsingHeader("Color Control Points"))
            {
                std::vector<glm::vec4> currentControlPoints = m_TransferFunc->getColorPoints();
                for (int i = 0; i < currentControlPoints.size(); i++)
                {
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
                    glm::vec2 currentControlPoint = currentControlPoints.at(i);
                    float controlPoint = currentControlPoint.x;
                    float threshold = currentControlPoint.y;
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
        ImGui::End();
    }

    void VolumeRendering::OnWindowResize(glm::vec2 size)
    {
        int newWindowSize = std::min((int)size.x, (int)size.y);
        windowOffset.x = (size.x - newWindowSize) / 2;
        windowOffset.y = (size.y - newWindowSize) / 2;

        windowSize.x = newWindowSize;
        windowSize.y = newWindowSize;

        currentWindowSize = splitScreen ? windowSize / 2 : windowSize;

        pixelOffset = currentWindowSize.x % windowSize.x;
        m_ArcCamera->SetDimensions(currentWindowSize);
    }

    void VolumeRendering::TakeScreenshot()
    {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream nowSS;
        nowSS << std::put_time(localtime(&now), "%F-%H-%M-%S");

        // Following part of the code is inspired by this stack overflow thread:
        // https://stackoverflow.com/questions/5844858/how-to-take-screenshot-in-opengl
        // Author is user nicknamed huy
        glm::ivec2 size{ windowSize.x - windowSize.x % 4,  windowSize.y - windowSize.y % 4};
        std::string fn = "../screenshots/" + nowSS.str() + ".tga";
        int* buffer = new int[size.x * size.y * 3];

        glReadPixels( windowOffset.x, windowOffset.y, size.x, size.y, GL_BGR, GL_UNSIGNED_BYTE, buffer );

        FILE   *out = fopen(fn.c_str(), "w");
        short TGAhead[] = {0, 2, 0, 0, 0, 0, (short)size.x, (short)size.y, (short)24};
        fwrite(&TGAhead, sizeof(TGAhead), 1, out);
        fwrite(buffer, 3 * size.x * size.y, 1, out);
        fclose(out);
        delete buffer;
    }
}