#include <GL/glew.h>
#include <GLFW/glfw3.h>

/*
http://docs.gl/
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "models/TestModel.h"
#include "models/VolumeRendering.h"

int WIDTH = 800;
int HEIGHT = 800;


int main(int argc, char* argv[])
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Volumetric Rendering", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        cout << "Error glewInit()" << endl;
        return 1;
    }
    cout << glGetString(GL_VERSION) << endl;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glViewport(0,0, WIDTH, HEIGHT);

    Renderer renderer;

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();

    test_model::TestModel* currentTestModel = NULL;
    test_model::TestModelMenu* testModelMenu = new test_model::TestModelMenu(currentTestModel);
    currentTestModel = testModelMenu;

    // testModelMenu->RegisterTestModel<test_model::CarModels>("CarModels", window, WIDTH, HEIGHT);
    testModelMenu->RegisterTestModel<test_model::VolumeRendering>("VolumeRendering", window, WIDTH, HEIGHT);

    if (argc == 2)
    {
        currentTestModel = testModelMenu->SetTestModel(argv[1]);
        
    }

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        renderer.Clear();

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        if (w != WIDTH || h != HEIGHT)
        {
            WIDTH = w;
            HEIGHT = h;
            glViewport(0,0, w, h);
            currentTestModel->ModelReinit(w, h);
        }
        

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Menu");
        if(currentTestModel){
            currentTestModel->OnUpdate(0.0f);
            currentTestModel->OnRender();

            if (currentTestModel != testModelMenu && (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS || ImGui::Button("<-"))){
                delete currentTestModel;
                currentTestModel = testModelMenu;
            }
            currentTestModel->OnImGuiRender();
        }
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    delete currentTestModel;
    if (currentTestModel != testModelMenu){
        delete testModelMenu;
    }
    
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
