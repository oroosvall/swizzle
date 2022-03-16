#ifndef SCENE_ASSET_HPP
#define SCENE_ASSET_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/asset/Scene.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::asset
{
    class SceneAsset : public Scene
    {
    public:
        virtual ~SceneAsset();

        virtual void loadScene(const SwChar* fileName) override;

        virtual void update(DeltaTime dt) override;
        virtual void render() override;
    };
}

/* Function Declaration */

#endif
