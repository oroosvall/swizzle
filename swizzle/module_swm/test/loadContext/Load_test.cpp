
/* Include files */

#include <gtest/gtest.h>

#include "Load.hpp"
#include "mocks/LoaderMock.hpp"

namespace t = testing;

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

namespace loadData
{
    TEST(SWM_Load_Data, loadData_0_meshes_success)
    {
        t::StrictMock<mocks::MeshLoaderIfcMock> meshLoaderIfcMock;
        swm::Model output;

        U32 count = 0u;
        U32 meshCount = 0u;
        EXPECT_CALL(meshLoaderIfcMock, loadMeshHeader(t::Matcher<U32&>(count))).Times(1).WillOnce(
            t::DoAll(
                t::SetArgReferee<0>(meshCount),
                t::Return(swm::Result::Success)
            )
        );

        EXPECT_EQ(swm::load::LoadData(meshLoaderIfcMock, output), swm::Result::Success);
    }

    TEST(SWM_Load_Data, loadData_0_meshes_fail)
    {
        t::StrictMock<mocks::MeshLoaderIfcMock> meshLoaderIfcMock;
        swm::Model output;

        U32 count = 0u;
        U32 meshCount = 0u;
        EXPECT_CALL(meshLoaderIfcMock, loadMeshHeader(t::Matcher<U32&>(count))).Times(1).WillOnce(
            t::DoAll(
                t::SetArgReferee<0>(meshCount),
                t::Return(swm::Result::ErrFileIo)
            )
        );

        EXPECT_EQ(swm::load::LoadData(meshLoaderIfcMock, output), swm::Result::ErrFileIo);
    }


    TEST(SWM_Load_Data, loadData_10_meshes_success)
    {
        t::StrictMock<mocks::MeshLoaderIfcMock> meshLoaderIfcMock;
        swm::Model output;

        U32 count = 0u;
        U32 meshCount = 10u;
        EXPECT_CALL(meshLoaderIfcMock, loadMeshHeader(t::Matcher<U32&>(count))).Times(1).WillOnce(
            t::DoAll(
                t::SetArgReferee<0>(meshCount),
                t::Return(swm::Result::Success)
            )
        );

        EXPECT_CALL(meshLoaderIfcMock, loadMeshData(t::_)).Times(meshCount).WillOnce(
            t::Return(swm::Result::Success)
        );

        EXPECT_EQ(swm::load::LoadData(meshLoaderIfcMock, output), swm::Result::Success);
    }

    TEST(SWM_Load_Data, loadData_10_meshes_fail_after_5)
    {
        t::StrictMock<mocks::MeshLoaderIfcMock> meshLoaderIfcMock;
        swm::Model output;

        U32 count = 0u;
        U32 meshCount = 10u;
        EXPECT_CALL(meshLoaderIfcMock, loadMeshHeader(t::Matcher<U32&>(count))).Times(1).WillOnce(
            t::DoAll(
                t::SetArgReferee<0>(meshCount),
                t::Return(swm::Result::Success)
            )
        );

        {
            t::InSequence s;
            EXPECT_CALL(meshLoaderIfcMock, loadMeshData(t::_)).Times(5u).WillRepeatedly(
                t::Return(swm::Result::Success)
            );
            EXPECT_CALL(meshLoaderIfcMock, loadMeshData(t::_)).WillOnce(
            t::Return(swm::Result::ErrFileIo)
            );
        }

        EXPECT_EQ(swm::load::LoadData(meshLoaderIfcMock, output), swm::Result::ErrFileIo);
    }

}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
