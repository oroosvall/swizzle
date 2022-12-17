
/* Include files */

#include "AnimatedTextureMesh.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <swizzle/core/Input.hpp>

#include "../AssetManager.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

AnimatedTextureMesh::AnimatedTextureMesh(common::Resource<swizzle::gfx::GfxContext> ctx,
                                         common::Resource<MeshInfo> meshInfo,
                                         common::Resource<swizzle::gfx::Buffer> inst,
                                         common::Resource<swizzle::gfx::Texture> texture,
                                         common::Resource<swizzle::gfx::Shader> shader)
    : mMeshInfo(meshInfo)
    , mTexture(texture)
    , mMaterial(nullptr)
    , mShader(shader)
    , mInst(inst)
    , mUvOffset()
{
    mUniform = ctx->createBuffer(swizzle::gfx::BufferType::UniformBuffer);

    mMaterial = ctx->createMaterial(mShader, swizzle::gfx::SamplerMode::SamplerModeRepeat);
    mMaterial->setDescriptorTextureResource(1u, mTexture);
}

void AnimatedTextureMesh::update(DeltaTime dt, SceneRenderSettings& settings, common::Unique<swizzle::gfx::CommandTransaction>& trans)
{
    if (!settings.mEnableAnimatedTextures)
    {
        dt = 0.0f;
    }

    trans->uploadTexture(mTexture);
    mUvOffset.y += 0.4f * dt;
    if (mUvOffset.y > 1.0f)
    {
        mUvOffset.y -= 1.0f;
    }
}

void AnimatedTextureMesh::render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam)
{
    struct tmp
    {
        glm::mat4 viewProj;
        glm::vec4 eye;
        glm::vec2 uvOffset;
    };

    float x, y;

    cam.getCameraSize(x, y);

    tmp t = {};
    t.viewProj = cam.getProjection() * cam.getView();
    t.eye = glm::vec4(cam.getPosition(), 1.0F);
    t.uvOffset = mUvOffset;

    ViewProjection c{};
    c.proj = cam.getProjection();
    c.view = cam.getView();

    mUniform->setBufferData(&c, sizeof(ViewProjection), sizeof(ViewProjection));
    mMaterial->setDescriptorBufferResource(0u, mUniform, ~0ull);

    trans->bindShader(mShader);

    trans->bindMaterial(mShader, mMaterial);
    trans->setViewport(U32(x), U32(y));

    trans->setShaderConstant(mShader, (U8*)&t, sizeof(t));

    trans->drawIndexedInstanced(mMeshInfo->mVertex, mMeshInfo->mIndex, mInst);
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
