#ifndef SWM_MODEL_V1_LOAD_HPP
#define SWM_MODEL_V1_LOAD_HPP

/* Include files */

#include <swm/Swm.hpp>
#include <utils/BufferReader.hpp>

#include <string>
#include <vector>

/* Defines */

#define MESH_FLAGS_V1_UV_BIT 0x00
#define MESH_FLAGS_V1_NORMAL_BIT 0x01
#define MESH_FLAGS_V1_COLOR_BIT 0x02

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swm_v1
{
    struct Mesh_v1
    {
        std::string mName;
        U16 mMeshFlags = 0u;

        std::vector<swm::Vertex> mVertices;
        std::vector<swm::Uv> mUvs;
        std::vector<swm::Vertex> mNormals;
        std::vector<swm::Color> mColors;

        std::vector<swm::Triangle> mTriangles;

    };
}

/* Class Declaration */

/* Function Declaration */

namespace swm_v1
{
    swm::LoadResult LoadV1(U16 minorVer, utils::BufferReader& br, swm::Model& mdl);

    swm::LoadResult LoadMeshV1(utils::BufferReader& br, Mesh_v1& mesh);

}

#endif