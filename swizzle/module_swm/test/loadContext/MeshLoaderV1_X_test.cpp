
/* Include files */

#include <gtest/gtest.h>

#include "Load.hpp"
#include "LoadV1_x.hpp"

#include "mocks/LoaderMock.hpp"

namespace t = testing;

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

namespace meshLoaderV1_x
{
    TEST(SWM_MeshLoader, loadHeader_Success)
    {
        t::DefaultValue<swm::Result>::Set(swm::Result(-1));

        swm::types::Header hdr = {};
        mocks::LoaderIfcMock loadMock;
        mocks::VTLoaderIfcMock vtMock;
        swm::load::MeshLoaderV1_x ldr = { loadMock, vtMock, hdr };

        U8 count = 0u;
        const U8 output = 12;
        EXPECT_CALL(loadMock, loadNumber(t::Matcher<U8&>(count))).Times(1).WillOnce(
            t::DoAll(
                t::SetArgReferee<0>(output),
                t::Return(swm::Result::Success)
            )
        );

        U32 readCount = 0u;
        EXPECT_EQ(ldr.loadMeshHeader(readCount), swm::Result::Success);
        EXPECT_EQ(readCount, (U32)output);
    }

    TEST(SWM_MeshLoader, loadHeader_Failure)
    {
        t::DefaultValue<swm::Result>::Set(swm::Result(-1));

        swm::types::Header hdr = {};
        mocks::LoaderIfcMock loadMock;
        mocks::VTLoaderIfcMock vtMock;
        swm::load::MeshLoaderV1_x ldr = { loadMock, vtMock, hdr };

        U8 count = 0u;
        EXPECT_CALL(loadMock, loadNumber(t::Matcher<U8&>(count))).Times(1).WillOnce(
            t::Return(swm::Result::ErrFileIo)
        );

        U32 readCount = 0u;
        EXPECT_EQ(ldr.loadMeshHeader(readCount), swm::Result::ErrFileIo);
    }

    TEST(SWM_MeshLoader, loadData_StringFailure)
    {
        t::DefaultValue<swm::Result>::Set(swm::Result(-1));

        swm::types::Header hdr = {};
        mocks::LoaderIfcMock loadMock;
        mocks::VTLoaderIfcMock vtMock;
        swm::load::MeshLoaderV1_x ldr = { loadMock, vtMock, hdr };

        swm::types::Mesh m = {};

        EXPECT_CALL(loadMock, loadShortString(t::Matcher<std::string&>(t::_))).Times(1).WillOnce(
            t::Return(swm::Result::ErrFileIo)
        );

        swm::types::Mesh mesh = {};
        EXPECT_EQ(ldr.loadMeshData(mesh), swm::Result::ErrFileIo);
    }

    TEST(SWM_MeshLoader, loadData_FlagsFailure)
    {
        t::DefaultValue<swm::Result>::Set(swm::Result(-1));

        swm::types::Header hdr = {};
        mocks::LoaderIfcMock loadMock;
        mocks::VTLoaderIfcMock vtMock;
        swm::load::MeshLoaderV1_x ldr = { loadMock, vtMock, hdr };

        swm::types::Mesh m = {};

        EXPECT_CALL(loadMock, loadShortString(t::Matcher<std::string&>(t::_))).Times(1).WillOnce(
            t::DoAll(
                t::SetArgReferee<0>("Test"),
                t::Return(swm::Result::Success)
            )
        );

        EXPECT_CALL(loadMock, loadNumber(t::Matcher<U16&>(t::_))).Times(1).WillOnce(
            t::Return(swm::Result::ErrFileIo)
        );

        swm::types::Mesh mesh = {};
        EXPECT_EQ(ldr.loadMeshData(mesh), swm::Result::ErrFileIo);
        EXPECT_STREQ(mesh.mName.c_str(), "Test");
    }

    TEST(SWM_MeshLoader, loadData_VertexDataFailure)
    {
        t::DefaultValue<swm::Result>::Set(swm::Result(-1));

        swm::types::Header hdr = {};
        mocks::LoaderIfcMock loadMock;
        mocks::VTLoaderIfcMock vtMock;
        swm::load::MeshLoaderV1_x ldr = { loadMock, vtMock, hdr };

        swm::types::Mesh m = {};

        EXPECT_CALL(loadMock, loadShortString(t::Matcher<std::string&>(t::_))).Times(1).WillOnce(
            t::Return(swm::Result::Success)
        );

        EXPECT_CALL(loadMock, loadNumber(t::Matcher<U16&>(t::_))).Times(1).WillOnce(
            t::DoAll(
                t::SetArgReferee<0>(U16(0x12)),
                t::Return(swm::Result::Success)
            )
        );

        EXPECT_CALL(vtMock, loadVertexData(t::Matcher<swm::types::Mesh&>(t::_))).Times(1).WillOnce(
            t::Return(swm::Result::ErrFileIo)
        );

        swm::types::Mesh mesh = {};
        EXPECT_EQ(ldr.loadMeshData(mesh), swm::Result::ErrFileIo);
        EXPECT_EQ(mesh.mFlags, 0x12);
    }

    TEST(SWM_MeshLoader, loadData_TriangleDataFailure)
    {
        t::DefaultValue<swm::Result>::Set(swm::Result(-1));

        swm::types::Header hdr = {};
        mocks::LoaderIfcMock loadMock;
        mocks::VTLoaderIfcMock vtMock;
        swm::load::MeshLoaderV1_x ldr = { loadMock, vtMock, hdr };

        swm::types::Mesh m = {};

        EXPECT_CALL(loadMock, loadShortString(t::Matcher<std::string&>(t::_))).Times(1).WillOnce(
            t::Return(swm::Result::Success)
        );

        EXPECT_CALL(loadMock, loadNumber(t::Matcher<U16&>(t::_))).Times(1).WillOnce(
            t::Return(swm::Result::Success)
        );

        EXPECT_CALL(vtMock, loadVertexData(t::Matcher<swm::types::Mesh&>(t::_))).Times(1).WillOnce(
            t::Return(swm::Result::Success)
        );

        EXPECT_CALL(vtMock, loadTriangleData(t::Matcher<swm::types::Mesh&>(t::_))).Times(1).WillOnce(
            t::Return(swm::Result::ErrFileIo)
        );

        swm::types::Mesh mesh = {};
        EXPECT_EQ(ldr.loadMeshData(mesh), swm::Result::ErrFileIo);
    }

    TEST(SWM_MeshLoader, loadData_Success)
    {
        t::DefaultValue<swm::Result>::Set(swm::Result(-1));

        swm::types::Header hdr = {};
        mocks::LoaderIfcMock loadMock;
        mocks::VTLoaderIfcMock vtMock;
        swm::load::MeshLoaderV1_x ldr = { loadMock, vtMock, hdr };

        swm::types::Mesh m = {};

        EXPECT_CALL(loadMock, loadShortString(t::Matcher<std::string&>(t::_))).Times(1).WillOnce(
            t::Return(swm::Result::Success)
        );

        EXPECT_CALL(loadMock, loadNumber(t::Matcher<U16&>(t::_))).Times(1).WillOnce(
            t::Return(swm::Result::Success)
        );

        EXPECT_CALL(vtMock, loadVertexData(t::Matcher<swm::types::Mesh&>(t::_))).Times(1).WillOnce(
            t::Return(swm::Result::Success)
        );

        EXPECT_CALL(vtMock, loadTriangleData(t::Matcher<swm::types::Mesh&>(t::_))).Times(1).WillOnce(
            t::Return(swm::Result::Success)
        );

        swm::types::Mesh mesh = {};
        EXPECT_EQ(ldr.loadMeshData(mesh), swm::Result::Success);
    }

}


/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
