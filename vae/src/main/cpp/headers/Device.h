//
// Created by HuongNguyenXuan on 8/1/2021.
//

#ifndef VULKAN_ANDROID_ENGINE_DEVICE_H
#define VULKAN_ANDROID_ENGINE_DEVICE_H

#include <cassert>
#include <vector>

#include "Log.hpp"
#include "vulkan_wrapper.h"

class Device {
public:
    Device(ANativeWindow *platformWindow, VkApplicationInfo *appInfo = nullptr);
    ~Device();
private:
    void CreateVulkanDevice(ANativeWindow* platformWindow,
                            VkApplicationInfo* appInfo = nullptr);

public:
//VulkanDeviceInfo
    VkInstance instance_;
    VkPhysicalDevice gpuDevice_;
    VkPhysicalDeviceMemoryProperties gpuMemoryProperties_;
    VkDevice device_;
    uint32_t queueFamilyIndex_;

    VkSurfaceKHR surface_;
    VkQueue queue_;


    VkBuffer CreateBuffers(float* vertexData, uint32_t InBytes);
    void DeleteBuffers(VkBuffer& buffer);
    bool MapMemoryTypeToIndex(uint32_t typeBits, VkFlags requirements_mask,
                              uint32_t* typeIndex);

};


#endif //VULKAN_ANDROID_ENGINE_DEVICE_H
