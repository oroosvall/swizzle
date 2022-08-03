#ifndef SAVER_MOCKS_HPP
#define SAVER_MOCKS_HPP

/* Include files */

#include <gmock/gmock.h>

#include "Save.hpp"

#include <vector>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace mocks
{
    class SaverIfcMock : public swm::load::SaverCommonIfc
    {
    public:
        virtual ~SaverIfcMock() = default;

        MOCK_METHOD(swm::Result, saveShortString, (const std::string& str), (override));

        MOCK_METHOD(swm::Result, saveNumber, (const U8 num), (override));
        MOCK_METHOD(swm::Result, saveNumber, (const U16 num), (override));
        MOCK_METHOD(swm::Result, saveNumber, (const U32 num), (override));

        MOCK_METHOD(swm::Result, saveArray, (const std::vector<U8>& vec), (override));
        MOCK_METHOD(swm::Result, saveArray, (const std::vector<U16>& vec), (override));
        MOCK_METHOD(swm::Result, saveArray, (const std::vector<F32>& vec), (override));

        MOCK_METHOD(swm::Result, saveArray, (const std::vector<swm::types::Vector3F>& vec), (override));
        MOCK_METHOD(swm::Result, saveArray, (const std::vector<swm::types::Vector2F>& vec), (override));
        MOCK_METHOD(swm::Result, saveArray, (const std::vector<swm::types::Color4U>& vec), (override));
        MOCK_METHOD(swm::Result, saveArray, (const std::vector<swm::types::BoneIndex>& vec), (override));
        MOCK_METHOD(swm::Result, saveArray, (const std::vector<swm::types::BoneWeights>& vec), (override));
        MOCK_METHOD(swm::Result, saveArray, (const std::vector<swm::types::Triangle>& vec), (override));
        MOCK_METHOD(swm::Result, saveArray, (const std::vector<swm::types::Matrix4x4>& vec), (override));

        MOCK_METHOD(swm::Result, saveArray, (const U8* data, size_t count), (override));
    };

    class VTSaverIfcMock : public swm::save::VTSaverIfc
    {
    public:
        virtual ~VTSaverIfcMock() = default;

        MOCK_METHOD(swm::Result, saveVertexData, (const swm::types::Mesh& mesh, const U16 flags), (override));
        MOCK_METHOD(swm::Result, saveTriangleData, (const swm::types::Mesh& mesh, const U16 flags), (override));

        MOCK_METHOD(swm::Result, saveAnimationData, (const swm::types::Mesh& mesh), (override));

        MOCK_METHOD(swm::Result, saveVertexDataCompressed, (const swm::types::Mesh& mesh, const U16 flags), (overrqide));
        MOCK_METHOD(swm::Result, saveTriangleDataCompressed, (const swm::types::Mesh& mesh), (override));
        MOCK_METHOD(swm::Result, saveAnimationDataCompressed, (const swm::types::Mesh& mesh), (override));
    };
}

/* Function Declaration */

