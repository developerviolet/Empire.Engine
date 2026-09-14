#include "PlatformExtensions.hpp"

#include <vector>

#define VK_USE_PLATFORM_XCB_KHR
#include <vulkan/vulkan.h>

namespace Vulkan
{
    std::vector<const char*> GetPlatformSurfaceExtensions()
    {
        return {
            VK_KHR_XCB_SURFACE_EXTENSION_NAME
        };
    }
}