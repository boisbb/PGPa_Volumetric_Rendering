#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "Shader.h"


class Camera {
public:
    Camera(int width, int height, glm::vec3 position, glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));
    ~Camera();

    void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
    void Input(GLFWwindow* window);
    void Matrix(Shader& shader, const char* uniform);

    glm::mat4 GetProjection() const { return c_Projection; }
    glm::mat4 GetView() const { return c_View; }
    glm::mat4 GetMatrix() const { return c_Matrix; }
    glm::vec3 GetPosition() const { return c_Position; }
    float GetFocalLength() const { return c_FocalLength; }

    glm::vec3 c_Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 c_Up = glm::vec3(0.0f, 1.0f, 0.0f);

private:
    glm::vec3 c_Position;
    
    glm::mat4 c_Matrix = glm::mat4(1.0f);
    glm::mat4 c_View;
    glm::mat4 c_Projection;
    float c_FocalLength;

    bool firstClick = true;

    int c_Width;
    int c_Height;

    float c_Speed = 0.5f;
    float sensitivity = 100.0f;
};