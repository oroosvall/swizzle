#ifndef RENDERABLE_HPP
#define RENDERABLE_HPP

/* Include files */

#include <common/Common.hpp>

#include <swizzle/gfx/CommandBuffer.hpp>
#include <swizzle/gfx/Buffer.hpp>
#include <swizzle/gfx/Material.hpp>
#include <swizzle/gfx/Shader.hpp>
#include <swizzle/gfx/Texture.hpp>

#include "../Camera.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

class Renderable
{
public:

    virtual ~Renderable()
    {
    }

    virtual void update(DeltaTime dt, common::Resource<swizzle::gfx::CommandBuffer> cmd) = 0;
    virtual void render(common::Resource<swizzle::gfx::CommandBuffer> cmd, PerspectiveCamera& cam) = 0;

private:

};

/* Function Declaration */

#endif
