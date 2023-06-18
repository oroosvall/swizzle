#ifndef ANIMATED_HPP
#define ANIMATED_HPP

/* Include files */

#include "Renderable.hpp"

#include <swizzle/asset2/Assets.hpp>
#include <swizzle/gfx/Context.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

class Animated : public Renderable
{
public:
    Animated(common::Resource<swizzle::gfx::GfxDevice> dev, common::Resource<swizzle::asset2::IMeshAsset> asset,
             common::Resource<swizzle::gfx::GfxBuffer> inst, common::Resource<swizzle::gfx::Texture> texture,
             common::Resource<swizzle::gfx::Shader> shader);

    virtual void update(DeltaTime dt, common::Unique<swizzle::gfx::CommandTransaction>& trans) override;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam) override;

private:
    common::Resource<swizzle::asset2::IMeshAsset> mAsset;

    common::Resource<swizzle::gfx::GfxBuffer> mMesh;
    common::Resource<swizzle::gfx::GfxBuffer> mIndex;
    common::Resource<swizzle::gfx::GfxBuffer> mBone;
    common::Resource<swizzle::gfx::GfxBuffer> mInst;
    common::Resource<swizzle::gfx::Texture> mTexture;
    common::Resource<swizzle::gfx::Material> mMaterial;
    common::Resource<swizzle::gfx::Shader> mShader;

    F32 mAnimDelta;
    U32 mCurAnim;
    U32 mFrameIndex;
};

/* Function Declaration */

#endif
