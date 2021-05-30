#include "Mesh.hpp"

#include <fstream>
#include <string>
#include <vector>
#include <cstring>

Mesh* loadObj(const std::string& file)
{
    Mesh* m = nullptr;
    std::ifstream input(file);
    if (input.is_open())
    {
        std::vector<Vertex3d> vertices;
        std::vector<Triangle16> triangles;

        std::string line;
        while (!input.eof())
        {
            std::getline(input, line);

            size_t off0 = line.find_first_of(' ');
            size_t off1 = line.find_first_of(' ', off0 + 1);
            size_t off2 = line.find_first_of(' ', off1 + 1);
            size_t off3 = line.find_first_of(' ', off2 + 1);

            if (line[0] == 'v')
            {
                float v1 = std::strtof(line.substr(off0, off1 - off0).c_str(), nullptr);
                float v2 = std::strtof(line.substr(off1, off2 - off1).c_str(), nullptr);
                float v3 = std::strtof(line.substr(off2, off3 - off2).c_str(), nullptr);
                vertices.push_back({v1, v2, v3});
            }
            else if (line[0] == 'f')
            {
                int v1 = std::strtol(line.substr(off0, off1 - off0).c_str(), nullptr, 0);
                int v2 = std::strtol(line.substr(off1, off2 - off1).c_str(), nullptr, 0);
                int v3 = std::strtol(line.substr(off2, off3 - off2).c_str(), nullptr, 0);
                triangles.push_back({uint16_t(v1 - 1), uint16_t(v2 - 1), uint16_t(v3 - 1)});
            }
        }
        m = new Mesh();
        m->mFlags.mHasNormal = false;
        m->mFlags.mHasUv = false;
        m->mFlags.mReserved = 0;

        m->mNumVers = static_cast<uint16_t>(vertices.size());
        m->mNumTriangles = static_cast<uint16_t>(triangles.size());

        m->mVertices = new Vertex3d[m->mNumVers];
        m->mNormals = nullptr;
        m->mUvs = nullptr;
        m->mTriangles = new Triangle16[m->mNumTriangles];

        memcpy(m->mVertices, vertices.data(), sizeof(Vertex3d) * vertices.size());
        memcpy(m->mTriangles, triangles.data(), sizeof(Triangle16) * triangles.size());
    }
    return m;
}

Mesh* loadSwm(const std::string& file)
{
    Mesh* mesh = nullptr;
    std::ifstream input(file, std::ios::binary);
    if (input.is_open())
    {
        MeshHeader hdr;
        input.read((char*)&hdr, sizeof(MeshHeader));
        MeshDescr_v1_0_variant_0 descr;
        descr.mNumMeshes = 1;
        input.read((char*)&descr.mNumMeshes, sizeof(descr.mNumMeshes));
        mesh = new Mesh();

        input.read((char*)&mesh->mFlags, sizeof(mesh->mFlags));
        input.read((char*)&mesh->mNumVers, sizeof(mesh->mNumVers));
        mesh->mVertices = new Vertex3d[mesh->mNumVers];
        mesh->mUvs = nullptr;
        mesh->mNormals = nullptr;
        input.read((char*)mesh->mVertices, mesh->mNumVers * sizeof(Vertex3d));
        if (mesh->mFlags.mHasUv)
        {
            mesh->mUvs = new Vertex2d[mesh->mNumVers];
            input.read((char*)mesh->mUvs, mesh->mNumVers * sizeof(Vertex2d));
        }
        if (mesh->mFlags.mHasNormal)
        {
            mesh->mNormals = new Vertex3d[mesh->mNumVers];
            input.read((char*)mesh->mNormals, mesh->mNumVers * sizeof(Vertex3d));
        }
        input.read((char*)&mesh->mNumTriangles, sizeof(mesh->mNumTriangles));
        mesh->mTriangles = new Triangle16[mesh->mNumTriangles];
        input.read((char*)mesh->mTriangles, mesh->mNumTriangles * sizeof(Triangle16));
    }
    return mesh;
}

void saveSwm(const std::string& file, Mesh* mesh)
{
    std::ofstream outFile(file, std::ios::binary);
    if (outFile.is_open())
    {
        MeshHeader hdr;
        memcpy(hdr.mMagic, MESH_MAGIC, sizeof(MESH_MAGIC));
        hdr.mMajor = 1;
        hdr.mMinor = 0;
        hdr.mVariant = 0;
        hdr.mPatch = 0;
        outFile.write((char*)&hdr, sizeof(MeshHeader));

        MeshDescr_v1_0_variant_0 descr;
        descr.mNumMeshes = 1;
        outFile.write((char*)&descr.mNumMeshes, sizeof(descr.mNumMeshes));

        outFile.write((char*)&mesh->mFlags, sizeof(mesh->mFlags));
        outFile.write((char*)&mesh->mNumVers, sizeof(mesh->mNumVers));
        outFile.write((char*)mesh->mVertices, mesh->mNumVers * sizeof(Vertex3d));
        if (mesh->mFlags.mHasUv)
        {
            outFile.write((char*)mesh->mUvs, mesh->mNumVers * sizeof(Vertex2d));
        }
        if (mesh->mFlags.mHasNormal)
        {
            outFile.write((char*)mesh->mNormals, mesh->mNumVers * sizeof(Vertex3d));
        }
        outFile.write((char*)&mesh->mNumTriangles, sizeof(mesh->mNumTriangles));
        outFile.write((char*)mesh->mTriangles, mesh->mNumTriangles * sizeof(Triangle16));
    }
}

void saveObj(const std::string& file, Mesh* mesh)
{
    std::ofstream outFile(file, std::ios::binary);
    if (outFile.is_open())
    {
        outFile.write("o mdl\n", sizeof("o mdl\n") - 1);

        for (size_t i = 0; i < mesh->mNumVers; i++)
        {
            std::string v = "v " + std::to_string(mesh->mVertices[i].x) + " " + std::to_string(mesh->mVertices[i].y) +
                            " " + std::to_string(mesh->mVertices[i].z) + "\n";
            outFile.write(v.data(), v.size());
        }

        for (size_t i = 0; i < mesh->mNumTriangles; i++)
        {
            std::string f = "f " + std::to_string(mesh->mTriangles[i].v1 + 1) + " " +
                            std::to_string(mesh->mTriangles[i].v2 + 1) + " " +
                            std::to_string(mesh->mTriangles[i].v3 + 1) + "\n";
            outFile.write(f.data(), f.size());
        }
    }
}

Mesh* loadMesh(char* filePath)
{
    Mesh* mesh = nullptr;
    std::string file(filePath);

    if (file.find(".obj") != std::string::npos)
    {
        mesh = loadObj(file);
    }
    else if (file.find(".swm") != std::string::npos)
    {
        mesh = loadSwm(file);
    }

    return mesh;
}

void saveMesh(char* filePath, Mesh* mesh)
{
    std::string file(filePath);

    if (file.find(".swm") != std::string::npos)
    {
        saveSwm(file, mesh);
    }
    else if (file.find(".obj") != std::string::npos)
    {
        saveObj(file, mesh);
    }
}

void deleteMesh(Mesh* mesh)
{
    delete[] mesh->mVertices;
    delete[] mesh->mUvs;
    delete[] mesh->mNormals;
    delete[] mesh->mTriangles;

    delete mesh;
}