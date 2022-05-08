
/* Include files */

#include "AssetManager.hpp"

#include <swizzle/asset/MeshLoader.hpp>
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
{

}

AssetManager::~AssetManager()
{

}

void AssetManager::clear()
{
    mMeshAssets.clear();
}

SwBool AssetManager::loadMesh(U32 id, const SwChar* filePath)
{
    SwBool loadedOk = false;

    auto mesh = swizzle::asset::LoadMesh(mContext, filePath, true);
    if (mesh.mVertexBuffer && mesh.mIndexBuffer)
    {
        mMeshAssets[id] = mesh;
        loadedOk = true;
    }

    return loadedOk;
}

swizzle::Mesh AssetManager::loadMesh2(const SwChar* filePath)
{
    return swizzle::asset::LoadMesh(mContext, filePath, true);
}

swizzle::MeshAnimated AssetManager::loadAnimMesh(const SwChar* filePath)
{
    return swizzle::asset::LoadMeshAnimated(mContext, filePath, true);
}

common::Resource<swizzle::gfx::Texture> AssetManager::loadTexture(const SwChar* filePath)
{
    return swizzle::asset::LoadTexture2D(mContext, filePath);
}

common::Resource<swizzle::gfx::Texture> AssetManager::loadCubeTexture(const SwChar* right, const SwChar* left, const SwChar* top, const SwChar* bottom, const SwChar* front, const SwChar* back)
{
    return swizzle::asset::LoadTextureCubeMap(mContext, right, left, top, bottom, front, back);
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
