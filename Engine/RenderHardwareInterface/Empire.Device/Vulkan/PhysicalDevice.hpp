#pragma once

#include "Aliases.hpp"

#include <vulkan/vulkan.h>

using namespace Engine;

namespace Vulkan
{
    class Instance;

    class PhysicalDevice
    {
    public:
        PhysicalDevice(const Instance& instance);

        VkPhysicalDevice Get() const;
        u32 GetGraphicsQueueFamilyIndex() const;

    private:
        VkPhysicalDevice physicalDevice = nullptr;
        u32 graphicsQueueFamilyIndex = 0;
    };
}