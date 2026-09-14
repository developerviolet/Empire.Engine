#include "Instance.hpp"
#include "PlatformExtensions.hpp"

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>

namespace Vulkan
{
    Instance::Instance()
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Empire Engine";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Empire";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_1;

        std::vector<const char*> requiredExtensions = {
            VK_KHR_SURFACE_EXTENSION_NAME
        };

        std::vector<const char*> platformExtensions = GetPlatformSurfaceExtensions();
        requiredExtensions.insert(
            requiredExtensions.end(),
            platformExtensions.begin(),
            platformExtensions.end()
        );

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
        createInfo.enabledLayerCount = 0;

        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan instance");
        }
    }

    Instance::~Instance()
    {
        if (instance)
        {
            vkDestroyInstance(instance, nullptr);
        }
    }

    VkInstance Instance::Get() const
    {
        return instance;
    }
}