
/* Include files */

#include "MeshAnimation.hpp"

#include <glm/glm.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

namespace swizzle::asset2
{
    static U16 FindRoot(const std::vector<U16>& parentList);

    static std::vector<U16> FindChildren(U16 index, const std::vector<U16>& parentList);

    static void CalcLocal(U16 boneIndex, std::vector<glm::mat4>& mats, const std::vector<U16>& parentList, glm::mat4 parent);
    static void CalcLocalBindPose(std::vector<glm::mat4>& mats, const std::vector<U16>& parentList);

    static std::vector<glm::mat4> GetInverse(const std::vector<glm::mat4>& mats, U32 numMats);
    static std::vector<glm::mat4> CalcPose(const std::vector<glm::mat4>& lbp, const std::vector<glm::mat4>& inverse);
}

/* Static Variables */

namespace swizzle::asset2
{
    static const glm::mat4 flipYZ = glm::mat4(
        1.0F, 0.0F, 0.0F, 0.0F,
        0.0F, 0.0F, 1.0F, 0.0F,
        0.0F, 1.0F, 0.0F, 0.0F,
        0.0F, 0.0F, 0.0F, 1.0F);
}

/* Static Function Definition */

namespace swizzle::asset2
{
    static U16 FindRoot(const std::vector<U16>& parentList)
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

    static std::vector<U16> FindChildren(U16 index, const std::vector<U16>& parentList)
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

    static void CalcLocal(U16 boneIndex, std::vector<glm::mat4>& mats, const std::vector<U16>& parentList, glm::mat4 parent)
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

    static void CalcLocalBindPose(std::vector<glm::mat4>& mats, const std::vector<U16>& parentList)
    {
        U16 root = FindRoot(parentList);
        CalcLocal(root, mats, parentList, glm::mat4(1.0));
    }

    static std::vector<glm::mat4> GetInverse(const std::vector<glm::mat4>& mats, U32 numMats)
    {
        std::vector<glm::mat4> inverseBindPose;
        for (U32 i = 0u; i < numMats; ++i)
        {
            inverseBindPose.push_back(glm::inverse(mats[i]));
        }
        return inverseBindPose;
    }

    static std::vector<glm::mat4> CalcPose(const std::vector<glm::mat4>& lbp, const std::vector<glm::mat4>& inverse)
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
}

/* Function Definition */

namespace swizzle::asset2
{
    void LoadAnimations(const swm::Model::Mesh& mesh, std::vector<Animation>& animationData)
    {
        const auto& animInfo = mesh.mAnimationData;
        if (animInfo.mNumBones != 0u)
        {
            for (const auto& anim : animInfo.mAnimations)
            {
                Animation a;
                a.mName = anim.mName;
                for (const auto& frame : anim.mKeyFrames)
                {
                    std::vector<glm::mat4> mats;
                    mats.resize(animInfo.mNumBones);

                    memcpy((F32*)mats.data(), (F32*)animInfo.mBindPose.data(), sizeof(glm::mat4) * animInfo.mNumBones);

                    CalcLocalBindPose(mats, animInfo.mParentList);

                    std::vector<glm::mat4> inverse = GetInverse(mats, animInfo.mNumBones);

                    memcpy((F32*)mats.data(), (F32*)frame.mFrameData.data(), sizeof(glm::mat4) * animInfo.mNumBones);
                    CalcLocalBindPose(mats, animInfo.mParentList);

                    KeyFrame kf;
                    kf.mFrameData = CalcPose(mats, inverse);

                    a.mKeyFrames.emplace_back(std::move(kf));

                }
                animationData.emplace_back(std::move(a));
            }
        }

    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
