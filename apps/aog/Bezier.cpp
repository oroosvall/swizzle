
/* Include files */

#include "Bezier.hpp"

#include <fstream>
#include <string>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

glm::vec3 GetPoint(const std::vector<glm::vec3>& points, F32 t);

/* Static Function Definition */

glm::vec3 GetPoint(const std::vector<glm::vec3>& points, F32 t)
{
    std::vector<glm::vec3> tmp = points;
    size_t i = points.size() - 1u;
    while (i > 0u)
    {
        for (size_t k = 0u; k < i; k++)
        {
            tmp[k] = tmp[k] + t * (tmp[k + 1] - tmp[k]);
        }
        i--;
    }
    return tmp[0];
}

/* Function Definition */

/* Class Public Function Definition */


BezierCurve::BezierCurve(common::Resource<swizzle::gfx::GfxContext> context, std::string name)
    : mBezierPoints()
    , mLineSegment()
    , mName(name)
    , mGenerated(false)
{
    mBezierLines = context->createBuffer(swizzle::gfx::BufferType::Vertex);
}

BezierCurve::~BezierCurve()
{ }

void BezierCurve::load(const SwChar* filePath)
{
    std::ifstream file(filePath);
    if (file.is_open())
    {
        std::string line;
        while (!file.eof())
        {
            std::getline(file, line);

            size_t off0 = line.find_first_of(' ');
            size_t off1 = line.find_first_of(' ', off0 + 1);
            size_t off2 = line.find_first_of(' ', off1 + 1);

            float v1 = std::strtof(line.substr(0, off0).c_str(), nullptr);
            float v2 = std::strtof(line.substr(off0, off1 - off0).c_str(), nullptr);
            float v3 = std::strtof(line.substr(off1, off2 - off1).c_str(), nullptr);

            glm::vec3 v{ v1,v2,v3 };
            mBezierPoints.push_back(v);
        }

        generateCurve();
    }
}

void BezierCurve::generateCurve()
{
    mLineSegment.clear();

    for (F32 i = 0.0f; i < 1.0f; i += 0.01f)
    {
        mLineSegment.push_back(GetPoint(mBezierPoints, i));
    }

    mBezierLines->setBufferData(mLineSegment.data(), mLineSegment.size() * sizeof(glm::vec3), sizeof(glm::vec3));
    mGenerated = true;
}

std::vector<glm::vec3>& BezierCurve::getControlPoints()
{
    return mBezierPoints;
}

const std::vector<glm::vec3>& BezierCurve::getLineSegments() const
{
    return mLineSegment;
}

common::Resource<swizzle::gfx::Buffer> BezierCurve::getBuffer() const
{
    return mBezierLines;
}

const std::string BezierCurve::getName() const
{
    return mName;
}

const SwBool BezierCurve::isReady() const
{
    return mGenerated;
}

glm::vec3 BezierCurve::getPoint(F32 t) const
{
    return GetPoint(mBezierPoints, t);
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
