#ifndef MODEL_HPP
#define MODEL_HPP

#include "Mesh.hpp"
#include <string>
#include <vector>

#include <fstream>

class Model
{
public:
    void load(const std::string& file);
    void save(const std::string& file, bool attemptCompress);

    struct Mesh
    {
        std::string mName;
        std::vector<Vertex3d> mVertices;
        std::vector<Vertex2d> mUvs;
        std::vector<Vertex3d> mNormals;

        std::vector<Triangle32> mTriangles;
    };

    std::vector<Mesh> mMeshes;

private:
    void loadObj(const std::string& file);
    void loadSwm(const std::string& file);

    void loadSwm1v0(std::ifstream& file, uint8_t variant);

    void saveObj(const std::string& file);
    void saveSwm(const std::string& file, bool attemptCompress);


    void Model::saveSwmComp(const std::string& file);
};

#endif