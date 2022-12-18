
/* Include files */

#include "AssetManager.hpp"

#include <swizzle/asset/TextureLoader.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

AssetManager::AssetManager(common::Resource<swizzle::gfx::GfxContext> context)
    : mContext(context)
    , mShaders()
{
}

AssetManager::~AssetManager()
{
    mShaders.clear();
}

bool AssetManager::loadShader(common::Resource<swizzle::gfx::Shader> shader, const SwChar* mFilePath)
{
    ShaderInfo info{};
    info.mFilePath = mFilePath;
    info.mShader = shader;

    bool ret = shader->load(mFilePath);

    mShaders.push_back(info);
    return ret;
}

common::Resource<swizzle::gfx::Texture> AssetManager::loadTexture(const SwChar* filePath)
{
    if (mTextures.count(filePath) == 0)
    {
        TextureInfo t{};
        t.mTexture = swizzle::asset::LoadTexture2D(mContext, filePath);
        mTextures.insert({filePath, t});
    }

    return mTextures[filePath].mTexture;
}

common::Resource<MeshInfo> AssetManager::loadMesh(const SwChar* filePath, SwBool animated)
{
    if (animated)
    {
        if (mAnimatedMeshes.count(filePath) == 0)
        {
            MeshInfo mi{};

            swizzle::asset2::MeshAssetLoaderDescription ldi = {};
            ldi.mLoadPossitions = {
                {swizzle::asset2::AttributeTypes::VertexPosition, 0u},
                {swizzle::asset2::AttributeTypes::NormalVector, sizeof(float) * 3u},
                {swizzle::asset2::AttributeTypes::UvCoordinates, sizeof(float) * 6u},
                {swizzle::asset2::AttributeTypes::BoneIndices, sizeof(float) * 8u},
                {swizzle::asset2::AttributeTypes::BoneWeights, sizeof(float) * 12u},
            };

            mi.mMesh = swizzle::asset2::LoadMesh(filePath, ldi);
            mi.mVertex = mContext->createBuffer(swizzle::gfx::BufferType::Vertex);
            mi.mIndex = mContext->createBuffer(swizzle::gfx::BufferType::Index);

            U32 vStride = (U32)sizeof(float) * (3u + 3u + 2u + 4u + 4u);
            U32 iStride = (U32)sizeof(U32) * 3u;

            mi.mVertex->setBufferData((U8*)mi.mMesh->getVertexDataPtr(), mi.mMesh->getVertexDataSize(), vStride);
            mi.mIndex->setBufferData((U8*)mi.mMesh->getIndexDataPtr(), mi.mMesh->getIndexDataSize(), iStride);

            mAnimatedMeshes.insert({filePath, common::CreateRef<MeshInfo>(mi)});
        }

        return mAnimatedMeshes[filePath];
    }
    else
    {
        if (mMeshes.count(filePath) == 0)
        {
            MeshInfo mi{};
            swizzle::asset2::MeshAssetLoaderDescription ldi = {};
            ldi.mLoadPossitions = {{swizzle::asset2::AttributeTypes::VertexPosition, 0u},
                                   {swizzle::asset2::AttributeTypes::NormalVector, sizeof(float) * 3u},
                                   {swizzle::asset2::AttributeTypes::UvCoordinates, sizeof(float) * 6u}};

            mi.mMesh = swizzle::asset2::LoadMesh(filePath, ldi);
            mi.mVertex = mContext->createBuffer(swizzle::gfx::BufferType::Vertex);
            mi.mIndex = mContext->createBuffer(swizzle::gfx::BufferType::Index);

            U32 vStride = (U32)sizeof(float) * (3u + 3u + 2u);
            U32 iStride = (U32)sizeof(U32) * 3u;

            mi.mVertex->setBufferData((U8*)mi.mMesh->getVertexDataPtr(), mi.mMesh->getVertexDataSize(), vStride);
            mi.mIndex->setBufferData((U8*)mi.mMesh->getIndexDataPtr(), mi.mMesh->getIndexDataSize(), iStride);
            mMeshes.insert({filePath, common::CreateRef<MeshInfo>(mi)});
        }

        return mMeshes[filePath];
    }
}

size_t AssetManager::getShaderCount() const
{
    return mShaders.size();
}

ShaderInfo AssetManager::getShaderInfo(size_t index)
{
    return mShaders[index];
}

SwBool AssetManager::reloadShader(size_t index)
{
    return mShaders[index].mShader->load(mShaders[index].mFilePath);
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
