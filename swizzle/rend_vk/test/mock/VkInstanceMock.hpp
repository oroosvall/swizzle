
#pragma once

/* Include files */

#include <vulkan/vulkan.h>

/* Defines */

/* Typedefs/enums */

enum class InstanceMockInit
{
    InstanceInvalid,
    InstanceInvalid_NoExtensions,
    InstanceValid_1Device
};

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

void VkInstanceMockInitialize(InstanceMockInit init);
void VkInstanceMockCleanup();
