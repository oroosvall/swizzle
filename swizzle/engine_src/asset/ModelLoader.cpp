
/* Include files */

#include <common/Common.hpp>
#include <swizzle/asset/MeshLoader.hpp>

#include "loaders/ObjLoader.hpp"

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
        UNUSED_ARG(createIndexBuffer);
        Mesh m = {};

        m.mVertexBuffer = ctx->createBuffer(gfx::BufferType::Vertex);
        m.mIndexBuffer = ctx->createBuffer(gfx::BufferType::Index);

        F32* verts = nullptr;
        U32* idx = nullptr;

        U32 numVerts = 0u;
        U32 numTris = 0u;

        if (loader::obj::loadObjFile(fileName, &verts, numVerts, &idx, numTris))
        {
            m.mVertexBuffer->setBufferData(verts, numVerts * sizeof(loader::Vertex), sizeof(loader::Vertex));
            m.mIndexBuffer->setBufferData(idx, numTris * sizeof(loader::Index), sizeof(loader::Index));

            delete[] verts;
            delete[] idx;
        }

        return m;
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
