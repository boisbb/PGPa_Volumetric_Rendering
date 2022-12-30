/*
* Volumetric Renderer implementation as a project for PGPa, 2MIT, VUT FIT
* Author: Boris Burkalo (xburka00), 2MIT
*/

#pragma once

#include <functional>
#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

namespace test_model {
    
    class TestModel{
    public:
        TestModel() {}
        virtual ~TestModel() {}

        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender() {}
        virtual void OnImGuiRender() {}
        virtual void OnWindowResize(glm::vec2 size) {}


    };

    class TestModelMenu : public TestModel{
    public:
        TestModelMenu(TestModel*& currentTestModelPointer);

        void OnWindowResize(glm::vec2 size) override;
        void OnImGuiRender() override;
        TestModel* SetTestModel(const char* testModelName);

        template<typename T>
        void RegisterTestModel(const std::string& name){

            m_TestModels.push_back(std::make_pair(name, []() { return new T(); }));
        }

        template<typename T>
        void RegisterTestModel(const std::string& name, GLFWwindow* window, int width, int height){

            m_TestModels.push_back(std::make_pair(name, [window, width, height]() { return new T(window, width, height); }));
        }

        
    private:
        TestModel*& m_CurrentTestModel;
        std::vector<std::pair<std::string, std::function<TestModel*()>>> m_TestModels;
    };
}