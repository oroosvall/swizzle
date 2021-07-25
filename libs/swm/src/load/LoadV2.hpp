#ifndef SWM_MODEL_V2_LOAD_HPP
#define SWM_MODEL_V2_LOAD_HPP

/* Include files */

#include <swm/Swm.hpp>
#include <utils/BufferReader.hpp>

#include <string>
#include <vector>

/* Defines */

#define MESH_FLAGS_V2_UV_BIT 0x00
#define MESH_FLAGS_V2_NORMAL_BIT 0x01
#define MESH_FLAGS_V2_COLOR_BIT 0x02

#define MESH_FLAGS_V2_ANIM_BIT 0x08

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swm_v2
{
    struct Animation_v2
    {
        U8 mFrameRate;
        std::vector<U16> mParentList;
        std::vector<swm::Matrix4x4> mBones;
        std::vector<swm::Animation> mAnimations;
    };

    struct Mesh_v2
    {
        std::string mName;
        U16 mMeshFlags = 0u;

        std::vector<swm::Vertex> mVertices;
        std::vector<swm::Uv> mUvs;
        std::vector<swm::Vertex> mNormals;
        std::vector<swm::Color> mColors;

        std::vector<swm::BoneIndex> mBoneIndex;
        std::vector<swm::BoneWeights> mBoneWeights;

        std::vector<swm::Triangle> mTriangles;

        Animation_v2 mAnimation;

    };
}

/* Class Declaration */

/* Function Declaration */

namespace swm_v2
{
    swm::LoadResult LoadV2(U16 minorVer, utils::BufferReader& br, swm::Model& mdl);

    swm::LoadResult LoadMeshV2(utils::BufferReader& br, Mesh_v2& mesh);
    swm::LoadResult LoadAnimationSectionV2(utils::BufferReader& br, Animation_v2& animation);

    swm::LoadResult LoadAnimationV2(utils::BufferReader& br, swm::Animation& animation, U16 numBones);

}

#endif