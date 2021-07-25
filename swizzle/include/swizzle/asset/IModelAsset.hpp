#ifndef I_MODEL_ASSET_HPP
#define I_MODEL_ASSET_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/gfx/CommandBuffer.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::asset
{
    class IAnimationState
    {

    };

    class IModelAsset
    {
    public:
        virtual ~IModelAsset() {}

        virtual void draw(common::Resource<gfx::CommandBuffer> cmdBuffer) = 0;
        virtual void update(F32 dt) = 0;
        virtual SwBool hasAnimations() = 0;
        virtual IAnimationState& getAnimationState() = 0;
    };
}

/* Function Declaration */

#endif