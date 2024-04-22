
#pragma once

/* Include files */

//#include <cstdint>
//#include <memory>
//#include <string>
//#include <vector>

#include <rend_vk/GfxDevice.hpp>

#include "resources/Device.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace rvk
{
    class Dev : public GfxDevice
    {
    public:
        Dev(std::shared_ptr<Device> dev);
        virtual ~Dev();

        virtual void waitIKdle() const override;
        virtual std::string getDeviceName() const override;

    private:
        std::shared_ptr<Device> mDevice;
    };
} // namespace rvk

/* Function Declaration */
