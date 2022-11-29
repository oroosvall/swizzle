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
    Sky(common::Resource<swizzle::gfx::Buffer> mesh, common::Resource<swizzle::gfx::Texture> texture,
        common::Resource<swizzle::gfx::Material> material, common::Resource<swizzle::gfx::Shader> shader);

    virtual void update(DeltaTime dt, common::Unique<swizzle::gfx::CommandTransaction>& trans) override;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam) override;

private:
    common::Resource<swizzle::gfx::Buffer> mMesh;
    common::Resource<swizzle::gfx::Texture> mTexture;
    common::Resource<swizzle::gfx::Material> mMaterial;
    common::Resource<swizzle::gfx::Shader> mShader;
};

/* Function Declaration */

#endif
