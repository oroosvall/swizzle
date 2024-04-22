
#pragma once

/* Include files */

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace rvk
{
    enum class MemoryType
    {
        DeviceLocalHostVisible,
        DeviceLocal,
        Host
    };
} // namespace rvk

/* Class Declaration */

namespace rvk
{
    class GfxDevice
    {
    public:
        virtual void waitIKdle() const            = 0;
        virtual std::string getDeviceName() const = 0;

    protected:
    };
} // namespace rvk

/* Function Declaration */
