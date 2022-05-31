
/* Include files */

#include "LoadV2.hpp"
#include "LoadBaseTypes.hpp"

#include <glm/glm.hpp>
#include <utils/BitUtil.hpp>

/* Defines */

/* Typedefs */

namespace swm_v2
{
    using LR = swm::LoadResult;
    using SVertex = swm::Vertex;
    using SUv = swm::Uv;
    using SColor = swm::Color;
    using SBoneIndex = swm::BoneIndex;
    using SBoneWeights = swm::BoneWeights;
    using STriangle = swm::Triangle;

    using SMatrix = swm::Matrix4x4;
} // namespace swm_v2

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swm_v2
{
    swm::LoadResult LoadV2(U16 minorVer, utils::BufferReader& br, swm::Model& mdl)
    {
        UNUSED_ARG(minorVer);
        U8 numMeshes = 0u;

        swm::LoadResult res = swm_base::LoadNumber<U8, LR::SizeError>(br, numMeshes);

        if (swm_base::ResultIsOk(res))
        {
            for (S32 i = 0; (i < (S32)numMeshes) && swm_base::ResultIsOk(res); ++i)
            {
                swm::Model::Mesh mesh;
                Mesh_v2 m;
                res = LoadMeshV2(br, m);
                if (swm_base::ResultIsOk(res))
                {
                    mesh.mName = m.mName;

                    mesh.mPositions = std::move(m.mVertices);
                    mesh.mNormals = std::move(m.mNormals);
                    mesh.mUvs = std::move(m.mUvs);
                    mesh.mVerColors = std::move(m.mColors);

                    for (U32 j = 0u; j < m.mBoneIndex.size(); ++j)
                    {
                        mesh.mBoneInfo.push_back({});
                        mesh.mBoneInfo.back().mIndex = m.mBoneIndex[j];
                        mesh.mBoneInfo.back().mWeights = m.mBoneWeights[j];
                    }

                    mesh.mTriangles = std::move(m.mTriangles);
                    mesh.mAnimationData.mFramerate = m.mAnimation.mFrameRate;
                    mesh.mAnimationData.mNumBones = (U16)m.mAnimation.mBones.size();
                    mesh.mAnimationData.mParentList = std::move(m.mAnimation.mParentList);
                    mesh.mAnimationData.mBindPose = std::move(m.mAnimation.mBones);
                    mesh.mAnimationData.mAnimations = std::move(m.mAnimation.mAnimations);

                    mdl.mMeshes.emplace_back(mesh);
                }
            }
        }

        return res;
    }

    swm::LoadResult LoadMeshV2(utils::BufferReader& br, Mesh_v2& mesh)
    {
        swm::LoadResult res = swm_base::LoadString(br, mesh.mName);

        if (swm_base::ResultIsOk(res))
        {
            res = swm_base::LoadNumber<U16, LR::SizeError>(br, mesh.mMeshFlags);
        }

        if (swm_base::ResultIsOk(res))
        {
            U32 numVerts = 0u;
            res = swm_base::LoadNumber<U32, LR::SizeError>(br, numVerts);

            if (swm_base::ResultIsOk(res))
            {
                res = swm_base::LoadArray<SVertex, LR::VertexError>(br, mesh.mVertices, numVerts);
            }

            if (swm_base::ResultIsOk(res) && utils::IsBitSet(mesh.mMeshFlags, MESH_FLAGS_V2_UV_BIT))
            {
                res = swm_base::LoadArray<SUv, LR::VertexError>(br, mesh.mUvs, numVerts);
            }

            if (swm_base::ResultIsOk(res) && utils::IsBitSet(mesh.mMeshFlags, MESH_FLAGS_V2_NORMAL_BIT))
            {
                res = swm_base::LoadArray<SVertex, LR::VertexError>(br, mesh.mNormals, numVerts);
            }

            if (swm_base::ResultIsOk(res) && utils::IsBitSet(mesh.mMeshFlags, MESH_FLAGS_V2_COLOR_BIT))
            {
                res = swm_base::LoadArray<SColor, LR::VertexError>(br, mesh.mColors, numVerts);
            }

            if (swm_base::ResultIsOk(res) && utils::IsBitSet(mesh.mMeshFlags, MESH_FLAGS_V2_ANIM_BIT))
            {
                res = swm_base::LoadArray<SBoneIndex, LR::VertexError>(br, mesh.mBoneIndex, numVerts);
            }

            if (swm_base::ResultIsOk(res) && utils::IsBitSet(mesh.mMeshFlags, MESH_FLAGS_V2_ANIM_BIT))
            {
                res = swm_base::LoadArray<SBoneWeights, LR::VertexError>(br, mesh.mBoneWeights, numVerts);
            }
        }

        if (swm_base::ResultIsOk(res))
        {
            U32 numTris = 0;
            res = swm_base::LoadNumber<U32, LR::SizeError>(br, numTris);
            if (swm_base::ResultIsOk(res))
            {
                res = swm_base::LoadArray<STriangle, LR::VertexError>(br, mesh.mTriangles, numTris);
            }
        }

        if (swm_base::ResultIsOk(res) && utils::IsBitSet(mesh.mMeshFlags, MESH_FLAGS_V2_ANIM_BIT))
        {
            res = LoadAnimationSectionV2(br, mesh.mAnimation);
        }


        return res;
    }

    swm::LoadResult LoadAnimationSectionV2(utils::BufferReader& br, Animation_v2& animation)
    {
        swm::LoadResult res = swm_base::LoadNumber<U8, LR::SizeError>(br, animation.mFrameRate);

        U16 numBones = 0u;
        if (swm_base::ResultIsOk(res))
        {
            res = swm_base::LoadNumber<U16, LR::SizeError>(br, numBones);
        }
        if (swm_base::ResultIsOk(res))
        {
            res = swm_base::LoadArray<U16, LR::SizeError>(br, animation.mParentList, numBones);
        }
        if (swm_base::ResultIsOk(res))
        {
            res = swm_base::LoadArray<SMatrix, LR::MatrixError>(br, animation.mBones, numBones);
        }

        if (swm_base::ResultIsOk(res))
        {
            U16 numAnimations = 0u;
            res = swm_base::LoadNumber<U16, LR::SizeError>(br, numAnimations);

            if (swm_base::ResultIsOk(res))
            {
                animation.mAnimations.resize(numAnimations);
                for (auto& a : animation.mAnimations)
                {
                    res = LoadAnimationV2(br, a, numBones);
                    if (!swm_base::ResultIsOk(res))
                    {
                        break;
                    }
                }
            }
        }

        return res;
    }

    swm::LoadResult LoadAnimationV2(utils::BufferReader& br, swm::Animation& animation, U16 numBones)
    {
        swm::LoadResult res = swm_base::LoadString(br, animation.mName);

        if (swm_base::ResultIsOk(res))
        {
            U16 numKeyframes = 0u;
            res = swm_base::LoadNumber<U16, LR::SizeError>(br, numKeyframes);
            if (swm_base::ResultIsOk(res))
            {
                animation.mKeyFrames.resize(numKeyframes);
                for (auto& frame : animation.mKeyFrames)
                {
                    res = swm_base::LoadArray<SMatrix, LR::MatrixError>(br, frame.mFrameData, numBones);
                    if (!swm_base::ResultIsOk(res))
                    {
                        break;
                    }
                }
            }
        }

        return res;
    }
} // namespace swm_v2

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
