#ifndef CLEANUP_HPP
#define CLEANUP_HPP

/* Include files */

#include <swizzle/core/Thread.hpp>

#include <mutex>
#include <condition_variable>
#include <list>
#include <atomic>
#include <thread>

#include "_fwDecl.hpp"


/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace vk
{
    struct ListContainer
    {
        std::list<common::Resource<DeviceMemory>> mMemoryAllocations;
        std::list<common::Resource<IVkResource>> mResources;
    };
}

/* Class Declaration */

namespace vk
{
    class CleanupRunnable
    {
    public:
        CleanupRunnable(common::Resource<Device> device);
        virtual ~CleanupRunnable();

        void run();
        void stop();

        void scheduleResourceDestruction(common::Resource<IVkResource> resource);
        void scheduleFreeingMemory(common::Resource<DeviceMemory> memory);

        void notify();

    private:

        bool hasWork() const;
        void waitWork();

        void doCleanup();

        common::Resource<Device> mDevice;

        std::array<ListContainer, 2> mLists;

        mutable std::mutex mMux;

        std::condition_variable mCond;

        std::atomic_bool mRunning;

        std::thread mThread;

        U32 mCleanupIndex;
        U32 mBufferIndex;

    };
}

/* Function Declaration */

#endif