
/* Include files */

#include "Portal.hpp"

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

Portal::Portal(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<MeshInfo> frame,
               common::Resource<MeshInfo> plane, common::Resource<swizzle::gfx::Shader> frameShader,
               common::Resource<swizzle::gfx::Shader> planeShader, common::Resource<swizzle::gfx::Shader> shadow,
               common::Resource<swizzle::gfx::Shader> clearDepth, common::Resource<swizzle::gfx::Buffer> inst,
               glm::mat4 trans)
    : mFrame(frame)
    , mPlane(plane)
    , mFrameShader(frameShader)
    , mPlaneShader(planeShader)
    , mShadowShader(shadow)
    , mClearDepth(clearDepth)
    , mInstance(inst)
    , mTransform(trans)
{
}

void Portal::update(DeltaTime dt, SceneRenderSettings& settings,
                    common::Unique<swizzle::gfx::CommandTransaction>& trans)
{
    UNUSED_ARG(dt);
    UNUSED_ARG(settings);
    UNUSED_ARG(trans);
}

void Portal::render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, OrthoCamera& cam)
{
    glm::mat4 camMat = cam.getProjection() * cam.getView();
    trans->bindShader(mShadowShader);
    trans->setViewport(2048, 2048);
    trans->setShaderConstant(mShadowShader, (U8*)&camMat, sizeof(glm::mat4));
    trans->drawIndexedInstanced(mFrame->mVertex, mFrame->mIndex, mInstance);
}

void Portal::render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam)
{
    struct tmp
    {
        glm::mat4 viewProj;
        glm::vec4 eye;
    };

    float x, y;

    cam.getCameraSize(x, y);

    tmp t = {};
    t.viewProj = cam.getProjection() * cam.getView();
    t.eye = glm::vec4(cam.getPosition(), 1.0);

    // ViewProjection c{};
    // c.proj = cam.getProjection();
    // c.view = cam.getView();

    // mUniform->setBufferData(&c, sizeof(ViewProjection), sizeof(ViewProjection));
    // mMaterial->setDescriptorBufferResource(0u, mUniform, ~0ull);

    trans->bindShader(mFrameShader);

    // trans->bindMaterial(mFrameShader, mMaterial);
    trans->setViewport(U32(x), U32(y));
    trans->enableStencilTest(false);

    trans->setShaderConstant(mFrameShader, (U8*)&t, sizeof(t));

    trans->drawIndexedInstanced(mFrame->mVertex, mFrame->mIndex, mInstance);
}

void Portal::renderTransform(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam,
                                   glm::mat4& mat)
{
    UNUSED_ARG(trans);
    UNUSED_ARG(cam);
    UNUSED_ARG(mat);
}

void Portal::renderPlane(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam)
{
    struct tmp
    {
        glm::mat4 viewProj;
        glm::vec4 eye;
    };

    float x, y;

    cam.getCameraSize(x, y);

    tmp t = {};
    t.viewProj = cam.getProjection() * cam.getView();
    t.eye = glm::vec4(cam.getPosition(), 1.0);

    trans->bindShader(mPlaneShader);

    trans->setViewport(U32(x), U32(y));
    trans->enableStencilTest(true);

    trans->setShaderConstant(mPlaneShader, (U8*)&t, sizeof(t));

    trans->drawIndexedInstanced(mPlane->mVertex, mPlane->mIndex, mInstance);

    trans->bindShader(mClearDepth);
    trans->setShaderConstant(mClearDepth, (U8*)&t, sizeof(t));

    trans->drawIndexedInstanced(mPlane->mVertex, mPlane->mIndex, mInstance);
}

glm::mat4 Portal::getMirrorTransform() const
{
    return mTransform;
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
