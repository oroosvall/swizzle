
#pragma once

/* Include files */

#include <swizzle/Swizzle.hpp>
#include <swizzle/gfx/GfxDevice.hpp>
#include <physics/Physics.hpp>

#include <vector>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

struct ColliderVertex
{
    glm::vec3 mPos;
    glm::vec3 mColor;
};

/* Class Declaration */

class PhysicsColliderRenderer
{
public:
    PhysicsColliderRenderer(common::Resource<sw::gfx::GfxDevice> ctx, common::Resource<sw::gfx::Swapchain> swp);

    void reset();
    void addAABB(const physics::AABB& aabb, glm::vec3 color = {1.0f, 1.0f, 1.0f});
    void addOOBB(const physics::OOBB& oobb, glm::vec3 color = {1.0f, 1.0f, 1.0f});
    void drawLine(const glm::vec3& pos1, const glm::vec3& pos2);
    void flush();

    common::Resource<sw::gfx::Shader> getShader() const;
    common::Resource<sw::gfx::GfxBuffer> getVertexBuffer() const;

private:

    void pushLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& color);

    std::vector<ColliderVertex> mBuildBuffer;

    common::Resource<sw::gfx::GfxBuffer> mVertexBuffer;
    common::Resource<sw::gfx::Shader> mShader;
};
/* Function Declaration */
