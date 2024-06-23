/* Include files */

#include "Dev.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace rvk
{
    Dev::Dev(std::shared_ptr<Device> dev)
        : mDevice(dev)
    {
    }

    Dev::~Dev()
    {
    }

    void Dev::waitIdle() const
    {
        return mDevice->waitIdle();
    }

    std::string Dev::getDeviceName() const
    {
        return mDevice->getDeviceName();
    }

} // namespace rvk

/* Class Protected Function Definition */

/* Class Private Function Definition */
