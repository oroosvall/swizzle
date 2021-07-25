#ifndef MODEL_ASSET_HPP
#define MODEL_ASSET_HPP

/* Include files */

#include <swizzle/asset/IModelAsset.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::asset
{
    class AnimationState : public IAnimationState
    {

    };

    class ModelAsset : public IModelAsset
    {
    public:
        virtual ~ModelAsset() {}

        virtual void draw(common::Resource<gfx::CommandBuffer> cmdBuffer) override;
        virtual void update(F32 dt) override;
        virtual SwBool hasAnimations() override;
        virtual IAnimationState& getAnimationState() override;

    private:


    };
}

/* Function Declaration */

#endif
