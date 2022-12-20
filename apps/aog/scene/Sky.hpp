#ifndef SKY_HPP
#define SKY_HPP

/* Include files */

#include "Renderable.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

class Sky : public Renderable
{
public:
    Sky(common::Resource<swizzle::gfx::Buffer> mesh, common::Resource<swizzle::gfx::Material> material,
        common::Resource<swizzle::gfx::Shader> shader, common::Resource<swizzle::gfx::Buffer> uniform,
        common::Resource<swizzle::gfx::Buffer> inst);

    virtual void update(DeltaTime dt, SceneRenderSettings& settings,
                        common::Unique<swizzle::gfx::CommandTransaction>& trans) override;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, OrthoCamera& cam) override;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam) override;

private:
    common::Resource<swizzle::gfx::Buffer> mMesh;
    common::Resource<swizzle::gfx::Material> mMaterial;
    common::Resource<swizzle::gfx::Shader> mShader;

    common::Resource<swizzle::gfx::Buffer> mUniformBuffer;
    common::Resource<swizzle::gfx::Buffer> mInst;
};

/* Function Declaration */

#endif
