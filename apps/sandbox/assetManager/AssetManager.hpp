#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/gfx/Context.hpp>

#include <unordered_map>
#include <swizzle/asset/MeshLoader.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

class AssetManager
{
public:
    AssetManager(common::Resource<swizzle::gfx::GfxContext> context);
    virtual ~AssetManager();

    void clear();
    SwBool loadMesh(U32 id, const SwChar* filePath);
    swizzle::Mesh loadMesh2(const SwChar* filePath);
    swizzle::MeshAnimated loadAnimMesh(const SwChar* filePath);

    common::Resource<swizzle::gfx::Texture> loadTexture(const SwChar* filePath);
    common::Resource<swizzle::gfx::Texture> loadCubeTexture(const SwChar* right, const SwChar* left, const SwChar* top, const SwChar* bottom, const SwChar* front, const SwChar* back);

private:

    common::Resource<swizzle::gfx::GfxContext> mContext;

    std::unordered_map<U32, swizzle::Mesh> mMeshAssets;
    //std::unordered_map<U32, swizzle::gfx::Texture> mTextures;
};

/* Function Declaration */

#endif
