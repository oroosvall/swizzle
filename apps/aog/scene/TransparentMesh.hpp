#ifndef TRANSPARENT_MESH_HPP
#define TRANSPARENT_MESH_HPP

/* Include files */

#include "Renderable.hpp"

#include <swizzle/asset2/Assets.hpp>
#include <swizzle/gfx/Context.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

class TransparentMesh : public Renderable
{
public:
    TransparentMesh(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<MeshInfo> meshInfo,
                    common::Resource<swizzle::gfx::Buffer> inst, common::Resource<swizzle::gfx::Shader> shader,
                    common::Resource<swizzle::gfx::Shader> shadow, glm::vec3 pos, glm::vec4 col);

    virtual void update(DeltaTime dt, SceneRenderSettings& settings,
                        common::Unique<swizzle::gfx::CommandTransaction>& trans) override;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, OrthoCamera& cam) override;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam) override;
    virtual void renderMirrorTransform(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans,
                                       PerspectiveCamera& cam, glm::mat4& mat) override;

    glm::vec3 getPos() const;

private:
    common::Resource<MeshInfo> mMeshInfo;

    common::Resource<swizzle::gfx::Shader> mShader;
    common::Resource<swizzle::gfx::Material> mMaterial;
    common::Resource<swizzle::gfx::Buffer> mUniform;

    common::Resource<swizzle::gfx::Buffer> mInst;

    common::Resource<swizzle::gfx::Shader> mShadowShader;

    glm::vec3 mPos;
    glm::vec4 mCol;
};

/* Function Declaration */

#endif
