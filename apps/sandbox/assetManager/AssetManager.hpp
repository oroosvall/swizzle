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

/* Class Declaration */

class AssetManager
{
public:
    AssetManager(common::Resource<swizzle::gfx::GfxDevice> device);
    virtual ~AssetManager();

    void clear();
    SwBool loadMesh(U32 id, const SwChar* filePath);
    common::Resource<swizzle::asset2::IMeshAsset> loadMesh2(const SwChar* filePath);
    common::Resource<swizzle::asset2::IMeshAsset> loadAnimMesh(const SwChar* filePath);

    common::Resource<swizzle::asset2::ITextureAsset> loadTexture(const SwChar* filePath);
    common::Resource<swizzle::asset2::ITextureAsset> loadCubeTexture(const SwChar* right, const SwChar* left,
                                                                     const SwChar* top, const SwChar* bottom,
                                                                     const SwChar* front, const SwChar* back);

private:
    common::Resource<swizzle::gfx::GfxDevice> mDevice;

    std::unordered_map<U32, common::Resource<swizzle::asset2::IMeshAsset>> mMeshAssets;
    // std::unordered_map<U32, swizzle::gfx::Texture> mTextures;
};

/* Function Declaration */

#endif
