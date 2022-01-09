
/* Include files */

#include "LoadV1.hpp"
#include "LoadBaseTypes.hpp"

#include <glm/glm.hpp>
#include <utils/BitUtil.hpp>

/* Defines */

/* Typedefs */

namespace swm_v1
{
    using LR = swm::LoadResult;
    using SVertex = swm::Vertex;
    using SUv = swm::Uv;
    using SColor = swm::Color;
    using STriangle = swm::Triangle;
}

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swm_v1
{
    swm::LoadResult LoadV1(U16 minorVer, utils::BufferReader& br, swm::Model& mdl)
    {
        UNUSED_ARG(minorVer);
        U8 numMeshes = 0u;

        swm::LoadResult res = swm_base::LoadNumber<U8, LR::SizeError>(br, numMeshes);

        if (swm_base::ResultIsOk(res))
        {
            for (S32 i = 0; (i < (S32)numMeshes) && swm_base::ResultIsOk(res); ++i)
            {
                swm::Model::Mesh mesh;
                Mesh_v1 m;
                res = LoadMeshV1(br, m);
                if (swm_base::ResultIsOk(res))
                {
                    mesh.mName = m.mName;

                    mesh.mPositions = std::move(m.mVertices);
                    mesh.mNormals = std::move(m.mNormals);
                    mesh.mUvs = std::move(m.mUvs);
                    mesh.mVerColors = std::move(m.mColors);

                    mesh.mTriangles = std::move(m.mTriangles);

                    mdl.mMeshes.emplace_back(mesh);
                }
            }
        }

        return res;
    }

    swm::LoadResult LoadMeshV1(utils::BufferReader& br, Mesh_v1& mesh)
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

            if (swm_base::ResultIsOk(res) && utils::IsBitSet(mesh.mMeshFlags, MESH_FLAGS_V1_UV_BIT))
            {
                res = swm_base::LoadArray<SUv, LR::VertexError>(br, mesh.mUvs, numVerts);
            }

            if (swm_base::ResultIsOk(res) && utils::IsBitSet(mesh.mMeshFlags, MESH_FLAGS_V1_NORMAL_BIT))
            {
                res = swm_base::LoadArray<SVertex, LR::VertexError>(br, mesh.mNormals, numVerts);
            }

            if (swm_base::ResultIsOk(res) && utils::IsBitSet(mesh.mMeshFlags, MESH_FLAGS_V1_COLOR_BIT))
            {
                res = swm_base::LoadArray<SColor, LR::VertexError>(br, mesh.mColors, numVerts);
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

        return res;
    }

}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
