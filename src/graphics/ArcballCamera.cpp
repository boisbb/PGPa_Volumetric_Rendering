/*
* Volumetric Renderer implementation as a project for PGPa, 2MIT, VUT FIT
* Author: Boris Burkalo (xburka00), 2MIT
* 
* Some of the code in this file was inspired by:
* https://oguz81.github.io/ArcballCamera/, Author: oguz81
*/

#include "ArcballCamera.h"
#include "ArcballCamera.h"
#include "ArcballCamera.h"


ArcballCamera::ArcballCamera(int width, int height, glm::vec3 position, glm::vec3 orientation, glm::vec3 up)
    : c_Width(width), c_Height(height), c_Position(position), c_Orientation(orientation), c_Up(up)
{
}

void ArcballCamera::updateMatrix(float FOVdeg, float nearPlane, float farPlane) 
{
    c_View = glm::mat4(1.0f);
    c_Projection = glm::mat4(1.0f);

    // c_View = glm::lookAt(c_Position, c_Position + c_Orientation, c_Up);
    c_Projection = glm::perspective(glm::radians(FOVdeg), (float)c_Width / c_Height, nearPlane, farPlane);
    // c_View = glm::lookAt(c_Position, glm::vec3(0,0,0), c_Up);
    c_View = glm::translate(c_View, c_Position);
    c_View = glm::rotate(c_View, glm::radians(c_Angle), c_RotationalAxis);
    
    c_Matrix = c_Projection * c_View;
    c_FocalLength = 1.0 / tan(M_PI / 180.0 * FOVdeg / 2.0);
}


void ArcballCamera::Input(GLFWwindow* window, glm::ivec2 windowOffset) 
{   
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
	    return;
    
    double currPosX;
    double currPosY;

    if (watchCursorPos)
    {
        glfwGetCursorPos(window, &currPosX, &currPosY);
        if (currPosX >= windowOffset.x && currPosY >= windowOffset.y && currPosX <= c_Width + windowOffset.x && currPosY <= c_Height + windowOffset.y)
        {
            if ((currPosX != prevMousePosX || currPosY != prevMousePosY))
            {
                currPosX -= windowOffset.x;
                currPosY -= windowOffset.y;
                double tempPrevMousePosX = prevMousePosX -  windowOffset.x;
                double tempPrevMousePosY = prevMousePosY -  windowOffset.y;
                
                glm::vec3 startPos;
                // convert to NDC
                startPos.x = ((tempPrevMousePosX - (c_Width / 2)) / (c_Width / 2)) * c_Radius;
                startPos.y = (((c_Height / 2) - tempPrevMousePosY) / (c_Height / 2)) * c_Radius;
                startPos.z = GetZCoord(startPos.x, startPos.y);

                glm::vec3 currentPos;
                currentPos.x = ((currPosX - (c_Width / 2)) / (c_Width / 2)) * c_Radius;
                currentPos.y = (((c_Height / 2) - currPosY) / (c_Height / 2)) * c_Radius;
                currentPos.z = GetZCoord(currentPos.x, currentPos.y);

                QuaternionRotate(startPos, currentPos);

            }
        }
    }

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
        if (!watchCursorPos)
        {
            double newPrevMousePosX, newPrevMousePosY;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            glfwGetCursorPos(window, &newPrevMousePosX, &newPrevMousePosY);
            if (newPrevMousePosX >= windowOffset.x && newPrevMousePosY >= windowOffset.y && newPrevMousePosX <= c_Width + windowOffset.x && newPrevMousePosY <= c_Height + windowOffset.y)
            {
                prevMousePosX = newPrevMousePosX;
                prevMousePosY = newPrevMousePosY;
                watchCursorPos = true;
            }

        }
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && watchCursorPos
    ){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        //firstClick = true;
        watchCursorPos = false;
        lastQuaternion.cosine = cosValue2;
        lastQuaternion.axis = rotationalAxis2;
    }

}

void ArcballCamera::QuaternionRotate(glm::vec3 startPos, glm::vec3 currentPos)
{
    glm::vec3 startPosNormalized = glm::normalize(startPos);
    glm::vec3 currentPosNormalized = glm::normalize(currentPos);
    currentQuaternion.axis = glm::normalize(glm::cross(startPos, currentPos));

    float cosValue = glm::dot(startPosNormalized, currentPosNormalized);
    cosValue = cosValue > 1 ? 1 : cosValue;
    float theta = (acos(cosValue) * 180 / M_PI);
    currentQuaternion.cosine = cos((theta / 2) * M_PI / 180.f);

    currentQuaternion.axis.x = currentQuaternion.axis.x * sin((theta / 2) * 3.1416 / 180);
    currentQuaternion.axis.y = currentQuaternion.axis.y * sin((theta / 2) * 3.1416 / 180);
    currentQuaternion.axis.z = currentQuaternion.axis.z * sin((theta / 2) * 3.1416 / 180);

    cosValue2 = (currentQuaternion.cosine * lastQuaternion.cosine) - glm::dot(currentQuaternion.axis, lastQuaternion.axis);

    glm::vec3 tempVec = glm::cross(currentQuaternion.axis, lastQuaternion.axis);
    rotationalAxis2.x = (currentQuaternion.cosine * lastQuaternion.axis.x) + (lastQuaternion.cosine * currentQuaternion.axis.x) + tempVec.x;
    rotationalAxis2.y = (currentQuaternion.cosine * lastQuaternion.axis.y) + (lastQuaternion.cosine * currentQuaternion.axis.y) + tempVec.y;
    rotationalAxis2.z = (currentQuaternion.cosine * lastQuaternion.axis.z) + (lastQuaternion.cosine * currentQuaternion.axis.z) + tempVec.z;

    c_Angle = (acos(cosValue2) * 180 / M_PI) * 2;
    c_RotationalAxis.x = rotationalAxis2.x / sin((c_Angle / 2) * M_PI / 180);
    c_RotationalAxis.y = rotationalAxis2.y / sin((c_Angle / 2) * M_PI / 180);
    c_RotationalAxis.z = rotationalAxis2.z / sin((c_Angle / 2) * M_PI / 180);
}

float ArcballCamera::GetZCoord(float x, float y)
{
    return (sqrt((x * x) + (y * y)) <= c_Radius) ? (float)sqrt((c_Radius * c_Radius) - (x * x) - (y * y)) : 0;
}


void ArcballCamera::Matrix(Shader& shader, const char* uniform) 
{
    shader.SetUniformMat4f(uniform, c_Matrix);
}

