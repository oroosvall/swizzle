#ifndef WATER_HPP
#define WATER_HPP

/* Include files */

#include "Renderable.hpp"

#include <swizzle/asset2/Assets.hpp>
#include <swizzle/gfx/Context.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

class Water : public Renderable
{
public:
    Water(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<swizzle::gfx::Buffer> inst,
          common::Resource<swizzle::gfx::Shader> shader, common::Resource<swizzle::gfx::Shader> compute);

    virtual void update(DeltaTime dt, SceneRenderSettings& settings,
                        common::Unique<swizzle::gfx::CommandTransaction>& trans) override;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam) override;

private:

    void generatePlane();

    common::Resource<swizzle::gfx::Buffer> mVertexBuffer;
    common::Resource<swizzle::gfx::Buffer> mIndexBuffer;

    common::Resource<swizzle::gfx::Shader> mShader;
    common::Resource<swizzle::gfx::Shader> mCompute;
    common::Resource<swizzle::gfx::Material> mMaterial;
    common::Resource<swizzle::gfx::Material> mMatCompute;

    common::Resource<swizzle::gfx::Texture> mHeight1;
    common::Resource<swizzle::gfx::Texture> mHeight2;

    common::Resource<swizzle::gfx::Buffer> mInst;

    common::Resource<swizzle::gfx::Buffer> mUniform;

    SwBool mActiveTexture;
    F32 mTime = 0.0f;
};

/* Function Declaration */

#endif
