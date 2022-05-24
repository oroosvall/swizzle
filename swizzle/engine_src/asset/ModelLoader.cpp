
/* Include files */

#include <common/Common.hpp>
#include <swizzle/asset/MeshLoader.hpp>

#include "loaders/ObjLoader.hpp"
#include "loaders/SwmLoader.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::asset
{
    Mesh LoadMesh(common::Resource<gfx::GfxContext> ctx, const SwChar* fileName, SwBool createIndexBuffer)
    {
        Mesh m = {};

        m.mVertexBuffer = ctx->createBuffer(gfx::BufferType::Vertex);
        m.mIndexBuffer = ctx->createBuffer(gfx::BufferType::Index);

        F32* verts = nullptr;
        U32* idx = nullptr;
        F32* bone = nullptr;

        U32 numVerts = 0u;
        U32 numTris = 0u;
        U32 numBones = 0u;
        if (loader::swm::loadSwmFile(fileName, &verts, numVerts, &idx, numTris, &bone, numBones))
        {
            m.mVertexBuffer->setBufferData(verts, numVerts * sizeof(loader::Vertex), sizeof(loader::Vertex));
            m.mIndexBuffer->setBufferData(idx, numTris * sizeof(loader::Index), sizeof(loader::Index));

            delete[] verts;
            delete[] idx;
        }
        else if (loader::obj::loadObjFile(fileName, &verts, numVerts, &idx, numTris))
        {
            m.mVertexBuffer->setBufferData(verts, numVerts * sizeof(loader::Vertex), sizeof(loader::Vertex));
            m.mIndexBuffer->setBufferData(idx, numTris * sizeof(loader::Index), sizeof(loader::Index));

            delete[] verts;
            delete[] idx;
        }

        return m;
    }

    MeshAnimated LoadMeshAnimated(common::Resource<gfx::GfxContext> ctx, const SwChar* fileName, SwBool createIndexBuffer)
    {
        UNUSED_ARG(createIndexBuffer);
        MeshAnimated m = {};

        m.mVertexBuffer = ctx->createBuffer(gfx::BufferType::Vertex);
        m.mIndexBuffer = ctx->createBuffer(gfx::BufferType::Index);
        m.mBoneBuffer= ctx->createBuffer(gfx::BufferType::UniformBuffer);

        F32* verts = nullptr;
        U32* idx = nullptr;

        F32* bone = nullptr;

        U32 numVerts = 0u;
        U32 numTris = 0u;
        U32 numBones = 0u;
        if (loader::swm::loadSwmFile(fileName, &verts, numVerts, &idx, numTris, &bone, numBones))
        {
            m.mVertexBuffer->setBufferData(verts, numVerts * sizeof(loader::VertexAnim), sizeof(loader::VertexAnim));
            m.mIndexBuffer->setBufferData(idx, numTris * sizeof(loader::Index), sizeof(loader::Index));
            //glm::mat4 bones[100];
            //for (auto& b : bones)
            //{
            //    b = glm::mat4(1.0);
            //}
            m.mBoneBuffer->setBufferData(bone, sizeof(glm::mat4) * numBones, sizeof(glm::mat4));
            //m.mBoneBuffer->setBufferData(bones, sizeof(bones), sizeof(glm::mat4));

            delete[] verts;
            delete[] idx;
            delete[] bone;
        }

        return m;
    }

}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
