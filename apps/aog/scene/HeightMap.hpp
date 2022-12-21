#ifndef HEIGHT_MAP_HPP
#define HEIGHT_MAP_HPP

/* Include files */

#include "Renderable.hpp"

#include <swizzle/asset2/Assets.hpp>
#include <swizzle/gfx/Context.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

class HeightMap : public Renderable
{
public:
    HeightMap(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<swizzle::gfx::Buffer> inst,
              common::Resource<swizzle::gfx::Texture> texture, common::Resource<swizzle::gfx::Shader> shader,
              common::Resource<swizzle::gfx::Shader> shadow);

    virtual void update(DeltaTime dt, SceneRenderSettings& settings,
                        common::Unique<swizzle::gfx::CommandTransaction>& trans) override;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, OrthoCamera& cam) override;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam) override;
    virtual void renderMirrorTransform(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam, glm::mat4& mat);

private:
    void loadHeightMap(SwBool cpuDisplace);
    F32 getHeight(U8* image, S32 x, S32 y, S32 ch, F32 xp, F32 yp, F32 max);

    common::Resource<swizzle::gfx::Buffer> mMesh;
    common::Resource<swizzle::gfx::Buffer> mIndex;
    common::Resource<swizzle::gfx::Texture> mTexture;
    common::Resource<swizzle::gfx::Material> mMaterial;
    common::Resource<swizzle::gfx::Shader> mShader;
    common::Resource<swizzle::gfx::Buffer> mInst;

    common::Resource<swizzle::gfx::Buffer> mUniform;
    common::Resource<swizzle::gfx::Shader> mShadowShader;

    SwBool mDisplaced;
};

/* Function Declaration */

#endif
