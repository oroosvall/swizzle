#ifndef I_ASSET_LOADER_HPP
#define I_ASSET_LOADER_HPP

/* Include files */

#include <common/Common.hpp>
#include <filesystem>

/* Defines */

/* Typedefs/enums */

namespace assetloader
{
    enum class LoadResult
    {
        NotSupported, // Loader cannot load this file type
        LoadingFailedVersion, // Unsupported file version
        LoadingFailedParsing, // Parsing of some data failed, e.g size fields does not add up
        LoadingFailedMissingData, // Loading failed because data was missing. e.g loading required normals but no normal data present
        LoadingOk // Loading file was ok
    };

    enum class ModelInfo
    {
        All,
        Vertex,
        Normal,
        Uv,
        Color,
        BoneIndex,
        BoneWeight,
    };

    enum class DataType
    {
        ignore, // data not needed
        vec2f, // 2 x 4 bytes (float)
        vec3f, // 3 x 4 bytes (float)
        vec4f, // 4 x 4 bytes (float)
        vec4i, // 4 x 4 bytes (int)
        vec4b, // 4 x 1 bytes (byte)
    };

}

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace assetloader
{
    struct TextureDimention
    {
        U32 mWidth;
        U32 mHeight;
        U32 mDepth;
    };

    struct ModelDataFields
    {
        SwBool mHasNormals;
        SwBool mHasUvs;
        SwBool mHasColors;
        SwBool mHasBones;

        U8 mVertexOffset;
        U8 mNormalOffset;
        U8 mUvOffset;
        U8 mColorOffset;
        U8 mBoneWeightOffset;
        U8 mBoneIndexOffset;
    };

    struct DataOffset
    {
        DataType mType;
        U8 mOffset;
    };

    struct RequiredData
    {
        DataOffset mVertex;
        DataOffset mNormal;
        DataOffset mUv;
        DataOffset mColor;
        DataOffset mBoneWeight;
        DataOffset mBoneIndex;
    };

}

/* Class Declaration */

namespace assetloader
{
    class ILoadedMesh
    {
    public:
        virtual ~ILoadedMesh() {};

        virtual ModelDataFields getLoadedFileds() = 0;
        virtual U64 getStride() = 0;

        virtual U32 getNumMeshes() = 0;

        virtual void* getMeshVertexData(U32 subMesh, ModelInfo selectedData, U64& dataSize) = 0;
        virtual void* getMeshIndexData(U32 subMesh, ModelInfo selectedData, U64& dataSize) = 0;
    };

    class ILoadedTexture
    {
    public:
        virtual ~ILoadedTexture() {};

        virtual TextureDimention getDimentions() const = 0;
        virtual U8* getTextureData() const = 0;
    };

    class ILoadedSound
    {
    public:
        virtual ~ILoadedSound() {};
    };

    class ILoadedScene
    {
    public:
        virtual ~ILoadedScene() {};
    };

    class IAssetLoader
    {
    public:
        virtual ~IAssetLoader() { }

        virtual SwChar* getLoaderName() const = 0;

        virtual SwBool canLoadMesh(const std::filesystem::path filePath) = 0;
        virtual SwBool canLoadTexture(const std::filesystem::path filePath) = 0;
        virtual SwBool canLoadSound(const std::filesystem::path filePath) = 0;
        virtual SwBool canLoadScene(const std::filesystem::path filePath) = 0;

        virtual common::Result<LoadResult, ILoadedMesh> loadMesh(std::filesystem::path filePath, RequiredData requiredData) = 0;
        virtual common::Result<LoadResult, ILoadedTexture> loadTexture(std::filesystem::path filePath) = 0;

        virtual common::Result<LoadResult, ILoadedSound> loadSound(std::filesystem::path filePath) = 0;
        virtual common::Result<LoadResult, ILoadedScene> loadScene(std::filesystem::path filePath) = 0;

    };
}

/* Function Declaration */

#endif