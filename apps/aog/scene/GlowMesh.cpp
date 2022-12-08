
/* Include files */

#include "GlowMesh.hpp"

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

GlowMesh::GlowMesh(common::Resource<swizzle::gfx::GfxContext> ctx,
                         common::Resource<swizzle::asset2::IMeshAsset> asset,
                         common::Resource<swizzle::gfx::Buffer> inst, common::Resource<swizzle::gfx::Texture> texture,
                         common::Resource<swizzle::gfx::Texture> glowTexture,
                         common::Resource<swizzle::gfx::Shader> shader)
    : mAsset(asset)
    , mMesh(nullptr)
    , mTexture(texture)
    , mGlowTexture(glowTexture)
    , mMaterial(nullptr)
    , mShader(shader)
    , mInst(inst)
{
    mMesh = ctx->createBuffer(swizzle::gfx::BufferType::Vertex);
    mIndex = ctx->createBuffer(swizzle::gfx::BufferType::Index);

    mMesh->setBufferData((U8*)mAsset->getVertexDataPtr(), mAsset->getVertexDataSize(), sizeof(float) * (3u + 3u + 2u));
    mIndex->setBufferData((U8*)mAsset->getIndexDataPtr(), mAsset->getIndexDataSize(), sizeof(U32) * 3u);

    mMaterial = ctx->createMaterial(mShader, swizzle::gfx::SamplerMode::SamplerModeClamp);
    mMaterial->setDescriptorTextureResource(0u, mTexture);
    mMaterial->setDescriptorTextureResource(1u, mGlowTexture);
}

void GlowMesh::update(DeltaTime dt, SceneRenderSettings& settings,
                         common::Unique<swizzle::gfx::CommandTransaction>& trans)
{
    UNUSED_ARG(dt);
    UNUSED_ARG(settings);
    trans->uploadTexture(mTexture);
    trans->uploadTexture(mGlowTexture);

}

void GlowMesh::render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam)
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

    trans->drawIndexedInstanced(mMesh, mIndex, mInst);
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
