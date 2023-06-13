
/* Include files */

#include "Animated.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <swizzle/core/Input.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

Animated::Animated(common::Resource<swizzle::gfx::GfxDevice> dev, common::Resource<swizzle::asset2::IMeshAsset> asset,
                   common::Resource<swizzle::gfx::Buffer> inst, common::Resource<swizzle::gfx::Texture> texture,
                   common::Resource<swizzle::gfx::Shader> shader)
    : mAsset(asset)
    , mMesh(nullptr)
    , mIndex(nullptr)
    , mBone(nullptr)
    , mInst(inst)
    , mTexture(texture)
    , mMaterial(nullptr)
    , mShader(shader)
    , mAnimDelta(0.0f)
    , mCurAnim(0u)
    , mFrameIndex(0u)
{
    mMesh = dev->createBuffer(swizzle::gfx::BufferType::Vertex);
    mIndex = dev->createBuffer(swizzle::gfx::BufferType::Index);
    mBone = dev->createBuffer(swizzle::gfx::BufferType::UniformBuffer);

    mMesh->setBufferData((U8*)mAsset->getVertexDataPtr(), mAsset->getVertexDataSize(),
                         sizeof(float) * (3u + 3u + 2u + 4u + 4u));
    mIndex->setBufferData((U8*)mAsset->getIndexDataPtr(), mAsset->getIndexDataSize(), sizeof(U32) * 3u);

    mBone->setBufferData((U8*)mAsset->getAnimationDataPtr(0, 0), mAsset->getNumberOfBones() * sizeof(glm::mat4),
                         sizeof(glm::mat4));

    mMaterial = dev->createMaterial(mShader, swizzle::gfx::SamplerMode::SamplerModeClamp);
    mMaterial->setDescriptorTextureResource(0u, mTexture);
    mMaterial->setDescriptorBufferResource(1u, mBone, ~0ull);
}

void Animated::update(DeltaTime dt, common::Unique<swizzle::gfx::CommandTransaction>& trans)
{
    const DeltaTime animFps = 1.0f / 24.0f;

    SwBool wasChanged = false;

    UNUSED_ARG(dt);
    trans->uploadTexture(mTexture);

    if (mAsset && mAsset->hasAnimations())
    {
        if (swizzle::input::WasKeyPressedThisFrame(swizzle::input::Keys::Key8))
        {
            mCurAnim++;
            mCurAnim = mCurAnim % mAsset->getNumberOfAnimations();
            mFrameIndex = 0u;
            mAnimDelta = 0.0f;
            wasChanged = true;
        }

        if (swizzle::input::WasKeyPressedThisFrame(swizzle::input::Keys::Key9))
        {
            mCurAnim--;
            mCurAnim = mCurAnim % mAsset->getNumberOfAnimations();
            mFrameIndex = 0u;
            mAnimDelta = 0.0f;
            wasChanged = true;
        }

        if (mAnimDelta > animFps || wasChanged)
        {
            mAnimDelta -= animFps;
            mFrameIndex++;
            mFrameIndex = mFrameIndex % mAsset->getNumberOfKeyFrames(mCurAnim);

            mBone->setBufferData((U8*)mAsset->getAnimationDataPtr(mCurAnim, mFrameIndex),
                                 mAsset->getNumberOfBones() * sizeof(glm::mat4), sizeof(glm::mat4));
            mMaterial->setDescriptorBufferResource(1u, mBone, ~0ull);
        }
        else
        {
            if (mAsset->getNumberOfKeyFrames(mCurAnim) > 1)
            {
                mAnimDelta += dt;
            }
        }
    }
}

void Animated::render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam)
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
    t.eye = glm::vec4(cam.getPosition(), 1.0F);

    trans->bindShader(mShader);

    trans->bindMaterial(mShader, mMaterial);
    trans->setViewport(U32(x), U32(y));

    trans->setShaderConstant(mShader, (U8*)&t, sizeof(t));

    // cmd->drawIndexed(mMesh, mIndex);
    trans->drawIndexedInstanced(mMesh, mIndex, mInst);
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
