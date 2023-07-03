
/* Include files */

#include "AssetManager.hpp"

#include <swizzle/asset2/Assets.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

AssetManager::AssetManager(common::Resource<swizzle::gfx::GfxDevice> device)
    : mDevice(device)
{
}

AssetManager::~AssetManager() {}

void AssetManager::clear()
{
    mMeshAssets.clear();
}

SwBool AssetManager::loadMesh(U32 id, const SwChar* filePath)
{
    UNUSED_ARG(id);
    UNUSED_ARG(filePath);
    return false;
}

common::Resource<swizzle::asset2::IMeshAsset> AssetManager::loadMesh2(const SwChar* filePath)
{
    UNUSED_ARG(filePath);
    return nullptr;
}

common::Resource<swizzle::asset2::IMeshAsset> AssetManager::loadAnimMesh(const SwChar* filePath)
{
    UNUSED_ARG(filePath);
    return nullptr;
}

common::Resource<swizzle::asset2::ITextureAsset> AssetManager::loadTexture(const SwChar* filePath)
{
    auto data = swizzle::asset2::LoadTexture(filePath);
    return data;
    // return mDevice->createTexture(data->getWidth(), data->getHeight(), data->getChannels(), false,
    // data->getData()->data());
}

common::Resource<swizzle::asset2::ITextureAsset> AssetManager::loadCubeTexture(const SwChar* right, const SwChar* left,
                                                                               const SwChar* top, const SwChar* bottom,
                                                                               const SwChar* front, const SwChar* back)
{
    auto data = swizzle::asset2::LoadCubeTexture(right, left, top, bottom, front, back);
    return data;
    // return mDevice->createCubeMapTexture(data->getWidth(), data->getHeight(), data->getChannels(),
    // data->getData()->data());
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
