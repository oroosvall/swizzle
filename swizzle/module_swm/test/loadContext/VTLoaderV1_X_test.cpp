
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

namespace vtLoaderV1_x
{
    TEST(SWM_VtLoader, loadVertexData_Success)
    {
        t::DefaultValue<swm::Result>::Set(swm::Result(-1));

        swm::types::Header hdr = {};
        t::StrictMock<mocks::LoaderIfcMock> loadMock;
        swm::load::VTLoaderV1_x ldr = {loadMock, hdr};

        swm::types::Mesh m = {};

        U32 count = 0u;
        const U32 output = 12;
        EXPECT_CALL(loadMock, loadNumber(count))
            .Times(1)
            .WillOnce(t::DoAll(t::SetArgReferee<0>(output), t::Return(swm::Result::Success)));

        // Only expect that loadArray is called once (only for vertex positionns)
        EXPECT_CALL(loadMock,
                    loadArray(t::Matcher<std::vector<swm::types::Vector3F>&>(t::_), t::Matcher<size_t>(output)))
            .Times(1)
            .WillOnce(t::Return(swm::Result::Success));

        // This only loads vertex position data
        EXPECT_EQ(ldr.loadVertexData(m), swm::Result::Success);
    }

    TEST(SWM_VtLoader, loadTriangleData_Success)
    {
        t::DefaultValue<swm::Result>::Set(swm::Result(-1));

        swm::types::Header hdr = {};
        t::StrictMock<mocks::LoaderIfcMock> loadMock;
        swm::load::VTLoaderV1_x ldr = {loadMock, hdr};

        swm::types::Mesh m = {};

        U32 count = 0u;
        const U32 output = 12;
        EXPECT_CALL(loadMock, loadNumber(count))
            .Times(1)
            .WillOnce(t::DoAll(t::SetArgReferee<0>(output), t::Return(swm::Result::Success)));

        EXPECT_CALL(loadMock,
                    loadArray(t::Matcher<std::vector<swm::types::Triangle>&>(t::_), t::Matcher<size_t>(output)))
            .Times(1)
            .WillOnce(t::Return(swm::Result::Success));

        EXPECT_EQ(ldr.loadTriangleData(m), swm::Result::Success);
    }

} // namespace vtLoaderV1_x

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
