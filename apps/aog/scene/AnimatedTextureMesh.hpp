#ifndef ANIMATED_TEXTURE_MESH_HPP
#define ANIMATED_TEXTURE_MESH_HPP

/* Include files */

#include "Renderable.hpp"

#include <swizzle/asset2/Assets.hpp>
#include <swizzle/gfx/Context.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

class AnimatedTextureMesh : public Renderable
{
public:
    AnimatedTextureMesh(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<swizzle::asset2::IMeshAsset> asset,
                common::Resource<swizzle::gfx::Buffer> inst, common::Resource<swizzle::gfx::Texture> texture,
                common::Resource<swizzle::gfx::Shader> shader);

    virtual void update(DeltaTime dt, SceneRenderSettings& settings, common::Unique<swizzle::gfx::CommandTransaction>& trans) override;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam) override;

private:
    common::Resource<swizzle::asset2::IMeshAsset> mAsset;

    common::Resource<swizzle::gfx::Buffer> mMesh;
    common::Resource<swizzle::gfx::Buffer> mIndex;
    common::Resource<swizzle::gfx::Texture> mTexture;
    common::Resource<swizzle::gfx::Material> mMaterial;
    common::Resource<swizzle::gfx::Shader> mShader;
    common::Resource<swizzle::gfx::Buffer> mInst;

    glm::vec2 mUvOffset;
};

/* Function Declaration */

#endif
