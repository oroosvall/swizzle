
/* Include files */

#include "SceneAsset.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::asset
{
    common::Resource<Scene> SWIZZLE_API CreateScene()
    {
        return common::CreateRef<SceneAsset>();
    }
}

/* Class Public Function Definition */

namespace swizzle::asset
{
    SceneAsset::~SceneAsset()
    {
    }

    void SceneAsset::loadScene(const SwChar* fileName)
    {
        UNUSED_ARG(fileName);
    }

    void SceneAsset::update(DeltaTime dt)
    {
        UNUSED_ARG(dt);
    }

    void SceneAsset::render()
    {
    }
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
