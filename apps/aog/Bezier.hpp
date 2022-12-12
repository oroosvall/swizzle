#ifndef BEZIER_HPP
#define BEZIER_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/gfx/Context.hpp>

#include <glm/glm.hpp>
#include <vector>
#include <string>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

class BezierCurve
{
public:

    BezierCurve(common::Resource<swizzle::gfx::GfxContext> context, std::string name);
    ~BezierCurve();

    void load(const SwChar* filePath);

    void generateCurve();

    std::vector<glm::vec3>& getControlPoints();
    const std::vector<glm::vec3>& getLineSegments() const;

    common::Resource<swizzle::gfx::Buffer> getBuffer() const;
    const std::string getName() const;

    const SwBool isReady() const;

    glm::vec3 getPoint(F32 t) const;

private:

    std::vector<glm::vec3> mBezierPoints;
    std::vector<glm::vec3> mLineSegment;

    common::Resource<swizzle::gfx::Buffer> mBezierLines;
    std::string mName;

    SwBool mGenerated;

};

/* Function Declaration */

#endif
