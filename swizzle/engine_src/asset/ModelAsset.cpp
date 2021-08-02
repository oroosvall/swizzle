
/* Include files */

#include "ModelAsset.hpp"
#include <swm/Swm.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::asset
{
    common::Resource<IModelAsset> LoadModelAsset(common::Resource<gfx::GfxContext> ctx, const SwChar* fileName)
    {
        UNUSED_ARG(ctx);
        UNUSED_ARG(fileName);

        common::Resource<IModelAsset> model;

        swm::Model mdl;

        if (swm::LoadSwm(fileName, mdl) == swm::LoadResult::Success)
        {
            model = common::CreateRef<ModelAsset>();
        }

        return model;
    }
}

/* Class Public Function Definition */

namespace swizzle::asset
{
    ModelAsset::ModelAsset()
        : mVertexBuffer(nullptr)
        , mIndexBuffer(nullptr)
        , mBoneBuffer(nullptr)
    {

    }

    ModelAsset::~ModelAsset()
    {
    }

    void ModelAsset::draw(common::Resource<gfx::CommandBuffer> cmdBuffer)
    {
        UNUSED_ARG(cmdBuffer)
    }

    void ModelAsset::update(F32 dt)
    {
        UNUSED_ARG(dt)
    }

    SwBool ModelAsset::hasAnimations()
    {
        return false;
    }

    IAnimationState& ModelAsset::getAnimationState()
    {
        static AnimationState as{};
        return as;
    }
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
