
/* Include files */

#include "SaveBaseTypes.hpp"
#include "SaveV1.hpp"

#include <utils/BitUtil.hpp>


/* Defines */

/* Typedefs */

typedef swm::SaveResult SR;

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swm_v1
{
    swm::SaveResult SaveV1(utils::BufferWriter& bw, const swm::Model& model, const swm::SaveOptions& options)
    {
        SR res = SR::Success;
        bw.write<U8>((U8)model.mMeshes.size());

        for (auto& mesh : model.mMeshes)
        {
            res = SaveMeshV1(bw, mesh, options);
            if (res != SR::Success)
            {
                break;
            }
        }

        return res;
    }

    swm::SaveResult SaveMeshV1(utils::BufferWriter& bw, const swm::Model::Mesh& mesh, const swm::SaveOptions& options)
    {
        SR res = swm_base::SaveString(bw, mesh.mName);

        U16 meshFlags = 0u;

        if (!mesh.mNormals.empty())
        {
            utils::SetBit(meshFlags, MESH_FLAGS_V1_NORMAL_BIT);
        }
        if (!mesh.mUvs.empty())
        {
            utils::SetBit(meshFlags, MESH_FLAGS_V1_UV_BIT);
        }
        if (!mesh.mVerColors.empty())
        {
            utils::SetBit(meshFlags, MESH_FLAGS_V1_COLOR_BIT);
        }

        bw.write(meshFlags);

        U32 vertCount = (U32)mesh.mPositions.size();
        bw.write(vertCount);

        bw.writeArray(mesh.mPositions.data(), vertCount);
        if (!mesh.mUvs.empty())
        {
            bw.writeArray(mesh.mUvs.data(), vertCount);
        }
        if (!mesh.mNormals.empty())
        {
            bw.writeArray(mesh.mNormals.data(), vertCount);
        }
        if (!mesh.mVerColors.empty())
        {
            bw.writeArray(mesh.mVerColors.data(), vertCount);
        }

        U32 triCount = (U32)mesh.mTriangles.size();
        bw.write(triCount);
        bw.writeArray(mesh.mTriangles.data(), triCount);

        return res;
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
