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

struct MeshInfo
{
    common::Resource<swizzle::asset2::IMeshAsset> mMesh;
    common::Resource<swizzle::gfx::Buffer> mVertex;
    common::Resource<swizzle::gfx::Buffer> mIndex;
};

struct TextureInfo
{
    common::Resource<swizzle::gfx::Texture> mTexture;
};

/* Class Declaration */

class AssetManager
{
public:
    AssetManager(common::Resource<swizzle::gfx::GfxContext> context);
    virtual ~AssetManager();

    bool loadShader(common::Resource<swizzle::gfx::Shader> shader, const SwChar* mFilePath);
    common::Resource<swizzle::gfx::Texture> loadTexture(const SwChar* filePath);
    common::Resource<MeshInfo> loadMesh(const SwChar* filePath, SwBool animated);

    size_t getShaderCount() const;
    ShaderInfo getShaderInfo(size_t index);
    SwBool reloadShader(size_t index);

private:
    common::Resource<swizzle::gfx::GfxContext> mContext;
    std::vector<ShaderInfo> mShaders;

    std::unordered_map<std::string, TextureInfo> mTextures;

    std::unordered_map<std::string, common::Resource<MeshInfo>> mMeshes;
    std::unordered_map<std::string, common::Resource<MeshInfo>> mAnimatedMeshes;


};

/* Function Declaration */

#endif
