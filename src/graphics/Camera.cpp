#include "Camera.h"

#include <string>
#include <iostream>
#include <math.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"

/*
    Some of the code here was inspired by tutorial by:
    Victor Gordan
    https://www.youtube.com/playlist?list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-
    https://github.com/VictorGordan/opengl-tutorials/tree/main/YoutubeOpenGL%208%20-%20Camera
*/

/**
 * @brief Construct a new Camera:: Camera object.
 * 
 * @param width Width of window
 * @param height Height of window
 * @param position Position of the Camera in WS
 * @param orientation Orientation of the camera
 * @param up Up vector
 */
Camera::Camera(int width, int height, glm::vec3 position, glm::vec3 orientation, glm::vec3 up)
    : c_Width(width), c_Height(height), c_Position(position), c_Orientation(orientation), c_Up(up),
    c_Rotation(0.0f, 0.0f)
{
}

Camera::~Camera() 
{
    
}

/**
 * @brief Updates the matrix each drawing cycle.
 * 
 * @param FOVdeg Field of view
 * @param nearPlane 
 * @param farPlane 
 */
void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane) 
{
    c_View = glm::mat4(1.0f);
    c_Projection = glm::mat4(1.0f);

    // c_View = glm::lookAt(c_Position, c_Position + c_Orientation, c_Up);
    c_View = glm::lookAt(c_Position, glm::vec3(0,0,0), c_Up);
    c_Projection = glm::perspective(glm::radians(FOVdeg), (float)c_Width / c_Height, nearPlane, farPlane);

    
    c_Matrix = c_Projection * c_View;
    c_FocalLength = 1.0 / tan(M_PI / 180.0 * FOVdeg / 2.0);
}

/**
 * @brief Processes the user input.
 * 
 * @param window 
 */
void Camera::Input(GLFWwindow* window) 
{   
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
	    return;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        c_Position += c_Speed * c_Orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        //c_Position += c_Speed * -glm::normalize(glm::cross(c_Orientation, c_Up));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        c_Position += c_Speed * -c_Orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        //c_Position += c_Speed * glm::normalize(glm::cross(c_Orientation, c_Up));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        c_Position += c_Speed * c_Up;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
        c_Position += c_Speed * -c_Up;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        c_Speed = 0.4f;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE){
        c_Speed = 0.1f;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        if (firstClick)
        {
            glfwSetCursorPos(window, (c_Width / 2), (c_Height / 2));
            firstClick = false;
        }
        

        double mouseX;
        double mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        glm::vec3 theRightAxis = glm::transpose(c_View)[0];
        /*
        double dx = mouseX - (c_Width / 2);
        double dy = mouseY - (c_Height / 2);

        float scale_x = abs(dx) / c_Width;
        float scale_y = abs(dy) / c_Height;
        float rotSpeed = 350.0f;

        
        float rot = rotSpeed * scale_y;

        if (dy < 0)
        {
            if (c_Rotation.y + rot > 89.9999)
            {
                rot = 89.9999 - c_Rotation.y;
            }
            c_Position = glm::rotate(c_Position, glm::radians(rot), theRightAxis);
            c_Rotation.y += rot;
        }
        else if (dy > 0)
        {
            if (c_Rotation.y - rot < -89.9999)
            {
                rot = 89.9999 + c_Rotation.y;
            }
            c_Position = glm::rotate(c_Position, glm::radians(-rot), theRightAxis);
            c_Rotation.y -= rot;
        }

        if (dx < 0)
        {
            c_Position = glm::rotate(c_Position, glm::radians(-rotSpeed * scale_x), glm::vec3(0,1,0));
            c_Rotation.x -= rotSpeed * scale_x;
        }
        else if (dx > 0) {
            c_Position = glm::rotate(c_Position, glm::radians(rotSpeed * scale_x), glm::vec3(0,1,0));
            c_Rotation.x += rotSpeed * scale_x;
        }
        */
        

        
        float rotx = sensitivity * (float)(mouseY - (c_Height / 2)) / c_Height;
        float roty = sensitivity * (float)(mouseX - (c_Width / 2)) / c_Width;

        c_Position = glm::rotate(c_Position, glm::radians(-rotx), theRightAxis);
        c_Position = glm::rotate(c_Position, glm::radians(roty), glm::vec3(0,1,0));
        

        glfwSetCursorPos(window, (c_Width / 2), (c_Height / 2));
        
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstClick = true;
    }

}

/**
 * @brief Sets the matrix uniform. 
 * 
 * @param shader 
 * @param uniform Uniform name.
 */
void Camera::Matrix(Shader& shader, const char* uniform) 
{
    shader.SetUniformMat4f(uniform, c_Matrix);
}

