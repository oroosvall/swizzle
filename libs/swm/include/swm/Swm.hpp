#ifndef SWM_HPP
#define SWM_HPP

/* Include files */

#include <common/Common.hpp>

#include <vector>
#include <string>
#include <filesystem>

/* Defines */

/* Typedefs/enums */

namespace swm
{
    enum class LoadResult
    {
        Success,
        InvalidHeader,
        InvalidVersion,
        FileIoError,
        SizeError,
        StringError,
        VertexError,
        AnimationError,
        MatrixError
    };

    typedef LoadResult SaveResult;
}

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swm
{
    struct Vertex
    {
        F32 x, y, z;
    };

    struct Uv
    {
        F32 u, v;
    };

    struct Color
    {
        U8 r, g, b, a;
    };

    struct BoneIndex
    {
        U16 b1, b2, b3, b4;
    };

    struct BoneWeights
    {
        F32 w1, w2, w3, w4;
    };

    struct BoneInfo
    {
        BoneIndex mIndex;
        BoneWeights mWeights;
    };

    struct Triangle
    {
        U32 v1, v2, v3;
    };

    struct Matrix4x4
    {
        F32 mMat[16];
    };

    struct AnimationFrame
    {
        std::vector<Matrix4x4> mFrameData;
    };

    struct Animation
    {
        std::string mName;
        std::vector<AnimationFrame> mKeyFrames;
    };

    struct AnimationInfo
    {
        U8 mFramerate = 0u;
        U16 mNumBones = 0u;
        std::vector<U16> mParentList;
        std::vector<Matrix4x4> mBindPose;
        std::vector<Animation> mAnimations;
    };

    struct SaveOptions
    {

    };
}

/* Class Declaration */

namespace swm
{
    class Model
    {
    public:
        struct Mesh
        {
            std::string mName;
            std::vector<Vertex> mPositions;
            std::vector<Vertex> mNormals;
            std::vector<Uv> mUvs;
            std::vector<Color> mVerColors;
            std::vector<BoneInfo> mBoneInfo;

            std::vector<Triangle> mTriangles;

            AnimationInfo mAnimationData;
        };

        std::vector<Mesh> mMeshes;

    };
}

/* Function Declaration */

namespace swm
{
    LoadResult LoadSwm(std::filesystem::path filePath, Model& model);
    SaveResult SaveSwm(std::filesystem::path filePath, const Model& model, SaveOptions options);

}

#endif