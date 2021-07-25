#ifndef SWM_LOADER_HPP
#define SWM_LOADER_HPP

/* Include files */
#include <common/Common.hpp>
#include "LoaderCommon.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace loader::swm
{
    SwBool loadSwmFile(const SwChar* fileName, F32** vertexData, U32& numVertecies, U32** indexData, U32& numTriangles, F32** boneData, U32& numBones);
}

#endif