/*
* Volumetric Renderer implementation as a project for PGPa, 2MIT, VUT FIT
* Author: Boris Burkalo (xburka00), 2MIT
*/

#include "TestModel.h"
#include "imgui.h"

namespace test_model
{

    TestModelMenu::TestModelMenu(TestModel*& currentTestModelPointer) 
        : m_CurrentTestModel(currentTestModelPointer)
    {
    }
    
    void TestModelMenu::OnImGuiRender() 
    {
        for (auto& test_model: m_TestModels){
            if (ImGui::Button(test_model.first.c_str()))
                m_CurrentTestModel = test_model.second();
        }
    }

    TestModel* TestModelMenu::SetTestModel(const char* testModelName) 
    {
        for (auto& test_model: m_TestModels){
            if (!strcmp(testModelName, test_model.first.c_str()))
                return test_model.second();
        }

        return NULL;
    }

    void TestModelMenu::OnWindowResize(glm::vec2 size)
    {

    }
}