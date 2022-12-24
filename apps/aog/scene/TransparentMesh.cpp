
/* Include files */

#include "TransparentMesh.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../AssetManager.hpp"
#include <swizzle/core/Input.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

TransparentMesh::TransparentMesh(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<MeshInfo> meshInfo,
                                 common::Resource<swizzle::gfx::Buffer> inst,
                                 common::Resource<swizzle::gfx::Shader> shader,
                                 common::Resource<swizzle::gfx::Shader> shadow, glm::vec3 pos, glm::vec4 col)
    : mMeshInfo(meshInfo)
    , mShader(shader)
    , mMaterial(nullptr)
    , mUniform(nullptr)
    , mInst(inst)
    , mShadowShader(shadow)
    , mPos(pos)
    , mCol(col)
{
    mUniform = ctx->createBuffer(swizzle::gfx::BufferType::UniformBuffer);
    mMaterial = ctx->createMaterial(mShader, swizzle::gfx::SamplerMode::SamplerModeClamp);
}

void TransparentMesh::update(DeltaTime dt, SceneRenderSettings& settings,
                             common::Unique<swizzle::gfx::CommandTransaction>& trans)
{
    UNUSED_ARG(dt);
    UNUSED_ARG(settings);
    UNUSED_ARG(trans);
}

void TransparentMesh::render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, OrthoCamera& cam)
{
    UNUSED_ARG(trans);
    UNUSED_ARG(cam);
    // glm::mat4 camMat = cam.getProjection() * cam.getView();
    // trans->bindShader(mShadowShader);
    // trans->setViewport(2048, 2048);
    // trans->setShaderConstant(mShadowShader, (U8*)&camMat, sizeof(glm::mat4));
    // trans->drawIndexedInstanced(mMeshInfo->mVertex, mMeshInfo->mIndex, mInst);
}

void TransparentMesh::render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam)
{
    struct tmp
    {
        glm::mat4 viewProj;
        glm::vec4 color;
    };

    float x, y;

    cam.getCameraSize(x, y);

    tmp t = {};
    t.viewProj = cam.getProjection() * cam.getView();
    t.color = mCol;

    ViewProjection c{};
    c.proj = cam.getProjection();
    c.view = cam.getView();

    mUniform->setBufferData(&c, sizeof(ViewProjection), sizeof(ViewProjection));
    mMaterial->setDescriptorBufferResource(0u, mUniform, ~0ull);

    trans->bindShader(mShader);

    trans->bindMaterial(mShader, mMaterial);
    trans->setViewport(U32(x), U32(y));
    trans->enableStencilTest(false);

    trans->setShaderConstant(mShader, (U8*)&t, sizeof(t));

    trans->drawIndexedInstanced(mMeshInfo->mVertex, mMeshInfo->mIndex, mInst);
}

void TransparentMesh::renderTransform(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans,
                                            PerspectiveCamera& cam, glm::mat4& mat)
{
    struct tmp
    {
        glm::mat4 viewProj;
        glm::vec4 color;
    };

    float x, y;

    cam.getCameraSize(x, y);

    tmp t = {};
    t.viewProj = cam.getProjection() * cam.getView() * mat;
    t.color = mCol;

    ViewProjection c{};
    c.proj = cam.getProjection();
    c.view = cam.getView();

    mUniform->setBufferData(&c, sizeof(ViewProjection), sizeof(ViewProjection));
    mMaterial->setDescriptorBufferResource(0u, mUniform, ~0ull);

    trans->bindShader(mShader);

    trans->bindMaterial(mShader, mMaterial);
    trans->setViewport(U32(x), U32(y));
    trans->enableStencilTest(true);

    trans->setShaderConstant(mShader, (U8*)&t, sizeof(t));

    trans->drawIndexedInstanced(mMeshInfo->mVertex, mMeshInfo->mIndex, mInst);
    trans->enableStencilTest(false);
}

glm::vec3 TransparentMesh::getPos() const
{
    return mPos;
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
