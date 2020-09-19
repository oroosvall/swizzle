#ifndef MESH_HPP
#define MESH_HPP

#include <cstdint>

#pragma pack(push, 1)

static const char MESH_MAGIC[4] = {'S', 'W', 'M', ' '};

struct Vertex2d
{
    float x;
    float y;
};

struct Vertex3d
{
    float x;
    float y;
    float z;
};

struct Triangle16
{
    uint16_t v1;
    uint16_t v2;
    uint16_t v3;
};

struct Triangle32
{
    uint32_t v1;
    uint32_t v2;
    uint32_t v3;
};

struct MeshHeader
{
    char mMagic[4];
    uint8_t mMajor;
    uint8_t mMinor;
    uint8_t mVariant;
    uint8_t mPatch;
};

struct MeshFlags
{
    uint16_t mHasUv : 1;
    uint16_t mHasNormal : 1;
    uint16_t mReserved : 14;
};

struct CompressionFlags
{
    uint8_t mVertexIndex : 1;
    uint8_t mTriangleIndex : 1;
};

struct Mesh
{
    MeshFlags mFlags;
    uint16_t mNumVers;
    Vertex3d* mVertices;
    Vertex2d* mUvs;
    Vertex3d* mNormals;
    uint16_t mNumTriangles;
    Triangle16* mTriangles;
};

struct CompressedMesh
{
    CompressionFlags mCmpFlags;
    MeshFlags mFlags;
};

struct MeshDescr_v1_0_variant_0
{
    uint8_t mNumMeshes;
    Mesh* mMeshes;
};

struct MeshDescr_v1_0_variant_1
{
    uint8_t mNumMeshes;
    CompressedMesh* mMeshes;
};

#pragma pack(pop)

Mesh* loadMesh(char* filePath);

void saveMesh(char* filePath, Mesh* mesh);

void deleteMesh(Mesh* mesh);

#endif