#ifndef LOADER_MOCK_HPP
#define LOADER_MOCK_HPP

/* Include files */

#include <gmock/gmock.h>

#include "Load.hpp"

#include <vector>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace mocks
{
    class LoaderIfcMock : public swm::load::LoaderCommonIfc
    {
    public:
        virtual ~LoaderIfcMock() = default;

        MOCK_METHOD(swm::Result, loadShortString, (std::string& str), (override));

        MOCK_METHOD(swm::Result, loadNumber, (U8& num), (override));
        MOCK_METHOD(swm::Result, loadNumber, (U16& num), (override));
        MOCK_METHOD(swm::Result, loadNumber, (U32& num), (override));

        MOCK_METHOD(swm::Result, loadArray, (std::vector<U8>& vec, size_t count), (override));
        MOCK_METHOD(swm::Result, loadArray, (std::vector<U16>& vec, size_t count), (override));
        MOCK_METHOD(swm::Result, loadArray, (std::vector<F32>& vec, size_t count), (override));

        MOCK_METHOD(swm::Result, loadArray, (std::vector<swm::types::Vector3F>& vec, size_t count), (override));
        MOCK_METHOD(swm::Result, loadArray, (std::vector<swm::types::Vector2F>& vec, size_t count), (override));
        MOCK_METHOD(swm::Result, loadArray, (std::vector<swm::types::Color4U>& vec, size_t count), (override));
        MOCK_METHOD(swm::Result, loadArray, (std::vector<swm::types::BoneIndex>& vec, size_t count), (override));
        MOCK_METHOD(swm::Result, loadArray, (std::vector<swm::types::BoneWeights>& vec, size_t count), (override));
        MOCK_METHOD(swm::Result, loadArray, (std::vector<swm::types::Triangle>& vec, size_t count), (override));

        MOCK_METHOD(swm::Result, loadArray, (std::vector<swm::types::Matrix4x4>& vec, size_t count), (override));
    };

    class VTLoaderIfcMock : public swm::load::VTLoaderIfc
    {
    public:
        virtual ~VTLoaderIfcMock() = default;

        MOCK_METHOD(swm::Result, loadVertexData, (swm::types::Mesh& mesh), (override));
        MOCK_METHOD(swm::Result, loadTriangleData, (swm::types::Mesh& mesh), (override));

        MOCK_METHOD(swm::Result, loadAnimationData, (swm::types::Mesh& mesh), (override));

        MOCK_METHOD(swm::Result, loadVertexDataCompressed, (swm::types::Mesh& mesh), (override));
        MOCK_METHOD(swm::Result, loadTriangleDataCompressed, (swm::types::Mesh& mesh), (override));
        MOCK_METHOD(swm::Result, loadAnimationDataCompressed, (swm::types::Mesh& mesh), (override));

    };

    class MeshLoaderIfcMock : public swm::load::MeshLoaderIfc
    {
    public:
        virtual ~MeshLoaderIfcMock() = default;

        MOCK_METHOD(swm::Result, loadMeshHeader, (U32& num), (override));
        MOCK_METHOD(swm::Result, loadMeshData, (swm::types::Mesh& mesh), (override));

    };
}

/* Function Declaration */

#endif
