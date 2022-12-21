#ifndef MIRROR_HPP
#define MIRROR_HPP

/* Include files */

#include "Renderable.hpp"

#include <swizzle/asset2/Assets.hpp>
#include <swizzle/gfx/Context.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

class Mirror : public Renderable
{
public:
    Mirror(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<MeshInfo> frane,
           common::Resource<MeshInfo> plane, common::Resource<swizzle::gfx::Shader> frameShader,
           common::Resource<swizzle::gfx::Shader> planeShader, common::Resource<swizzle::gfx::Shader> shadow,
           common::Resource<swizzle::gfx::Shader> clearDepth, common::Resource<swizzle::gfx::Buffer> inst, glm::mat4 trans);

    virtual void update(DeltaTime dt, SceneRenderSettings& settings,
                        common::Unique<swizzle::gfx::CommandTransaction>& trans) override;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, OrthoCamera& cam) override;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam) override;
    virtual void renderMirrorTransform(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam, glm::mat4& mat);

    void renderPlane(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam);

    glm::mat4 getMirrorTransform() const;

private:
    common::Resource<MeshInfo> mFrame;
    common::Resource<MeshInfo> mPlane;

    common::Resource<swizzle::gfx::Shader> mFrameShader;
    common::Resource<swizzle::gfx::Shader> mPlaneShader;
    common::Resource<swizzle::gfx::Shader> mShadowShader;
    common::Resource<swizzle::gfx::Shader> mClearDepth;
    common::Resource<swizzle::gfx::Buffer> mInstance;

    glm::mat4 mTransform;
};

/* Function Declaration */

#endif
