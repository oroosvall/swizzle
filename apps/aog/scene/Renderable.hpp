#ifndef RENDERABLE_HPP
#define RENDERABLE_HPP

/* Include files */

#include <common/Common.hpp>
#include "SceneSettings.hpp"

#include <swizzle/gfx/Buffer.hpp>
#include <swizzle/gfx/CommandBuffer.hpp>
#include <swizzle/gfx/Material.hpp>
#include <swizzle/gfx/Shader.hpp>
#include <swizzle/gfx/Texture.hpp>

#include "../Camera.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

struct MeshInfo;

/* Struct Declaration */

/* Class Declaration */

class Renderable
{
public:
    virtual ~Renderable() {}

    virtual void update(DeltaTime dt, SceneRenderSettings& settings, common::Unique<swizzle::gfx::CommandTransaction>& trans) = 0;

    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, OrthoCamera& cam) = 0;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam) = 0;
    virtual void renderMirrorTransform(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam, glm::mat4& mat) = 0;

private:
};

/* Function Declaration */

#endif
