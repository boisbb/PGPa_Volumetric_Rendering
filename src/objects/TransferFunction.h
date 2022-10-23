#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <array>

#include "Renderer.h"

struct CubicSpline
{
public:
    CubicSpline(){}
    CubicSpline(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d)
        : a(a), b(b), c(c), d(d) {}
    
    glm::vec3 getPointOnSpline(const float s) const
    {
        return (((d * s) + c) * s + b) * s + a;
    }

private:
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;
    glm::vec3 d;
};

class TransferFunction
{
public:
    TransferFunction();
    TransferFunction(std::vector<glm::vec4> colorPoints, std::vector<glm::vec2> alphaPoints);
    glm::vec4 getColor(const float val);
    void createTexture();
    void Bind();
    void Unbind();
    

private:
    static std::vector<CubicSpline> CalculateCubicSpline(std::vector<glm::vec3> points);

    std::vector<glm::vec4> colorPoints;
    std::vector<glm::vec2> alphaPoints;
    std::vector<CubicSpline> colorSpline;
    std::vector<CubicSpline> alphaSpline;
    std::array<glm::vec4, 256> transferFunc;
    unsigned int transferFuncTexID;
};