
/* Include files */

#include "Cleanup.hpp"

#include <mutex>

#include "Device.hpp"
#include "VkResource.hpp"
#include "DeviceMemory.hpp"

#include <optick/optick.h>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace vk
{

    CleanupRunnable::CleanupRunnable(common::Resource<Device> device)
        : mDevice(device)
        , mLists()
        , mMux()
        , mCond()
        , mRunning(true)
        , mCleanupIndex(0u)
        , mBufferIndex(1u)
    {
        mThread = std::thread(&CleanupRunnable::run, this);
    }

    CleanupRunnable::~CleanupRunnable()
    {
    }

    void CleanupRunnable::run()
    {
        OPTICK_THREAD("Vk::Cleanup");
        while (mRunning)
        {
            OPTICK_EVENT("WaitWork");
            while (!hasWork())
            {
                waitWork();
                if (!mRunning)
                {
                    break;
                }
            }
            doCleanup();
        }
    }

    void CleanupRunnable::stop()
    {
        mRunning = false;
        mCond.notify_one();
        mThread.join();
    }

    void CleanupRunnable::sheduleResourceDestruction(common::Resource<IVkResource> resource)
    {
        std::lock_guard lock(mMux);
        mLists[mBufferIndex].mResources.emplace_back(resource);
        mCond.notify_one();
    }

    void CleanupRunnable::sheduleFreeingMemory(common::Resource<DeviceMemory> memory)
    {
        std::lock_guard lock(mMux);
        mLists[mBufferIndex].mMemoryAllocations.emplace_back(memory);
        mCond.notify_one();
    }

} // namespace vk

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{
    bool CleanupRunnable::hasWork() const
    {
        /*bool hasWork = false;
        if (mMux.try_lock())
        {
            bool notEmpty1 = !mMemoryAllocations.empty();
            bool notEmpty2 = !mResources.empty();
            mMux.unlock();

            hasWork = notEmpty1 || notEmpty2;

        }

        return hasWork;*/
        return true;
    }

    void CleanupRunnable::waitWork()
    {
        std::unique_lock<std::mutex> lock(mMux);
        mCond.wait(lock);
    }

    void CleanupRunnable::doCleanup()
    {
        OPTICK_EVENT("doCleanup");
        if (mMux.try_lock())
        {
            OPTICK_EVENT("swap");
            U32 tmp = mCleanupIndex;
            mCleanupIndex = mBufferIndex;
            mBufferIndex = tmp;

            mMux.unlock();
        }

        {
            OPTICK_EVENT("mResources.begin()");
            auto itRes = mLists[mCleanupIndex].mResources.begin();
            while (itRes != mLists[mCleanupIndex].mResources.end())
            {
                if ((*itRes)->activeUserCount() == 0)
                {
                    auto& itm = *itRes;
                    mDevice->destroyResource(itm);
                    itRes = mLists[mCleanupIndex].mResources.erase(itRes);
                }
                else
                {
                    itRes++;
                }
            }
        }

        {
            OPTICK_EVENT("mMemoryAllocations.begin()");
            auto itMem = mLists[mCleanupIndex].mMemoryAllocations.begin();
            while (itMem != mLists[mCleanupIndex].mMemoryAllocations.end())
            {
                if ((*itMem)->activeUserCound() == 0)
                {
                    auto& itm = *itMem;
                    mDevice->freeMemory(itm);
                    itMem = mLists[mCleanupIndex].mMemoryAllocations.erase(itMem);
                }
                else
                {
                    itMem++;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}