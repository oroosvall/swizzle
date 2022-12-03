
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
