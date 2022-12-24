#ifndef REGULAR_MESH_HPP
#define REGULAR_MESH_HPP

/* Include files */

#include "Renderable.hpp"

#include <swizzle/asset2/Assets.hpp>
#include <swizzle/gfx/Context.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

class RegularMesh : public Renderable
{
public:
    RegularMesh(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<MeshInfo> meshInfo,
                common::Resource<swizzle::gfx::Buffer> inst, common::Resource<swizzle::gfx::Texture> texture,
                common::Resource<swizzle::gfx::Texture> normalTex,
                common::Resource<swizzle::gfx::Texture> optionalTexture, common::Resource<swizzle::gfx::Shader> shader,
                common::Resource<swizzle::gfx::Shader> optionalShader, common::Resource<swizzle::gfx::Shader> shadow);

    virtual void update(DeltaTime dt, SceneRenderSettings& settings,
                        common::Unique<swizzle::gfx::CommandTransaction>& trans) override;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, OrthoCamera& cam) override;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam) override;
    virtual void renderTransform(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam, glm::mat4& mat) override;

private:
    common::Resource<MeshInfo> mMeshInfo;

    common::Resource<swizzle::gfx::Texture> mTexture;
    common::Resource<swizzle::gfx::Texture> mNormalTex;
    common::Resource<swizzle::gfx::Texture> mOptionalTexture;
    common::Resource<swizzle::gfx::Material> mMaterial;
    common::Resource<swizzle::gfx::Shader> mShader;
    common::Resource<swizzle::gfx::Shader> mOptionalShader;

    common::Resource<swizzle::gfx::Buffer> mInst;

    common::Resource<swizzle::gfx::Buffer> mUniform;

    common::Resource<swizzle::gfx::Shader> mSelectedShader;
    common::Resource<swizzle::gfx::Shader> mShadowShader;

    SwBool mNormalEnabled;
};

/* Function Declaration */

#endif
