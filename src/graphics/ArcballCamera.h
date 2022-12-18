#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <math.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"

#include "Shader.h"

struct Quaternion 
{
    float cosine;
    glm::vec3 axis;
};

class ArcballCamera 
{
public:
    ArcballCamera(int width, int height, glm::vec3 position, glm::vec3 orientation = glm::vec3(0.f, 0.f, -1.f), glm::vec3 up = glm::vec3(0.f, 1.f, 0.f));
    ~ArcballCamera() {}

    void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
    void Input(GLFWwindow* window);
    void Matrix(Shader& shader, const char* uniform);


    glm::mat4 GetProjection() const { return c_Projection; }
    glm::mat4 GetView() const { return c_View; }
    glm::mat4 GetMatrix() const { return c_Matrix; }
    float GetFocalLength() const { return c_FocalLength; }

    void SetDimensions(glm::ivec2 dims) { c_Width = dims.x; c_Height = dims.y; }
private:
    float GetZCoord(float x, float y);
    void QuaternionRotate(glm::vec3 startPos, glm::vec3 currentPos);

    glm::vec3 c_Position;
    glm::vec3 c_Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 c_Up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 c_Matrix = glm::mat4(1.f);
    glm::mat4 c_View;
    glm::mat4 c_Projection;

    float c_Angle = 180.f;
    glm::vec3 c_RotationalAxis = glm::vec3(1.f, 0.f, 0.f);
    
    float c_FocalLength;

    bool firstClick = true;
    bool pressed = false;

    int c_Width;
    int c_Height;
    int c_Radius = 1.f;

    float c_Speed = 0.5f;
    float sensitivity = 100.f;

    Quaternion currentQuaternion;
    Quaternion lastQuaternion = {0.f, glm::vec3(1.f, 0.f, 0.f)};
    glm::vec3 rotationalAxis2;
    float cosValue2;

    double prevMousePosX = 0.0;
    double prevMousePosY = 0.0;

    bool watchCursorPos = false;

};