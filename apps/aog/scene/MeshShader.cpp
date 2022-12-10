
/* Include files */

#include "MeshShader.hpp"

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

MeshShader::MeshShader(common::Resource<swizzle::gfx::GfxContext> ctx,
                       common::Resource<swizzle::asset2::IMeshAsset> asset, common::Resource<swizzle::gfx::Buffer> inst,
                       common::Resource<swizzle::gfx::Shader> shader, common::Resource<swizzle::gfx::Shader> normalShader)
    : mAsset(asset)
    , mVertexData(nullptr)
    , mVertexIndex(nullptr)
    , mPrimitiveIndex(nullptr)
    , mMeshlets(nullptr)
    , mShader(shader)
    , mMaterial(nullptr)
    , mInst(inst)
    , mNumMeshlets(0u)
    , mNormalShader(normalShader)
    , mHasMeshShader(false)
{
    if (ctx->hasMeshShaderSupport())
    {
        generateMeshlets(ctx);
        mHasMeshShader = true;
    }

    mNormalVertex = ctx->createBuffer(swizzle::gfx::BufferType::Vertex);
    mNormalIndex = ctx->createBuffer(swizzle::gfx::BufferType::Index);

    mNormalVertex->setBufferData((U8*)mAsset->getVertexDataPtr(), mAsset->getVertexDataSize(), sizeof(float) * (3u + 3u + 2u));
    mNormalIndex->setBufferData((U8*)mAsset->getIndexDataPtr(), mAsset->getIndexDataSize(), sizeof(U32) * 3u);

    mNormalMaterial = ctx->createMaterial(mNormalShader, swizzle::gfx::SamplerMode::SamplerModeClamp);

}

void MeshShader::update(DeltaTime dt, SceneRenderSettings& settings,
                        common::Unique<swizzle::gfx::CommandTransaction>& trans)
{
    UNUSED_ARG(dt);
    UNUSED_ARG(trans);

    mMeshShaderEnabled = settings.mMeshShaders;
}

void MeshShader::render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam)
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

    if (mHasMeshShader && mMeshShaderEnabled)
    {
        trans->bindShader(mShader);
        trans->bindMaterial(mShader, mMaterial);

        trans->setViewport(U32(x), U32(y));

        trans->setShaderConstant(mShader, (U8*)&t, sizeof(t));

        trans->drawMeshlet(mNumMeshlets);
    }
    else
    {
        trans->bindShader(mNormalShader);
        trans->bindMaterial(mNormalShader, mNormalMaterial);

        trans->setViewport(U32(x), U32(y));
        trans->setShaderConstant(mNormalShader, (U8*)&t, sizeof(t));

        trans->drawIndexedInstanced(mNormalVertex, mNormalIndex, mInst);
    }

    //trans->drawIndexedInstanced(mMesh, mIndex, mInst);
}

/* Class Protected Function Definition */

/* Class Private Function Definition */

void MeshShader::generateMeshlets(common::Resource<swizzle::gfx::GfxContext> ctx)
{
    mVertexData = ctx->createBuffer(swizzle::gfx::BufferType::StorageBuffer);
    mVertexIndex = ctx->createBuffer(swizzle::gfx::BufferType::StorageBuffer);
    mPrimitiveIndex = ctx->createBuffer(swizzle::gfx::BufferType::StorageBuffer);
    mMeshlets = ctx->createBuffer(swizzle::gfx::BufferType::StorageBuffer);

    //glm::vec3 verts[] =
    //{
    //    glm::vec3(-1.0, -1.0, 1.0),
    //    glm::vec3(1.0, -1.0, 1.0),
    //    glm::vec3(1.0, 1.0, 1.0),
    //    glm::vec3(-1.0, 1.0, 1.0),
    //};

    mVertexData->setBufferData((U8*)mAsset->getVertexDataPtr(), mAsset->getVertexDataSize(), sizeof(float) * (3u));
    //mVertexData->setBufferData(&verts, sizeof(verts), sizeof(float));

    struct Triangle
    {
        U32 t1, t2, t3;
    };

    const U32 maxVertexCount = 253u;
    const U32 maxPrimitiveCount = 128u;

    Triangle* originalTris = (Triangle*)mAsset->getIndexDataPtr();
    size_t triangleCount = mAsset->getIndexDataSize() / sizeof(Triangle);

    std::vector<MeshletDescription> meshlets;
    std::vector<U32> primitiveIndex;
    std::vector<U32> vertexIndex;

    MeshletDescription workingMeshlet = {};
    for (size_t i = 0u; i < triangleCount; i++)
    {
        Triangle& t = originalTris[i];
        createIndex(t.t1, vertexIndex, primitiveIndex, workingMeshlet);
        createIndex(t.t2, vertexIndex, primitiveIndex, workingMeshlet);
        createIndex(t.t3, vertexIndex, primitiveIndex, workingMeshlet);

        workingMeshlet.mPrimitiveCount++;

        if (workingMeshlet.mPrimitiveCount >= maxPrimitiveCount || workingMeshlet.mVertexCount >= maxVertexCount)
        {
            meshlets.push_back(workingMeshlet);
            workingMeshlet = {};
            workingMeshlet.mPrimitiveBegin = (U32)primitiveIndex.size();
            workingMeshlet.mVertexBegin = (U32)vertexIndex.size();
        }
    }

    if (workingMeshlet.mPrimitiveCount != 0 && workingMeshlet.mVertexCount != 0)
    {
        meshlets.push_back(workingMeshlet);
    }

    mVertexIndex->setBufferData(vertexIndex.data(), vertexIndex.size() * sizeof(U32), sizeof(U32));
    mPrimitiveIndex->setBufferData(primitiveIndex.data(), primitiveIndex.size() * sizeof(U32), sizeof(U32));
    mMeshlets->setBufferData(meshlets.data(), meshlets.size() * sizeof(MeshletDescription), sizeof(MeshletDescription));

    mMaterial = ctx->createMaterial(mShader, swizzle::gfx::SamplerMode::SamplerModeClamp);

    mMaterial->setDescriptorBufferResource(0u, mMeshlets, ~0ull);
    mMaterial->setDescriptorBufferResource(1u, mVertexData, ~0ull);
    mMaterial->setDescriptorBufferResource(2u, mVertexIndex, ~0ull);
    mMaterial->setDescriptorBufferResource(3u, mPrimitiveIndex, ~0ull);

    mNumMeshlets = (U32)meshlets.size();
}

void MeshShader::createIndex(U32 index, std::vector<U32>& vertexList, std::vector<U32>& primitive, MeshletDescription& meshletDescr)
{
    U32 outIndex = 0u;
    if (isInList(index, vertexList, meshletDescr.mVertexBegin, outIndex))
    {
        primitive.push_back((U8)outIndex);
    }
    else
    {
        size_t idx = vertexList.size() - (size_t)meshletDescr.mVertexBegin;
        primitive.push_back(U8(idx));
        vertexList.push_back(index);
        meshletDescr.mVertexCount++;
    }
}

SwBool MeshShader::isInList(U32 index, std::vector<U32>& vertexList, U32 begin, U32& outIndex)
{
    for (size_t i = begin; i < vertexList.size(); i++)
    {
        if (vertexList[i] == index)
        {
            outIndex = U32(i) - begin;
            return true;
        }
    }
    return false;
}