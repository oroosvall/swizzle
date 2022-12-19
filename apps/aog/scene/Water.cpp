
/* Include files */

#include "Water.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../AssetManager.hpp"
#include <swizzle/core/Input.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

struct WaterVertex
{
    F32 x, y, z;
    F32 u, v;
};

struct WaterIndex
{
    U32 v1, v2, v3;
};

struct ComputePushs
{
    float dir;
    int x;
    int y;
    int r;
};

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

Water::Water(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<swizzle::gfx::Buffer> inst,
             common::Resource<swizzle::gfx::Shader> shader, common::Resource<swizzle::gfx::Shader> compute)
    : mVertexBuffer()
    , mIndexBuffer()
    , mShader(shader)
    , mCompute(compute)
    , mMaterial()
    , mMatCompute()
    , mInst(inst)
    , mUniform()
    , mActiveTexture(false)
{
    mVertexBuffer = ctx->createBuffer(swizzle::gfx::BufferType::Vertex);
    mIndexBuffer = ctx->createBuffer(swizzle::gfx::BufferType::Index);
    mUniform = ctx->createBuffer(swizzle::gfx::BufferType::UniformBuffer);

    mMaterial = ctx->createMaterial(mShader, swizzle::gfx::SamplerMode::SamplerModeClamp);
    mMatCompute = ctx->createMaterial(mCompute, swizzle::gfx::SamplerMode::SamplerModeClamp);

    mHeight1 = ctx->createTexture(250u, 250u, 1, true);
    mHeight2 = ctx->createTexture(250u, 250u, 1, true);

    mMaterial->setDescriptorTextureResource(1u, mHeight1);
    mMaterial->setDescriptorTextureResource(2u, mHeight2);

    mMatCompute->setDescriptorComputeImageResource(0u, mHeight1);
    mMatCompute->setDescriptorComputeImageResource(1u, mHeight2);

    std::vector<F32> data;
    data.resize(250u * 250u);

    mHeight1->setData(250u, 250u, 1, (const U8*)data.data());
    mHeight2->setData(250u, 250u, 1, (const U8*)data.data());

    generatePlane();
}

void Water::update(DeltaTime dt, SceneRenderSettings& settings, common::Unique<swizzle::gfx::CommandTransaction>& trans)
{
    UNUSED_ARG(dt);
    UNUSED_ARG(settings);

    trans->uploadTexture(mHeight1);
    trans->uploadTexture(mHeight2);

    mTime += dt;

    ComputePushs pushs{};
    pushs.dir = mActiveTexture ? 1.0f : 0.0f;
    pushs.x = 125;
    pushs.y = 125;
    pushs.r = 0;

    if (mTime > 1.0f)
    {
        mTime -= 1.0f;
        pushs.r = 2;
    }

    //if (mTime > 0.2f)
    {
        trans->changeImageLayoutCompute(mHeight1);
        trans->changeImageLayoutCompute(mHeight2);
        trans->bindComputeShader(mCompute, mMatCompute, (U8*)&pushs, sizeof(ComputePushs));
        trans->dispatchCompute(500u - 2u, 500u - 2u, 1u);
        trans->changeImageLayoutRender(mHeight1);
        trans->changeImageLayoutRender(mHeight2);

        mActiveTexture = !mActiveTexture;
        //mTime -= 0.2f;
    }
}

void Water::render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam)
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
    t.eye = glm::vec4(cam.getPosition(), mActiveTexture ? 1.0 : 0.0);

    ViewProjection c{};
    c.proj = cam.getProjection();
    c.view = cam.getView();

    mUniform->setBufferData(&c, sizeof(ViewProjection), sizeof(ViewProjection));
    mMaterial->setDescriptorBufferResource(0u, mUniform, ~0ull);

    trans->bindShader(mShader);

    trans->bindMaterial(mShader, mMaterial);
    trans->setViewport(U32(x), U32(y));

    trans->setShaderConstant(mShader, (U8*)&t, sizeof(t));

    trans->drawIndexedInstanced(mVertexBuffer, mIndexBuffer, mInst);
}

/* Class Protected Function Definition */

/* Class Private Function Definition */

void Water::generatePlane()
{
    std::vector<WaterVertex> verts;



    for (F32 x = -25.0f; x < 25.0f; x += 0.1f)
    {
        for (F32 y = -25.0f; y < 25.0f; y += 0.1f)
        {
            F32 ux = (x + 25.0f) / 50.0f;
            F32 uy = (y + 25.0f) / 50.0f;
            WaterVertex vert{};
            vert.x = x;
            vert.z = y;
            //vert.ny = 1.0f;
            vert.u = ux;
            vert.v = uy;
            verts.push_back(vert);
        }
    }

    std::vector<WaterIndex> tris;

    for (U32 y = 0; y < 499; ++y)
    {
        for (U32 x = 0; x < 499; ++x)
        {
            U32 currRow = (y * 500) + x;
            U32 nextRow = ((y + 1) * 500) + x;

            WaterIndex t1{ currRow + 1, nextRow + 1, currRow };
            WaterIndex t2{ currRow, nextRow + 1, nextRow };
            tris.push_back(t1);
            tris.push_back(t2);
        }
    }

    mVertexBuffer->setBufferData((U8*)verts.data(), verts.size() * sizeof(WaterVertex), sizeof(WaterVertex));
    mIndexBuffer->setBufferData((U8*)tris.data(), tris.size() * sizeof(WaterIndex), sizeof(WaterIndex));
}