#pragma once 

#define VK_USE_PLATFORM_XCB_KHR
#include <vulkan/vulkan.h>

namespace Vulkan
{
    class Instance
    {
        public:
        Instance();
        ~Instance();

        Instance(const Instance&) = delete;
        Instance& operator=(const Instance&) = delete;

        VkInstance Get() const;

        private:
        VkInstance instance = nullptr;
    };
}   