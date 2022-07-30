#ifndef SWM_NEW_HPP
#define SWM_NEW_HPP

/* Include files */

#include <common/Common_new.hpp>

#include <vector>
#include <string>
#include <filesystem>

/* Defines */

/* Typedefs/enums */

namespace swm
{
    enum class Result
    {
        Success = 0,
        ErrFileIo,
        ErrMagic,
        ErrVersion,
        ErrDataSizeMissmatch,
        ErrToManyAttributes,
    };
}

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swm::types
{
    struct Vector3F
    {
        F32 x, y, z;
    };

    struct Vector2F
    {
        F32 u, v;
    };

    struct Color4U
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

    struct Mesh
    {
        std::string mName;
        U16 mFlags;
        std::vector<Vector3F> mPositions;
        std::vector<Vector3F> mNormals;
        std::vector<Vector2F> mUvs;
        std::vector<Color4U> mVertColors;
        std::vector<BoneInfo> mBoneInfo;

        std::vector<Triangle> mTriangles;

        AnimationInfo mAnimationData;
    };
}

namespace swm::options
{
    struct SaveOptions
    {
        SwBool mCompressVertex;
        SwBool mCompressIndex;
        SwBool mCompressAnimations;
    };
}

/* Class Declaration */

namespace swm
{
    class Model
    {
    public:


        std::vector<types::Mesh> mMeshes;

    };
}

/* Function Declaration */

namespace swm
{
    Result LoadSwm(std::filesystem::path filePath, Model& model);
    Result SaveSwm(std::filesystem::path filePath, const Model& model, options::SaveOptions options);

    const SwChar* GetErrorDescription(Result result);
}

#endif