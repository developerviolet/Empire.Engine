#include "PhysicalDevice.hpp"

#include "PhysicalDevice.hpp"
#include "Instance.hpp"

#include <vector>
#include <stdexcept>

namespace Vulkan
{
    PhysicalDevice::PhysicalDevice(const Instance& instance)
    {
        // Шаг 1: узнать, сколько GPU вообще есть в системе
        u32 deviceCount = 0;
        vkEnumeratePhysicalDevices(instance.Get(), &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            throw std::runtime_error("No GPUs with Vulkan support found");
        }

        // Шаг 2: получить реальный список этих GPU
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance.Get(), &deviceCount, devices.data());

        // Шаг 3: для старта просто берём первый попавшийся GPU
        physicalDevice = devices[0];

        // Шаг 4: узнать, сколько "очередей" (queue families) есть у этого GPU
        u32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(
            physicalDevice, &queueFamilyCount, nullptr
        );

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(
            physicalDevice, &queueFamilyCount, queueFamilies.data()
        );

        // Шаг 5: пройтись по всем очередям и найти ту, что умеет рисовать
        bool foundGraphicsQueue = false;

        for (u32 i = 0; i < queueFamilyCount; ++i)
        {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                graphicsQueueFamilyIndex = i;
                foundGraphicsQueue = true;
                break;
            }
        }

        if (!foundGraphicsQueue)
        {
            throw std::runtime_error("No graphics queue family found");
        }
    }

    VkPhysicalDevice PhysicalDevice::Get() const
    {
        return physicalDevice;
    }

    u32 PhysicalDevice::GetGraphicsQueueFamilyIndex() const
    {
        return graphicsQueueFamilyIndex;
    }
}