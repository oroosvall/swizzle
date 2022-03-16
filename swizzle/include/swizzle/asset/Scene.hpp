#ifndef SCENE_HPP
#define SCENE_HPP

/* Include files */

#include <swizzle/Api.hpp>
#include <common/Common.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::asset
{
    class Scene
    {
    public:
        virtual ~Scene()
        {
        }

        virtual void loadScene(const SwChar* fileName) = 0;

        virtual void update(DeltaTime dt) = 0;
        virtual void render() = 0;

    };
}

/* Function Declaration */

namespace swizzle::asset
{
    common::Resource<Scene> SWIZZLE_API CreateScene();
}

#endif
