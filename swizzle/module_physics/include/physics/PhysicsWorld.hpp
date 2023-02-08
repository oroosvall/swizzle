
#pragma once

/* Include files */

#include <common/Common_new.hpp>
#include <glm/glm.hpp>
#include <physics/PhysicsObject.hpp>

#include <vector>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace physics
{
    class PhysicsWorld
    {
    public:

        void addObject(PhysicsObject* obj);
        void removeObject(PhysicsObject* obj);

        void update(DeltaTime dt);

    private:

        std::vector<PhysicsObject*> mObjects;
    };
}


/* Function Declaration */