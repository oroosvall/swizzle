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
    AnimatedTextureMesh(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<MeshInfo> meshInfo,
                        common::Resource<swizzle::gfx::Buffer> inst, common::Resource<swizzle::gfx::Texture> texture,
                        common::Resource<swizzle::gfx::Shader> shader,
                        common::Resource<swizzle::gfx::Shader> shadowShader);

    virtual void update(DeltaTime dt, SceneRenderSettings& settings,
                        common::Unique<swizzle::gfx::CommandTransaction>& trans) override;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, OrthoCamera& cam) override;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam) override;

private:
    common::Resource<MeshInfo> mMeshInfo;

    common::Resource<swizzle::gfx::Texture> mTexture;
    common::Resource<swizzle::gfx::Material> mMaterial;
    common::Resource<swizzle::gfx::Shader> mShader;
    common::Resource<swizzle::gfx::Buffer> mInst;

    common::Resource<swizzle::gfx::Shader> mShadowShader;

    common::Resource<swizzle::gfx::Buffer> mUniform;

    glm::vec2 mUvOffset;
};

/* Function Declaration */

#endif
