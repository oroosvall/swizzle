#include "Model.hpp"

#include <fstream>
#include <utils/Bitstream.hpp>
#include <utils/BitUtil.hpp>

#include <map>
#include <vector>

#include <swizzle/core/Logging.hpp>

void Model::load(const std::string& file)
{
    if (file.find(".obj") != std::string::npos)
    {
        loadObj(file);
    }
    else if (file.find(".swm") != std::string::npos)
    {
        loadSwm(file);
    }
}

void Model::save(const std::string& file, bool attemptCompress)
{
    if (file.find(".swm") != std::string::npos)
    {
        saveSwm(file, attemptCompress);
    }
    else if (file.find(".obj") != std::string::npos)
    {
        saveObj(file);
    }
}



void Model::loadObj(const std::string& file)
{
    std::ifstream input(file);
    if (input.is_open())
    {
        std::string line;
        Model::Mesh mesh;
        uint32_t offset = 0;
        uint32_t loadCounter = 1;
        LOG_INFO("Loading obj file\n");

        std::vector<Vertex3d> verts;
        std::vector<Vertex3d> normals;
        std::vector<Vertex2d> uvs;

        bool hasNormal = false;
        bool hasUv = false;

        while (!input.eof())
        {
            std::getline(input, line);

            size_t off0 = line.find_first_of(' ');
            size_t off1 = line.find_first_of(' ', off0 + 1);
            size_t off2 = line.find_first_of(' ', off1 + 1);
            size_t off3 = line.find_first_of(' ', off2 + 1);

            if (line[0] == 'o')
            {
                if (mMeshes.size() >= 1)
                {
                    offset += static_cast<uint32_t>(mMeshes.back().mVertices.size());
                    //Model::Mesh& m = mMeshes.back();
                    LOG_INFO("\r loading mesh %d", loadCounter++);
                    //printf("%s loaded with %zd vetecies, and %zd triangles\n", m.mName.c_str(), m.mVertices.size(),
                    //       m.mTriangles.size());
                }
                mMeshes.emplace_back(Model::Mesh());
                std::string s = line.substr(off0 + 1);
                mMeshes.back().mName = s;
                mMeshes.back().mName.push_back('\0');
            }
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
                verts.push_back({v1, v2, v3});
            }
            else if (line[0] == 'f')
            {
                std::string line_ll_1 = line.substr(off0, off1 - off0);
                std::string line_ll_2 = line.substr(off1, off2 - off1);
                std::string line_ll_3 = line.substr(off2, off3 - off2);

                int v1 = std::strtol(line_ll_1.c_str(), nullptr, 0);
                int v2 = std::strtol(line_ll_2.c_str(), nullptr, 0);
                int v3 = std::strtol(line_ll_3.c_str(), nullptr, 0);

                mMeshes.back().mVertices.push_back(verts[uint32_t(v1 - offset - 1)]);
                mMeshes.back().mVertices.push_back(verts[uint32_t(v2 - offset - 1)]);
                mMeshes.back().mVertices.push_back(verts[uint32_t(v3 - offset - 1)]);

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

                    int n1 = std::strtol(line_ll_1_n.c_str(), nullptr, 0);
                    int n2 = std::strtol(line_ll_2_n.c_str(), nullptr, 0);
                    int n3 = std::strtol(line_ll_3_n.c_str(), nullptr, 0);

                    mMeshes.back().mNormals.push_back(normals[uint32_t(n1 - offset - 1)]);
                    mMeshes.back().mNormals.push_back(normals[uint32_t(n2 - offset - 1)]);
                    mMeshes.back().mNormals.push_back(normals[uint32_t(n3 - offset - 1)]);
                }
                else
                {
                    mMeshes.back().mNormals.push_back({});
                    mMeshes.back().mNormals.push_back({});
                    mMeshes.back().mNormals.push_back({});
                }
                
                if (hasUv)
                {
                    size_t off0_u_1 = line_ll_1.find_first_of('/') + 1;
                    size_t off1_u_1 = line_ll_1.find_first_of('/', off0_u_1 + 1) + 1;

                    size_t off0_u_2 = line_ll_2.find_first_of('/') + 1;
                    size_t off1_u_2 = line_ll_2.find_first_of('/', off0_u_2 + 1) + 1;

                    size_t off0_u_3 = line_ll_3.find_first_of('/') + 1;
                    size_t off1_u_3 = line_ll_3.find_first_of('/', off0_u_3 + 1) + 1;

                    std::string line_ll_1_n = line_ll_1.substr(off0_u_1, off1_u_1 - off0_u_1);
                    std::string line_ll_2_n = line_ll_2.substr(off0_u_2, off1_u_2 - off0_u_2);
                    std::string line_ll_3_n = line_ll_3.substr(off0_u_3, off1_u_3 - off0_u_3);

                    int u1 = std::strtol(line_ll_1.c_str(), nullptr, 0);
                    int u2 = std::strtol(line_ll_2.c_str(), nullptr, 0);
                    int u3 = std::strtol(line_ll_3.c_str(), nullptr, 0);

                    mMeshes.back().mUvs.push_back(uvs[uint32_t(u1 - offset - 1)]);
                    mMeshes.back().mUvs.push_back(uvs[uint32_t(u2 - offset - 1)]);
                    mMeshes.back().mUvs.push_back(uvs[uint32_t(u3 - offset - 1)]);
                }
                else
                {
                    mMeshes.back().mUvs.push_back({});
                    mMeshes.back().mUvs.push_back({});
                    mMeshes.back().mUvs.push_back({});
                }

                size_t len = mMeshes.back().mVertices.size();
                mMeshes.back().mTriangles.push_back(
                    { uint32_t(len - 3U), uint32_t(len - 2U), uint32_t(len - 1U) }
                );

                //mMeshes.back().mTriangles.push_back(
                //    {uint32_t(v1 - offset - 1), uint32_t(v2 - offset - 1), uint32_t(v3 - offset - 1)});
            }
        }
        printf("\n");
    }
}

void Model::loadSwm(const std::string& file)
{

    std::ifstream input(file, std::ios::binary);
    if (input.is_open())
    {
        MeshHeader hdr;
        input.read((char*)&hdr, sizeof(MeshHeader));

        if (hdr.mMajor == 1 && hdr.mMinor == 0)
        {
            loadSwm1v0(input, hdr.mVariant);
        }
    }
}

void Model::loadSwm1v0(std::ifstream& file, uint8_t variant)
{
    CompressionFlags compressFlags = {0};

    MeshDescr_v1_0_variant_1 descr;
    file.read((char*)&descr.mNumMeshes, sizeof(descr.mNumMeshes));

    LOG_INFO("Loading %d number of meshes\n", descr.mNumMeshes);
    for (size_t i = 0; i < descr.mNumMeshes; i++)
    {
        LOG_INFO("\r loading mesh %zd", i);
        mMeshes.emplace_back(Model::Mesh());

        Model::Mesh& m = mMeshes.back();
        MeshFlags meshFlags;

        uint32_t nameLen = 0;
        file.read((char*)&nameLen, sizeof(nameLen));
        m.mName.resize(nameLen);
        file.read(m.mName.data(), nameLen);

        if (variant == 1)
        {
            file.read((char*)&compressFlags, sizeof(compressFlags));
        }
        file.read((char*)&meshFlags, sizeof(meshFlags));

        if (compressFlags.mVertexIndex)
        {
            uint32_t numValues = 0;
            file.read((char*)&numValues, sizeof(numValues));

            float* values = new float[numValues];

            file.read((char*)values, sizeof(float) * numValues);

            uint8_t bpi = 0;
            uint32_t numVertices = 0;
            file.read((char*)&bpi, sizeof(bpi));
            file.read((char*)&numVertices, sizeof(numVertices));

            uint32_t dataSize = 3; // vertex position
            if (meshFlags.mHasUv)
                dataSize += 2; // vertex uv
            if (meshFlags.mHasNormal)
                dataSize += 3; // vertex uv

            size_t size = ((dataSize * numVertices * bpi) + 7) / 8;

            uint8_t* data = (uint8_t*)malloc(size);
            file.read((char*)data, size);

            utils::BitStreamReader bitStream(data, size);
            m.mVertices.reserve(numVertices);

            for (size_t j = 0; j < numVertices; j++)
            {
                uint32_t x, y, z;
                bitStream.readBits(x, bpi);
                bitStream.readBits(y, bpi);
                bitStream.readBits(z, bpi);
                m.mVertices.push_back({values[x], values[y], values[z]});
            }

            if (meshFlags.mHasUv)
            {
                m.mUvs.reserve(numVertices);
                for (size_t j = 0; j < numVertices; j++)
                {
                    uint32_t u, v;
                    bitStream.readBits(u, bpi);
                    bitStream.readBits(v, bpi);
                    m.mUvs.push_back({values[u], values[v]});
                }
            }

            if (meshFlags.mHasUv)
            {
                m.mNormals.reserve(numVertices);
                for (size_t j = 0; j < numVertices; j++)
                {
                    uint32_t x, y, z;
                    bitStream.readBits(x, bpi);
                    bitStream.readBits(y, bpi);
                    bitStream.readBits(z, bpi);
                    m.mNormals.push_back({values[x], values[y], values[z]});
                }
            }

            free(data);
            delete[] values;
        }
        else
        {
            uint32_t numVerts = 0;
            file.read((char*)&numVerts, sizeof(numVerts));

            m.mVertices.resize(numVerts);

            file.read((char*)m.mVertices.data(), m.mVertices.size() * sizeof(Vertex3d));
            if (meshFlags.mHasUv)
            {
                m.mUvs.resize(numVerts);
                file.read((char*)m.mUvs.data(), m.mUvs.size() * sizeof(Vertex2d));
            }
            if (meshFlags.mHasNormal)
            {
                m.mNormals.resize(numVerts);
                file.read((char*)m.mNormals.data(), m.mNormals.size() * sizeof(Vertex3d));
            }
        }

        if (compressFlags.mTriangleIndex)
        {
            uint32_t numTriangles = 0;
            uint8_t bitsPerTriangle = 0;
            uint8_t* data = nullptr;

            file.read((char*)&bitsPerTriangle, sizeof(bitsPerTriangle));
            file.read((char*)&numTriangles, sizeof(numTriangles));

            size_t readSize = ((numTriangles * 3U * bitsPerTriangle) + 7 ) / 8;

            data = (uint8_t*)malloc(readSize);
            file.read((char*)data, readSize);

            utils::BitStreamReader bitStream(data, readSize);

            for (size_t j = 0; j < numTriangles; j++)
            {
                uint32_t v1, v2, v3;
                bitStream.readBits(v1, bitsPerTriangle);
                bitStream.readBits(v2, bitsPerTriangle);
                bitStream.readBits(v3, bitsPerTriangle);

                m.mTriangles.push_back({v1, v2, v3});
            }

            free(data);
        }
        else
        {
            uint32_t numTriangles = 0;

            file.read((char*)&numTriangles, sizeof(numTriangles));

            m.mTriangles.resize(numTriangles);
            file.read((char*)m.mTriangles.data(), m.mTriangles.size() * sizeof(Triangle32));
        }
    }
    LOG_INFO("\n");

}

void Model::saveObj(const std::string& file)
{
    std::ofstream outFile(file, std::ios::binary);
    if (outFile.is_open())
    {
        uint32_t offset = 0;
        uint32_t writeCounter = 0;
        LOG_INFO("Saving obj file\n");
        for (const auto& m : mMeshes)
        {
            LOG_INFO("\r writing mesh %d", writeCounter++);
            outFile.write("o ", sizeof("o ") - 1);
            outFile.write(m.mName.c_str(), m.mName.size() - 1);
            outFile.write("\n", sizeof("\n") - 1);

            for (size_t i = 0; i < m.mVertices.size(); i++)
            {
                Vertex3d vert = m.mVertices[i];
                std::string v =
                    "v " + std::to_string(vert.x) + " " + std::to_string(vert.y) + " " + std::to_string(vert.z) + "\n";
                outFile.write(v.data(), v.size());
            }

            for (size_t i = 0; i < m.mTriangles.size(); i++)
            {
                Triangle32 tri = m.mTriangles[i];
                std::string f = "f " + std::to_string(tri.v1 + offset + 1) + " " + std::to_string(tri.v2 + offset + 1) +
                                " " + std::to_string(tri.v3 + offset + 1) + "\n";
                outFile.write(f.data(), f.size());
            }

            offset += static_cast<uint32_t>(m.mVertices.size());
        }
        LOG_INFO("\n");
    }
}

uint8_t* getModelVertexData(const Model::Mesh& mesh, CompressionFlags& compressed, size_t& outSize)
{
    const size_t vertexSize = mesh.mVertices.size() * sizeof(Vertex3d);
    const size_t normalSize = mesh.mNormals.size() * sizeof(Vertex3d);
    const size_t uvSize = mesh.mUvs.size() * sizeof(Vertex2d);
    const size_t size = vertexSize + normalSize + uvSize + sizeof(uint32_t);
    
    uint32_t index = 0U;
    std::map<float, uint32_t> vertexFloatMap;

    for (const auto& vert : mesh.mVertices)
    {
        float f = vert.x;
        if (vertexFloatMap.find(f) == vertexFloatMap.end())
        {
            vertexFloatMap[f] = index++;
        }
        f = vert.y;
        if (vertexFloatMap.find(f) == vertexFloatMap.end())
        {
            vertexFloatMap[f] = index++;
        }
        f = vert.z;
        if (vertexFloatMap.find(f) == vertexFloatMap.end())
        {
            vertexFloatMap[f] = index++;
        }
    }

    uint8_t numBitsVerts = utils::bits_needed(static_cast<uint32_t>(vertexFloatMap.size() - 1U));

    size_t compressedSize = (vertexFloatMap.size() * sizeof(float)) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint8_t);
    compressedSize += (((mesh.mVertices.size() * 3U * (size_t)numBitsVerts) + 7U) / 8U);

    uint8_t* data = nullptr;

    if (compressedSize < size && compressed.mVertexIndex == 1U)
    {
        data = new uint8_t[compressedSize];
        uint32_t* numFloats = (uint32_t*)data;
        *numFloats = static_cast<uint32_t>(vertexFloatMap.size());
        
        size_t offset = sizeof(uint32_t);
        float* floats = (float*)(data + offset);
        for (const auto& f : vertexFloatMap)
        {
            floats[f.second] = f.first;
        }

        offset += sizeof(float) * vertexFloatMap.size();

        uint8_t* bpv = (uint8_t*)(data + offset);
        *bpv = numBitsVerts;
        offset += sizeof(uint8_t);

        uint32_t* numVerts = (uint32_t*)(data + offset);
        *numVerts = static_cast<uint32_t>(mesh.mVertices.size());
        offset += sizeof(uint32_t);

        utils::BitStreamWriter bsw(data + offset, compressedSize - offset);

        for (const auto& vert : mesh.mVertices)
        {
            float f = vert.x;
            auto idx = vertexFloatMap[f];
            bsw.writeBits(idx, numBitsVerts);
            f = vert.y;
            idx = vertexFloatMap[f];
            bsw.writeBits(idx, numBitsVerts);
            f = vert.z;
            idx = vertexFloatMap[f];
            bsw.writeBits(idx, numBitsVerts);
        }

        bsw.flush();
        offset += bsw.getWrittenSize();
        outSize = offset;
    }
    else
    {
        compressed.mVertexIndex = 0U;
        uint32_t numVerts = (uint32_t)mesh.mVertices.size();

        data = new uint8_t[size];

        memcpy(data, &numVerts, sizeof(uint32_t));
        size_t offset = sizeof(uint32_t);
        memcpy(data + offset, mesh.mVertices.data(), vertexSize);
        offset += vertexSize;
        memcpy(data + offset, mesh.mNormals.data(), normalSize);
        offset += normalSize;
        memcpy(data + offset, mesh.mUvs.data(), uvSize);

        outSize = size;
    }

    return data;
}

uint8_t* getModelTriangleData(const Model::Mesh& mesh, CompressionFlags& compressed, size_t& outSize)
{
    if (compressed.mTriangleIndex == 1U)
    {
        uint32_t numVertices = (uint32_t)mesh.mVertices.size();
        uint32_t numTriangles = (uint32_t)mesh.mTriangles.size();

        uint8_t numBits = utils::bits_needed(numVertices - 1);

        const size_t size = ((((size_t)numTriangles * (size_t)numBits * 3U) + 7U) / 8U) + 10U;

        uint8_t* data = new uint8_t[size];

        memcpy(data, &numBits, sizeof(uint8_t));
        memcpy(data + sizeof(uint8_t), &numTriangles, sizeof(uint32_t));

        utils::BitStreamWriter bsw(data + 5U, size);

        for (const auto& t : mesh.mTriangles)
        {
            bsw.writeBits(t.v1, numBits);
            bsw.writeBits(t.v2, numBits);
            bsw.writeBits(t.v3, numBits);
        }

        bsw.flush();

        outSize = bsw.getWrittenSize() + 5U;

        return data;
    }
    else
    {
        const size_t size = (mesh.mTriangles.size() * sizeof(Triangle32)) + sizeof(uint32_t);
        uint8_t* data = new uint8_t[size];

        uint32_t numTris = static_cast<uint32_t>(mesh.mTriangles.size());

        memcpy(data, &numTris, sizeof(uint32_t));
        memcpy(data + sizeof(uint32_t), mesh.mTriangles.data(), size - sizeof(uint32_t));

        outSize = size;
        return data;
    }
}

void Model::saveSwm(const std::string& file, bool attemptCompression)
{
    std::ofstream outFile(file, std::ios::binary);
    if (outFile.is_open())
    {
        MeshHeader hdr;
        memcpy(hdr.mMagic, MESH_MAGIC, sizeof(MESH_MAGIC));
        hdr.mMajor = 1;
        hdr.mMinor = 0;
        hdr.mVariant = attemptCompression;
        hdr.mPatch = 0;
        outFile.write((char*)&hdr, sizeof(MeshHeader));

        MeshDescr_v1_0_variant_0 descr;
        descr.mNumMeshes = static_cast<uint8_t>(mMeshes.size());
        outFile.write((char*)&descr.mNumMeshes, sizeof(descr.mNumMeshes));
        
        LOG_INFO("Saving compressed swm file\n");
        uint32_t writeCounter = 1;
        for (const auto& m : mMeshes)
        {
            LOG_INFO("\r writing mesh %d", writeCounter++);
            uint32_t nameLength = static_cast<uint32_t>(m.mName.size());
            outFile.write((char*)&nameLength, sizeof(nameLength));
            outFile.write((char*)m.mName.c_str(), nameLength);

            CompressionFlags cf{ 0 };
            if (attemptCompression)
            {
                cf.mVertexIndex = 1;
                cf.mTriangleIndex = 1;
            }
            MeshFlags mf{0};

            size_t vertexSize = 0U;
            size_t triangleSize = 0U;

            uint8_t* vertexData = getModelVertexData(m, cf, vertexSize);
            uint8_t* triangleData = getModelTriangleData(m, cf, triangleSize);
            
            mf.mHasUv = m.mUvs.size() != 0;
            mf.mHasNormal = m.mNormals.size() != 0;


            outFile.write((char*)&cf, sizeof(cf));
            outFile.write((char*)&mf, sizeof(mf));

            outFile.write((char*)vertexData, vertexSize);
            outFile.write((char*)triangleData, triangleSize);
            
            delete[] vertexData;
            delete[] triangleData;

        }
        LOG_INFO("\n");

    }
}