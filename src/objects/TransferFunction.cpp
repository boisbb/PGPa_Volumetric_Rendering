/*
* Volumetric Renderer implementation as a project for PGPa, 2MIT, VUT FIT
* Author: Boris Burkalo (xburka00), 2MIT
*
* The code for the Cubic Spline and Transfer Function is inspired by/taken from:
* https://github.com/jose-villegas/VolumeRendering, Author: jose-villegas
*/

#include "TransferFunction.h"

TransferFunction::TransferFunction(std::string type)
{
    if (type == "human")
    {
        colorPoints = std::vector{
            glm::vec4(1.0f, 1.0f, 1.0f, 0),
            glm::vec4(.91f, .7f, .61f, 40),
            glm::vec4(.91f, .7f, .61f, 85),
            glm::vec4(1.0f, 1.0f, .85f, 95),
            glm::vec4(1.0f, 1.0f, .85f, 255),
        };

        alphaPoints = std::vector{
            glm::vec2(0.9f, 0),
            glm::vec2(1.f, 134),
            glm::vec2(1.f, 255),
        };
    }
    else if (type == "teapot")
    {
        colorPoints = std::vector {
            glm::vec4(1.0f, 1.0f, 1.0f, 0.f),
            glm::vec4(0.f, 255.f/255.f, 7.f/255.f, 50.726),
            glm::vec4(245.f/255.f, 0.f, 253.f/255.f, 56.514),
            glm::vec4(217.f/255.f, 222.f/255.f, 255.f/255.f, 173.676),
            glm::vec4(255.f/255.f, 217.f/255.f, 239.f/255.f, 255),
        };

        alphaPoints = std::vector{
            glm::vec2(1.f, 0),
            glm::vec2(0.957f, 249.042),
            glm::vec2(0.870, 255),
        };
    }
    else
    {
        colorPoints = std::vector {
            glm::vec4(1.0f, 1.0f, 1.0f, 19.615),
            glm::vec4(156.f/255.f, 232.f/255.f, 158.f/255.f, 160),
            glm::vec4(156.f/255.f, 232.f/255.f, 156.f/255.f, 85),
            glm::vec4(217.f/255.f, 222.f/255.f, 255.f/255.f, 95),
            glm::vec4(255.f/255.f, 217.f/255.f, 239.f/255.f, 255),
        };

        alphaPoints = std::vector{
            glm::vec2(0.978f, 0),
            glm::vec2(0.995f, 160),
            glm::vec2(0.974f, 255),
        };
    }
}

TransferFunction::TransferFunction(std::vector<glm::vec4> colorPoints, std::vector<glm::vec2> alphaPoints)
    : colorPoints(colorPoints), alphaPoints(alphaPoints)
{

}

glm::vec4 TransferFunction::getColor(const float val)
{
    float alpha = 0;
    glm::vec3 color = glm::vec3(0);
    int i = 0;
    int isoValue = val * 255;

    for (auto it = alphaPoints.cbegin(); it != --alphaPoints.cend() && i < alphaSpline.size();)
    {
        glm::vec2 ap = *it;
        glm::vec2 nextAp = *(++it);
        int currentIso = ap.y;
        int nextIso = nextAp.y;

        if(isoValue >= currentIso && isoValue <= nextIso)
        {
            CubicSpline currentCubic = alphaSpline[i];
            float evalAt = (float)(isoValue - currentIso) / (nextIso - currentIso);
            alpha = currentCubic.getPointOnSpline(evalAt).r;
            break;
        }

        i++;
    }
    
    i = 0;


    for (auto it = colorPoints.cbegin(); it != --colorPoints.cend() && i < colorSpline.size();)
    {
        glm::vec4 ap = *it;
        glm::vec4 nextAp = *(++it);
        int currentIso = ap[3];
        int nextIso = nextAp[3];

        if(isoValue >= currentIso && isoValue <= nextIso)
        {
            CubicSpline currentCubic = colorSpline[i];
            float evalAt = (float)(isoValue - currentIso) / (nextIso - currentIso);
            color = currentCubic.getPointOnSpline(evalAt);
            break;
        }

        i++;
    }

    return glm::vec4(color.r, color.g, color.b, alpha);
}

void TransferFunction::createTexture()
{
    std::vector<glm::vec3> alphaPts(alphaPoints.size());
    std::vector<glm::vec3> colorPts(colorPoints.size());

    for (int i = 0; i < alphaPoints.size(); i++)
        alphaPts[i] = glm::vec3(alphaPoints[i].x);
    
    for (int i = 0; i < colorPoints.size(); i++)
        colorPts[i] = glm::vec3(colorPoints[i].x, colorPoints[i].y, colorPoints[i].z);
    
    alphaSpline = std::move(CalculateCubicSpline(alphaPts));
    colorSpline = std::move(CalculateCubicSpline(colorPts));


    for (int i = 0; i < transferFunc.size(); i++)
    {
        transferFunc[i] = getColor((float)i / (float)255);
    }
    
    glGenTextures(1, &transferFuncTexID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, transferFuncTexID);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_FLOAT, transferFunc.data());
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_1D, 0);
}

std::vector<CubicSpline> TransferFunction::CalculateCubicSpline(std::vector<glm::vec3> points)
{
    int n = points.size() - 1;
    std::vector<glm::vec3>& v = points;
    std::vector<glm::vec3> gamma(n + 1);
    std::vector<glm::vec3> delta(n + 1);
    std::vector<glm::vec3> D(n + 1);

    int i;

    gamma[0] = glm::vec3(0);
    gamma[0].x = 1.0f / 2.0f;
    gamma[0].y = 1.0f / 2.0f;
    gamma[0].z = 1.0f / 2.0f;

    for (i = 1; i < n; i++)
        gamma[i] = glm::vec3(1) / ((4.0f * glm::vec3(1)) - gamma[i - 1]);
    
    gamma[n] = glm::vec3(1) / ((2.0f * glm::vec3(1)) - gamma[n - 1]);
    delta[0] = 3.0f * (v[1] - v[0]) * gamma[0];

    for (i = 1; i < n; i++)
        delta[i] = (3.0f * (v[i + 1] - v[i - 1] - delta[i - 1]) * gamma[i]);
    
    delta[n] = (3.0f * (v[n] - v[n - 1]) - delta[n - 1]) * gamma[n];
    D[n] = delta[n];

    for (i = n - 1; i >= 0; i--)
        D[i] = delta[i] - gamma[i] * D[i + 1];

    std::vector<CubicSpline> C(n);

    for (i = 0; i < n; i++)
    {
        glm::vec3 a = v[i];
        glm::vec3 b = D[i];
        glm::vec3 c = 3.0f * (v[i + 1] - v[i]) - 2.0f * D[i] - D[i + 1];
        glm::vec3 d = 2.0f * (v[i] - v[i + 1]) + D[i] + D[i + 1];
        C[i] = CubicSpline(a, b, c, d);
    }

    return C;
}

void TransferFunction::Bind()
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, transferFuncTexID);
}

void TransferFunction::Unbind()
{
    glBindTexture(GL_TEXTURE_1D, 0);
}

void TransferFunction::setColorPointAtIdx(int id, glm::vec4 values)
{
    if (id < colorPoints.size() - 1)
    {
        float succVal = colorPoints[id + 1].a;
        if (values.a > succVal)
            return;
    }

    if (id > 0)
    {
        float lowVal = colorPoints[id - 1].a;
        if (values.a < lowVal)
            return;
    }
    colorPoints[id] = values;
}

void TransferFunction::setAlphaPointAtIdx(int id, glm::vec2 values)
{
    if (id < alphaPoints.size() - 1)
    {
        float succVal = alphaPoints[id + 1].y;
        if (values.y > succVal)
            return;
    }

    if (id > 0)
    {
        float lowVal = alphaPoints[id - 1].y;
        if (values.y < lowVal)
            return;
    }
    alphaPoints[id] = values;
}