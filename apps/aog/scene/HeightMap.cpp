
/* Include files */

#include "HeightMap.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <swizzle/core/Input.hpp>

#include <stb/stb_image.h>

/* Defines */

/* Typedefs */

/* Structs/Classes */

struct HeightMapVertex
{
    F32 x, y, z;
    F32 nx, ny, nz;
    F32 u, v;
};

struct HeightMapTriangle
{
    U32 v1, v2, v3;
};

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

HeightMap::HeightMap(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<swizzle::gfx::Buffer> inst,
                     common::Resource<swizzle::gfx::Texture> texture, common::Resource<swizzle::gfx::Shader> shader)
    : mMesh(nullptr)
    , mTexture(texture)
    , mMaterial(nullptr)
    , mShader(shader)
    , mInst(inst)
{
    mMesh = ctx->createBuffer(swizzle::gfx::BufferType::Vertex);
    mIndex = ctx->createBuffer(swizzle::gfx::BufferType::Index);

    S32 ix = 0;
    S32 iy = 0;
    S32 ic = 0;

    U8* image = stbi_load("AoG/textures/heightmap.png", &ix, &iy, &ic, 4);

    std::vector<HeightMapVertex> verts;

    for (F32 x = -25.0f; x < 25.0f; x += 1.0f)
    {
        for (F32 y = -25.0f; y < 25.0f; y += 1.0f)
        {
            F32 ux = (x + 25.0f) / 50.0f;
            F32 uy = (y + 25.0f) / 50.0f;
            HeightMapVertex vert{};
            vert.x = x;
            vert.y = getHeight(image, ix, iy, ic, ux, uy, 10.0f);
            vert.z = y;
            vert.ny = 1.0f;
            vert.u = ux;
            vert.v = uy;
            verts.push_back(vert);
        }
    }

    stbi_image_free(image);

    std::vector<HeightMapTriangle> tris;

    for (U32 y = 0; y < 49; ++y)
    {
        for (U32 x = 0; x < 49; ++x)
        {
            U32 currRow = (y * 50) + x;
            U32 nextRow = ((y + 1) * 50) + x;

            HeightMapTriangle t1{currRow, nextRow + 1, currRow + 1};
            HeightMapTriangle t2{currRow, nextRow, nextRow + 1};
            tris.push_back(t1);
            tris.push_back(t2);
        }
    }

    mMesh->setBufferData((U8*)verts.data(), verts.size() * sizeof(HeightMapVertex), sizeof(HeightMapVertex));
    mIndex->setBufferData((U8*)tris.data(), tris.size() * sizeof(HeightMapTriangle), sizeof(HeightMapTriangle));

    mMaterial = ctx->createMaterial(mShader, swizzle::gfx::SamplerMode::SamplerModeClamp);
    mMaterial->setDescriptorTextureResource(0u, mTexture);
}

void HeightMap::update(DeltaTime dt, common::Unique<swizzle::gfx::CommandTransaction>& trans)
{
    UNUSED_ARG(dt);
    trans->uploadTexture(mTexture);
}

void HeightMap::render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam)
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

F32 HeightMap::getHeight(U8* image, S32 x, S32 y, S32 ch, F32 xp, F32 yp, F32 max)
{
    F32 value = 0.0f;
    if (image)
    {
        U32 xCoord = U32(F32(x) * xp);
        U32 yCoord = U32(F32(y) * yp);

        U32 idx = ((yCoord * x) + xCoord) * ch;

        U8 val = image[idx];
        F32 conv = F32(val) / 255.0f;

        value = conv * max;
    }
    return value;
}