
/* Include files */

#include "PhysicsColliderRenderer.hpp"
#include <glm/gtx/quaternion.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

PhysicsColliderRenderer::PhysicsColliderRenderer(common::Resource<sw::gfx::GfxContext> ctx,
                                                 common::Resource<sw::gfx::Swapchain> swp)
    : mBuildBuffer()
    , mVertexBuffer(nullptr)
    , mShader(nullptr)
{
    mVertexBuffer = ctx->createBuffer(sw::gfx::BufferType::Vertex);

    sw::gfx::ShaderAttributeList attribs{};
    attribs.mBufferInput = {{sw::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3u + 3u)}};
    attribs.mAttributes = {{0u, sw::gfx::ShaderAttributeDataType::vec3f, 0u},
                           {0u, sw::gfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3u}};
    attribs.mDescriptors = {};
    attribs.mPushConstantSize = sizeof(glm::mat4);
    attribs.mEnableDepthTest = true;
    attribs.mEnableDepthWrite = true;
    attribs.mEnableBlending = false;
    attribs.mPrimitiveType = sw::gfx::PrimitiveType::linelist;

    mShader = ctx->createShader(swp, sw::gfx::ShaderType::ShaderType_Graphics, attribs);
    mShader->load("shaders/physicsCollider.shader");
}

void PhysicsColliderRenderer::reset()
{
    mBuildBuffer.clear();
}

void PhysicsColliderRenderer::addAABB(const physics::AABB& aabb)
{
    glm::vec3 min = aabb.mHalfSlab;
    glm::vec3 max = -aabb.mHalfSlab;

    std::array<glm::vec3, 8> vertices{};

    vertices[0] = aabb.mPos + min;
    vertices[1] = aabb.mPos + glm::vec3(min.x, min.y, max.z);
    vertices[2] = aabb.mPos + glm::vec3(min.x, max.y, min.z);
    vertices[3] = aabb.mPos + glm::vec3(min.x, max.y, max.z);
    vertices[4] = aabb.mPos + glm::vec3(max.x, min.y, min.z);
    vertices[5] = aabb.mPos + glm::vec3(max.x, min.y, max.z);
    vertices[6] = aabb.mPos + glm::vec3(max.x, max.y, min.z);
    vertices[7] = aabb.mPos + max;

    pushLine(vertices[0], vertices[1], {1.0f, 1.0f, 1.0f});
    pushLine(vertices[0], vertices[2], {1.0f, 1.0f, 1.0f});
    pushLine(vertices[1], vertices[3], {1.0f, 1.0f, 1.0f});
    pushLine(vertices[2], vertices[3], {1.0f, 1.0f, 1.0f});

    pushLine(vertices[7], vertices[6], {1.0f, 1.0f, 1.0f});
    pushLine(vertices[7], vertices[5], {1.0f, 1.0f, 1.0f});
    pushLine(vertices[6], vertices[4], {1.0f, 1.0f, 1.0f});
    pushLine(vertices[5], vertices[4], {1.0f, 1.0f, 1.0f});

    pushLine(vertices[0], vertices[4], {1.0f, 1.0f, 1.0f});
    pushLine(vertices[1], vertices[5], {1.0f, 1.0f, 1.0f});
    pushLine(vertices[2], vertices[6], {1.0f, 1.0f, 1.0f});
    pushLine(vertices[3], vertices[7], {1.0f, 1.0f, 1.0f});
}

void PhysicsColliderRenderer::addOOBB(const physics::OOBB& oobb)
{
    glm::vec3 min = oobb.mHalfSlab;
    glm::vec3 max = -oobb.mHalfSlab;

    std::array<glm::vec3, 8> vertices{};

    vertices[0] = oobb.mPos + oobb.mRot * min;
    vertices[1] = oobb.mPos + oobb.mRot * glm::vec3(min.x, min.y, max.z);
    vertices[2] = oobb.mPos + oobb.mRot * glm::vec3(min.x, max.y, min.z);
    vertices[3] = oobb.mPos + oobb.mRot * glm::vec3(min.x, max.y, max.z);
    vertices[4] = oobb.mPos + oobb.mRot * glm::vec3(max.x, min.y, min.z);
    vertices[5] = oobb.mPos + oobb.mRot * glm::vec3(max.x, min.y, max.z);
    vertices[6] = oobb.mPos + oobb.mRot * glm::vec3(max.x, max.y, min.z);
    vertices[7] = oobb.mPos + oobb.mRot * max;

    pushLine(vertices[0], vertices[1], {1.0f, 1.0f, 1.0f});
    pushLine(vertices[0], vertices[2], {1.0f, 1.0f, 1.0f});
    pushLine(vertices[1], vertices[3], {1.0f, 1.0f, 1.0f});
    pushLine(vertices[2], vertices[3], {1.0f, 1.0f, 1.0f});

    pushLine(vertices[7], vertices[6], {1.0f, 1.0f, 1.0f});
    pushLine(vertices[7], vertices[5], {1.0f, 1.0f, 1.0f});
    pushLine(vertices[6], vertices[4], {1.0f, 1.0f, 1.0f});
    pushLine(vertices[5], vertices[4], {1.0f, 1.0f, 1.0f});

    pushLine(vertices[0], vertices[4], {1.0f, 1.0f, 1.0f});
    pushLine(vertices[1], vertices[5], {1.0f, 1.0f, 1.0f});
    pushLine(vertices[2], vertices[6], {1.0f, 1.0f, 1.0f});
    pushLine(vertices[3], vertices[7], {1.0f, 1.0f, 1.0f});
}

void PhysicsColliderRenderer::flush()
{
    mVertexBuffer->setBufferData(mBuildBuffer.data(), mBuildBuffer.size() * sizeof(ColliderVertex),
                                 sizeof(ColliderVertex));
}

common::Resource<sw::gfx::Shader> PhysicsColliderRenderer::getShader() const
{
    return mShader;
}

common::Resource<sw::gfx::Buffer> PhysicsColliderRenderer::getVertexBuffer() const
{
    return mVertexBuffer;
}

/* Class Protected Function Definition */

/* Class Private Function Definition */

void PhysicsColliderRenderer::pushLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& color)
{
    mBuildBuffer.emplace_back(ColliderVertex{p1, color});
    mBuildBuffer.emplace_back(ColliderVertex{p2, color});
}