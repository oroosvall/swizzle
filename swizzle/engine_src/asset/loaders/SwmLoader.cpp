
/* Include files */

#include "SwmLoader.hpp"
#include "LoaderCommon.hpp"
#include <swm/Swm.hpp>



/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

static const glm::mat4 flipYZ = glm::mat4(
    1.0F, 0.0F, 0.0F, 0.0F,
    0.0F, 0.0F, 1.0F, 0.0F,
    0.0F, 1.0F, 0.0F, 0.0F,
    0.0F, 0.0F, 0.0F, 1.0F);

/* Static Function Declaration */

U16 FindRoot(const std::vector<U16>& parentList);
std::vector<U16> FindChildren(U16 index, const std::vector<U16>& parentList);

void CalcLocal(U16 boneIndx, std::vector<glm::mat4>& mats, const std::vector<U16>& parentList, glm::mat4 parent);

void CalcLocalBindPose(std::vector<glm::mat4>& mats, const std::vector<U16>& parentList);

std::vector<glm::mat4> GetInverse(const std::vector<glm::mat4>& mats, U32 numMats);
std::vector<glm::mat4> CalcPose(const std::vector<glm::mat4>& lbp, const std::vector<glm::mat4>& inverse);

/* Static Function Definition */


U16 FindRoot(const std::vector<U16>& parentList)
{
    U32 count = (U32)parentList.size();
    for (U32 i = 0u; i < count; ++i)
    {
        if (parentList[i] == i)
        {
            return (U16)i;
        }
    }
    return 0u;
}

std::vector<U16> FindChildren(U16 index, const std::vector<U16>& parentList)
{
    std::vector<U16> childs;
    U32 count = (U32)parentList.size();
    for (U32 i = 0u; i < count; ++i)
    {
        if ((parentList[i] == index) && (i != index))
        {
            childs.push_back((U16)i);
        }
    }

    return childs;
}

void CalcLocal(U16 boneIndex, std::vector<glm::mat4>& mats, const std::vector<U16>& parentList, glm::mat4 parent)
{
    glm::mat4 fx = mats[boneIndex];

    glm::mat4 lbp = fx * parent;
    std::vector<U16> childs = FindChildren(boneIndex, parentList);
    for (auto c : childs)
    {
        CalcLocal(c, mats, parentList, lbp);
    }

    mats[boneIndex] = lbp;
}

void CalcLocalBindPose(std::vector<glm::mat4>& mats, const std::vector<U16>& parentList)
{
    U16 root = FindRoot(parentList);
    CalcLocal(root, mats, parentList, glm::mat4(1.0));
}

std::vector<glm::mat4> GetInverse(const std::vector<glm::mat4>& mats, U32 numMats)
{
    std::vector<glm::mat4> inverseBindPose;
    for (U32 i = 0u; i < numMats; ++i)
    {
        inverseBindPose.push_back(glm::inverse(mats[i]));
    }
    return inverseBindPose;
}

std::vector<glm::mat4> CalcPose(const std::vector<glm::mat4>& lbp, const std::vector<glm::mat4>& inverse)
{
    std::vector<glm::mat4> pose;
    for (size_t i = 0u; i < inverse.size(); ++i)
    {
        auto m = inverse[i] * lbp[i];
        m = glm::transpose(m);

        pose.push_back(m);
    }

    return pose;
}

/* Function Definition */

namespace loader::swm
{
    SwBool loadSwmFile(const SwChar* fileName, F32** vertexData, U32& numVertecies, U32** indexData, U32& numTriangles, F32** boneData, U32& numBones)
    {
        SwBool ok = false;
        ::swm::Model mdl;
        if (::swm::LoadSwm(fileName, mdl) == ::swm::LoadResult::Success)
        {
            numVertecies = (U32)mdl.mMeshes[0].mPositions.size();
            numTriangles = (U32)mdl.mMeshes[0].mTriangles.size();
            ::swm::AnimationInfo animInfo = mdl.mMeshes[0].mAnimationData;
            F32* verts = nullptr;

            if (animInfo.mNumBones != 0)
            {
                verts = new F32[numVertecies * sizeof(loader::VertexAnim)];
            }
            else
            {
                verts = new F32[numVertecies * sizeof(loader::Vertex)];
            }
            U32* tris = new U32[numTriangles * sizeof(loader::Index)];

            for (U32 i = 0u; i < numVertecies; ++i)
            {
                if (animInfo.mNumBones != 0)
                {
                    loader::VertexAnim* vverts = (loader::VertexAnim*)verts;

                    vverts[i].pos.x = mdl.mMeshes[0].mPositions[i].x;
                    vverts[i].pos.y = mdl.mMeshes[0].mPositions[i].y;
                    vverts[i].pos.z = mdl.mMeshes[0].mPositions[i].z;

                    vverts[i].norm.x = mdl.mMeshes[0].mNormals[i].x;
                    vverts[i].norm.y = mdl.mMeshes[0].mNormals[i].y;
                    vverts[i].norm.z = mdl.mMeshes[0].mNormals[i].z;

                    vverts[i].uv.u = mdl.mMeshes[0].mUvs[i].u;
                    vverts[i].uv.v = mdl.mMeshes[0].mUvs[i].v;

                    vverts[i].idx.i1 = mdl.mMeshes[0].mBoneInfo[i].mIndex.b1;
                    vverts[i].idx.i2 = mdl.mMeshes[0].mBoneInfo[i].mIndex.b2;
                    vverts[i].idx.i3 = mdl.mMeshes[0].mBoneInfo[i].mIndex.b3;
                    vverts[i].idx.i4 = mdl.mMeshes[0].mBoneInfo[i].mIndex.b4;

                    vverts[i].wgt.w1 = mdl.mMeshes[0].mBoneInfo[i].mWeights.w1;
                    vverts[i].wgt.w2 = mdl.mMeshes[0].mBoneInfo[i].mWeights.w2;
                    vverts[i].wgt.w3 = mdl.mMeshes[0].mBoneInfo[i].mWeights.w3;
                    vverts[i].wgt.w4 = mdl.mMeshes[0].mBoneInfo[i].mWeights.w4;
                }
                else
                {
                    loader::Vertex* vverts = (loader::Vertex*)verts;
                    vverts[i].pos.x = mdl.mMeshes[0].mPositions[i].x;
                    vverts[i].pos.y = mdl.mMeshes[0].mPositions[i].y;
                    vverts[i].pos.z = mdl.mMeshes[0].mPositions[i].z;

                    vverts[i].norm.x = mdl.mMeshes[0].mNormals[i].x;
                    vverts[i].norm.y = mdl.mMeshes[0].mNormals[i].y;
                    vverts[i].norm.z = mdl.mMeshes[0].mNormals[i].z;

                    vverts[i].uv.u = mdl.mMeshes[0].mUvs[i].u;
                    vverts[i].uv.v = mdl.mMeshes[0].mUvs[i].v;
                }
            }

            loader::Index* ttris = (loader::Index*)tris;

            for (U32 i = 0u; i < numTriangles; ++i)
            {
                ttris[i].i1 = mdl.mMeshes[0].mTriangles[i].v1;
                ttris[i].i2 = mdl.mMeshes[0].mTriangles[i].v2;
                ttris[i].i3 = mdl.mMeshes[0].mTriangles[i].v3;
            }

            if (animInfo.mNumBones != 0)
            {
                std::vector<glm::mat4> mats;
                mats.resize(animInfo.mNumBones);
                memcpy(mats.data(), animInfo.mBindPose.data(), sizeof(glm::mat4) * animInfo.mNumBones);

                CalcLocalBindPose(mats, animInfo.mParentList);

                std::vector<glm::mat4> inverse = GetInverse(mats, animInfo.mNumBones);

                memcpy(mats.data(), animInfo.mAnimations[0].mKeyFrames.data(), sizeof(glm::mat4) * animInfo.mNumBones);
                CalcLocalBindPose(mats, animInfo.mParentList);

                std::vector<glm::mat4> pose = CalcPose(mats, inverse);

                numBones = animInfo.mNumBones;
                F32* bData = new F32[sizeof(glm::mat4) * numBones];
                memcpy(bData, pose.data(), sizeof(glm::mat4) * numBones);
                *boneData = bData;
            }

            *vertexData = verts;
            *indexData = tris;
            ok = true;
        }

        return ok;
    }
}
/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
