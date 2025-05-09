
/* Include files */

#include "SwmMeshAsset.hpp"
#include "AssetUtils.hpp"

#include <swm/Swm.hpp>
#include <swizzle/core/Logging.hpp>

#include <vector>
#include <map>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::asset2
{
    common::Resource<IMeshAsset> LoadSwmMesh(const SwChar* fileName, MeshAssetLoaderDescription& loadInfo)
    {
        swm::Model mdl;
        swm::Result res = swm::LoadSwm(fileName, mdl);

        common::Resource<IMeshAsset> loadedModel = nullptr;

        if (res == swm::Result::Success)
        {
            const auto& mesh = mdl.mMeshes.front();
            U64 vertexCount = mesh.mPositions.size();

            U64 totalAttributeSize = 0u;
            for (auto& info : loadInfo.mLoadPossitions)
            {
                totalAttributeSize = std::max(totalAttributeSize, info.mByteOffset + AttributeSize(info.mAttributeType));
            }

            U64 dataSize = totalAttributeSize * vertexCount;
            VertexData vertexData;
            vertexData.resize(dataSize);

            U64 stride = 0ull;
            for (auto& info : loadInfo.mLoadPossitions)
            {
                U8* src = nullptr;
                switch (info.mAttributeType)
                {
                case swizzle::asset2::AttributeTypes::VertexPosition:
                    src = (U8*)mesh.mPositions.data();
                    stride = sizeof(swm::types::Vector3F);
                    break;
                case swizzle::asset2::AttributeTypes::UvCoordinates:
                    if (mesh.mUvs.size() == 0u)
                    {
                        LOG_ERROR("Attempted to load uvs from file without uv information\n  Continuing, No data will be copied");
                        continue;
                    }
                    src = (U8*)mesh.mUvs.data();
                    stride = sizeof(swm::types::Vector2F);
                    break;
                case swizzle::asset2::AttributeTypes::NormalVector:
                    if (mesh.mNormals.size() == 0u)
                    {
                        LOG_ERROR("Attempted to load normals from file without normal information\n  Continuing, No data will be copied");
                        continue;
                    }
                    src = (U8*)mesh.mNormals.data();
                    stride = sizeof(swm::types::Vector3F);
                    break;
                case swizzle::asset2::AttributeTypes::BoneIndices:
                case swizzle::asset2::AttributeTypes::BoneWeights:
                    if (mesh.mBoneInfo.size() == 0u)
                    {
                        LOG_ERROR("Attempted to load bone info from file without skinning data\n  Continuing, No data will be copied");
                        continue;
                    }
                    src = (U8*)mesh.mBoneInfo.data();
                    stride = sizeof(swm::types::BoneInfo);
                    break;
                case swizzle::asset2::AttributeTypes::VertexColor:
                    if (mesh.mVertColors.size() == 0u)
                    {
                        LOG_ERROR("Attempted to load vertex color data from file without color data\n  Continuing, No data will be copied");
                        continue;
                    }
                    src = (U8*)mesh.mVertColors.data();
                    stride = sizeof(swm::types::Color4U);
                    break;
                default:
                    LOG_ERROR("Reached default state\n  Nothing to do");
                    continue;
                }

                if (src)
                {
                    for (U64 i = 0ull; i < vertexCount; ++i)
                    {
                        U64 size = AttributeSize(info.mAttributeType);
                        U8* dst = &vertexData.data()[i * totalAttributeSize];
                        U8* src2 = &src[i * stride];

                        struct NewIdx
                        {
                            U32 i1, i2, i3, i4;
                        } iddx = {};

                        struct NewCol
                        {
                            F32 c1, c2, c3, c4;
                        } coll = {};

                        if (info.mAttributeType == swizzle::asset2::AttributeTypes::BoneWeights)
                        {
                            src2 = &src2[sizeof(swm::types::BoneIndex)];
                        }
                        else if (info.mAttributeType == swizzle::asset2::AttributeTypes::BoneIndices)
                        {
                            swm::types::BoneIndex* bi = (swm::types::BoneIndex*)src2;
                            iddx.i1 = bi->b1;
                            iddx.i2 = bi->b2;
                            iddx.i3 = bi->b3;
                            iddx.i4 = bi->b4;
                            src2 = (U8*)&iddx;
                        }
                        else if (info.mAttributeType == swizzle::asset2::AttributeTypes::VertexColor)
                        {
                            swm::types::Color4U* bi = (swm::types::Color4U*)src2;
                            coll.c1 = (bi->r / 255.0f);
                            coll.c2 = (bi->g / 255.0f);
                            coll.c3 = (bi->b / 255.0f);
                            coll.c4 = (bi->a / 255.0f);
                            src2 = (U8*)&coll;
                        }

                        memcpy(&dst[info.mByteOffset], src2, size);
                    }
                }
            }

            U64 indexDataSize = mesh.mTriangles.size() * sizeof(swm::types::Triangle);

            IndexData indexData;
            indexData.resize(indexDataSize);

            memcpy(indexData.data(), mesh.mTriangles.data(), indexDataSize);

            AnimationDataInformation ani = {};
            ani.mNumBoneCount = mesh.mAnimationData.mNumBones;

            LoadAnimations(mesh, ani.mAnimations);

            loadedModel = common::CreateRef<MeshAsset>(vertexData, indexData, &ani);
        }
        else
        {
            LOG_ERROR("%s\n", swm::GetErrorDescription(res));
        }
        return loadedModel;
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
