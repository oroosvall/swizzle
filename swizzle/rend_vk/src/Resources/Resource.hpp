
#pragma once

/* Include files */

#include "Vk.hpp"
#include "_fw.hpp"

#include <memory>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace rvk
{

    class IResource
    {
    public:
        virtual ~IResource(){ }
        virtual uint32_t getUserCount() const = 0;
    };


    // Do we need this?
    template <typename T>
    class Resource : public IResource
    {
    public:

        Resource(T resource, )
            : mResource(resource)
        {
        }

        virtual ~Resource()
        {
            if (mResource)
            {

            }
        }

        T reset()
        {
            T ret = mResource;
            mResource = VK_NULL_HANDLE;
            return ret;
        }

        T operator()
        {
            return mResource;
        }

        virtual uint32_t activeUserCount() const override
        {
            return 0;
        }

    private:
        T mResource;


    };

} // namespace rvk

/* Function Declaration */