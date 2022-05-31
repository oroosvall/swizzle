
/* Include files */

#include "ObjMeshAsset.hpp"

#include <swizzle/core/Logging.hpp>

#include <vector>
#include <map>
#include <fstream>

/* Defines */

/* Typedefs */

/* Structs/Classes */

namespace swizzle::asset2::_int
{
    struct Vertex2D
    {
        F32 u;
        F32 v;
    };

    struct Vertex3D
    {
        F32 x;
        F32 y;
        F32 z;
    };

    struct Int4D
    {
        U32 i1;
        U32 i2;
        U32 i3;
        U32 i4;
    };

    struct Weight4D
    {
        F32 w1;
        F32 w2;
        F32 w3;
        F32 w4;
    };

    struct Vertex
    {
        Vertex3D pos;
        Vertex3D norm;
        Vertex2D uv;
    };

    struct VertexAnim
    {
        Vertex3D pos;
        Vertex3D norm;
        Vertex2D uv;
        Int4D idx;
        Weight4D wgt;
    };

    struct Index
    {
        U32 i1;
        U32 i2;
        U32 i3;
    };
}

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::asset2
{
    common::Resource<IMeshAsset> LoadObjMesh(const SwChar* fileName, MeshAssetLoaderDescription& loadInfo)
    {
        UNUSED_ARG(loadInfo);

        common::Resource<IMeshAsset> loadedModel = nullptr;

        std::ifstream input(fileName);
        if (input.is_open())
        {
            std::string line;
            //uint32_t offset = 0u;
            //uint32_t loadCounter = 1u;
            LOG_INFO("Loading obj file %s\n", fileName);

            std::vector<_int::Vertex3D> verts;
            std::vector<_int::Vertex3D> normals;
            std::vector<_int::Vertex2D> uvs;
            std::vector<_int::Vertex> combinedVerts;
            std::vector<_int::Index> tris;

            bool hasNormal = false;
            bool hasUv = false;

            while (!input.eof())
            {
                std::getline(input, line);

                size_t off0 = line.find_first_of(' ');
                size_t off1 = line.find_first_of(' ', off0 + 1);
                size_t off2 = line.find_first_of(' ', off1 + 1);
                size_t off3 = line.find_first_of(' ', off2 + 1);

                if (line[0] == 'v' && line[1] == 't')
                {
                    float v1 = std::strtof(line.substr(off0, off1 - off0).c_str(), nullptr);
                    float v2 = std::strtof(line.substr(off1, off2 - off1).c_str(), nullptr);
                    uvs.push_back({ v1, v2 });
                    hasUv = true;
                }
                else if (line[0] == 'v' && line[1] == 'n')
                {
                    float v1 = std::strtof(line.substr(off0, off1 - off0).c_str(), nullptr);
                    float v2 = std::strtof(line.substr(off1, off2 - off1).c_str(), nullptr);
                    float v3 = std::strtof(line.substr(off2, off3 - off2).c_str(), nullptr);
                    normals.push_back({ v1, v2, v3 });
                    hasNormal = true;
                }
                else if (line[0] == 'v' && line[1] == ' ')
                {
                    float v1 = std::strtof(line.substr(off0, off1 - off0).c_str(), nullptr);
                    float v2 = std::strtof(line.substr(off1, off2 - off1).c_str(), nullptr);
                    float v3 = std::strtof(line.substr(off2, off3 - off2).c_str(), nullptr);
                    verts.push_back({ v1, v2, v3 });
                }
                else if (line[0] == 'f')
                {

                    _int::Vertex vert1 = {};
                    _int::Vertex vert2 = {};
                    _int::Vertex vert3 = {};

                    std::string line_ll_1 = line.substr(off0, off1 - off0);
                    std::string line_ll_2 = line.substr(off1, off2 - off1);
                    std::string line_ll_3 = line.substr(off2, off3 - off2);

                    U32 v1 = std::strtoul(line_ll_1.c_str(), nullptr, 0);
                    U32 v2 = std::strtoul(line_ll_2.c_str(), nullptr, 0);
                    U32 v3 = std::strtoul(line_ll_3.c_str(), nullptr, 0);

                    vert1.pos = verts[v1 - 1u];
                    vert2.pos = verts[v2 - 1u];
                    vert3.pos = verts[v3 - 1u];

                    if (hasNormal)
                    {
                        size_t off0_n_1 = line_ll_1.find_first_of('/') + 1;
                        size_t off1_n_1 = line_ll_1.find_first_of('/', off0_n_1 + 1) + 1;
                        size_t off2_n_1 = line_ll_1.find_first_of('/', off1_n_1 + 1) + 1;

                        size_t off0_n_2 = line_ll_2.find_first_of('/') + 1;
                        size_t off1_n_2 = line_ll_2.find_first_of('/', off0_n_2 + 1) + 1;
                        size_t off2_n_2 = line_ll_2.find_first_of('/', off1_n_2 + 1) + 1;

                        size_t off0_n_3 = line_ll_3.find_first_of('/') + 1;
                        size_t off1_n_3 = line_ll_3.find_first_of('/', off0_n_3 + 1) + 1;
                        size_t off2_n_3 = line_ll_3.find_first_of('/', off1_n_3 + 1) + 1;

                        std::string line_ll_1_n = line_ll_1.substr(off1_n_1, off2_n_1 - off1_n_1);
                        std::string line_ll_2_n = line_ll_2.substr(off1_n_2, off2_n_2 - off1_n_2);
                        std::string line_ll_3_n = line_ll_3.substr(off1_n_3, off2_n_3 - off1_n_3);

                        U32 n1 = std::strtoul(line_ll_1_n.c_str(), nullptr, 0);
                        U32 n2 = std::strtoul(line_ll_2_n.c_str(), nullptr, 0);
                        U32 n3 = std::strtoul(line_ll_3_n.c_str(), nullptr, 0);

                        vert1.norm = normals[n1 - 1u];
                        vert2.norm = normals[n2 - 1u];
                        vert3.norm = normals[n3 - 1u];

                    }

                    if (hasUv)
                    {
                        size_t off0_u_1 = line_ll_1.find_first_of('/') + 1;
                        size_t off1_u_1 = line_ll_1.find_first_of('/', off0_u_1 + 1) + 1;

                        size_t off0_u_2 = line_ll_2.find_first_of('/') + 1;
                        size_t off1_u_2 = line_ll_2.find_first_of('/', off0_u_2 + 1) + 1;

                        size_t off0_u_3 = line_ll_3.find_first_of('/') + 1;
                        size_t off1_u_3 = line_ll_3.find_first_of('/', off0_u_3 + 1) + 1;

                        std::string line_ll_1_u = line_ll_1.substr(off0_u_1, off1_u_1 - off0_u_1);
                        std::string line_ll_2_u = line_ll_2.substr(off0_u_2, off1_u_2 - off0_u_2);
                        std::string line_ll_3_u = line_ll_3.substr(off0_u_3, off1_u_3 - off0_u_3);

                        U32 u1 = std::strtoul(line_ll_1_u.c_str(), nullptr, 0);
                        U32 u2 = std::strtoul(line_ll_2_u.c_str(), nullptr, 0);
                        U32 u3 = std::strtoul(line_ll_3_u.c_str(), nullptr, 0);

                        vert1.uv = uvs[u1 - 1u];
                        vert2.uv = uvs[u2 - 1u];
                        vert3.uv = uvs[u3 - 1u];

                    }

                    combinedVerts.push_back(vert1);
                    combinedVerts.push_back(vert2);
                    combinedVerts.push_back(vert3);

                    size_t len = combinedVerts.size();

                    tris.push_back({ uint32_t(len - 3U), uint32_t(len - 2U), uint32_t(len - 1U) });
                }
            }

            IndexData vertexData;
            vertexData.resize(combinedVerts.size() * sizeof(_int::Vertex));
            memcpy(vertexData.data(), combinedVerts.data(), combinedVerts.size() * sizeof(_int::Vertex));

            IndexData indexData;
            indexData.resize(tris.size() * sizeof(_int::Index));
            memcpy(indexData.data(), tris.data(), tris.size() * sizeof(_int::Index));

            loadedModel = common::CreateRef<MeshAsset>(vertexData, indexData, nullptr);
        }
        return loadedModel;
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
