#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/gfx/Context.hpp>

#include <swizzle/asset2/Assets.hpp>
#include <unordered_map>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

struct ShaderInfo
{
    const SwChar* mFilePath;
    common::Resource<swizzle::gfx::Shader> mShader;
};

/* Class Declaration */

class AssetManager
{
public:
    AssetManager(common::Resource<swizzle::gfx::GfxContext> context);
    virtual ~AssetManager();

    bool loadShader(common::Resource<swizzle::gfx::Shader> shader, const SwChar* mFilePath);

    size_t getShaderCount() const;
    ShaderInfo getShaderInfo(size_t index);
    SwBool reloadShader(size_t index);

private:
    common::Resource<swizzle::gfx::GfxContext> mContext;
    std::vector<ShaderInfo> mShaders;
};

/* Function Declaration */

#endif
