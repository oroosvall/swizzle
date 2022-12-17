
/* Include files */

#include "RegularMesh.hpp"

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

RegularMesh::RegularMesh(common::Resource<swizzle::gfx::GfxContext> ctx,
                         common::Resource<swizzle::asset2::IMeshAsset> asset,
                         common::Resource<swizzle::gfx::Buffer> inst, common::Resource<swizzle::gfx::Texture> texture,
                         common::Resource<swizzle::gfx::Texture> optionalTexture,
                         common::Resource<swizzle::gfx::Shader> shader,
                         common::Resource<swizzle::gfx::Shader> optionalShader)
    : mAsset(asset)
    , mMesh(nullptr)
    , mTexture(texture)
    , mOptionalTexture(optionalTexture)
    , mMaterial(nullptr)
    , mShader(shader)
    , mOptionalShader(optionalShader)
    , mInst(inst)
{
    mMesh = ctx->createBuffer(swizzle::gfx::BufferType::Vertex);
    mIndex = ctx->createBuffer(swizzle::gfx::BufferType::Index);
    mUniform = ctx->createBuffer(swizzle::gfx::BufferType::UniformBuffer);

    mMesh->setBufferData((U8*)mAsset->getVertexDataPtr(), mAsset->getVertexDataSize(), sizeof(float) * (3u + 3u + 2u));
    mIndex->setBufferData((U8*)mAsset->getIndexDataPtr(), mAsset->getIndexDataSize(), sizeof(U32) * 3u);

    mMaterial = ctx->createMaterial(mShader, swizzle::gfx::SamplerMode::SamplerModeClamp);
    mMaterial->setDescriptorTextureResource(1u, mTexture);
}

void RegularMesh::update(DeltaTime dt, SceneRenderSettings& settings,
                         common::Unique<swizzle::gfx::CommandTransaction>& trans)
{
    UNUSED_ARG(dt);
    trans->uploadTexture(mTexture);
    trans->uploadTexture(mOptionalTexture);

    if (mNormalEnabled != settings.mEnableNormalMaps)
    {
        mNormalEnabled = settings.mEnableNormalMaps;
        if (mNormalEnabled)
        {
            mMaterial->setDescriptorTextureResource(1u, mTexture);
        }
        else
        {
            mMaterial->setDescriptorTextureResource(1u, mOptionalTexture);
        }
    }

    if (settings.mTesselation)
    {
        mSelectedShader = mShader;
    }
    else
    {
        mSelectedShader = mOptionalShader;
    }
}

void RegularMesh::render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam)
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

    ViewProjection c{};
    c.proj = cam.getProjection();
    c.view = cam.getView();

    mUniform->setBufferData(&c, sizeof(ViewProjection), sizeof(ViewProjection));
    mMaterial->setDescriptorBufferResource(0u, mUniform, ~0ull);

    trans->bindShader(mSelectedShader);

    trans->bindMaterial(mSelectedShader, mMaterial);
    trans->setViewport(U32(x), U32(y));

    trans->setShaderConstant(mSelectedShader, (U8*)&t, sizeof(t));

    trans->drawIndexedInstanced(mMesh, mIndex, mInst);
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
