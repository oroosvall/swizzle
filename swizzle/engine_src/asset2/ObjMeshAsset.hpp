#ifndef OBJ_MESH_ASSET_HPP
#define OBJ_MESH_ASSET_HPP

/* Include files */

#include "MeshAsset.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace swizzle::asset2
{
    common::Resource<IMeshAsset> LoadObjMesh(const SwChar* fileName, MeshAssetLoaderDescription& loadInfo);
}

#endif
