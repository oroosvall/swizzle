#ifndef SAVE_V1_HPP
#define SAVE_V1_HPP

/* Include files */

#include <swm/Swm.hpp>
#include <utils/BufferWriter.hpp>

#include <string>
#include <vector>

/* Defines */

#define MESH_FLAGS_V1_UV_BIT 0x00
#define MESH_FLAGS_V1_NORMAL_BIT 0x01
#define MESH_FLAGS_V1_COLOR_BIT 0x02

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace swm_v1
{
    swm::SaveResult SaveV1(utils::BufferWriter& bw, const swm::Model& model, const swm::SaveOptions& options);

    swm::SaveResult SaveMeshV1(utils::BufferWriter& bw, const swm::Model::Mesh& model, const swm::SaveOptions& options);
}

#endif