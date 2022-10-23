#include "TransferFunction.h"

TransferFunction::TransferFunction()
{
    colorPoints = std::vector{
        glm::vec4(.91f, .7f, .61f, 0),
        glm::vec4(.91f, .7f, .61f, 80),
        glm::vec4(1.0f, 1.0f, .85f, 82),
        glm::vec4(1.0f, 1.0f, .85f, 256),
    };

    alphaPoints = std::vector{
        glm::vec2(0.0f, 0),
        glm::vec2(0.0f, 40),
        glm::vec2(0.05f, 63),
        glm::vec2(0.0f, 80),
        glm::vec2(0.9f, 82),
        glm::vec2(1.f, 256),
    };

    createTexture();
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
        std::cout << transferFunc[i][3] << std::endl;
        //transferFunc[i] = glm::vec4(1,0,0,1);

    }
    
    // create 1D texture
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
    /* We need to solve the equation
    * taken from: http://mathworld.wolfram.com/CubicSpline.html
    [2 1       ] [D[0]]   [3(v[1] - v[0])  ]
    |1 4 1     | |D[1]|   |3(v[2] - v[0])  |
    |  1 4 1   | | .  | = |      .         |
    |    ..... | | .  |   |      .         |
    |     1 4 1| | .  |   |3(v[n] - v[n-2])|
    [       1 2] [D[n]]   [3(v[n] - v[n-1])]
    by converting the matrix to upper triangular.
    The D[i] are the derivatives at the control points.
    */

    // build coeffs of the matrix
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

    // compute the coeffs of the cubics
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