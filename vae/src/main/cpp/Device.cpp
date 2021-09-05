//
// Created by HuongNguyenXuan on 8/1/2021.
//

#include "Device.h"

Device::Device(ANativeWindow* platformWindow,
               VkApplicationInfo* appInfo) {
    this->CreateVulkanDevice(platformWindow,appInfo);
}

// Create vulkan device
void Device::CreateVulkanDevice(ANativeWindow* platformWindow,
                                  VkApplicationInfo* appInfo) {
    std::vector<const char*> instance_extensions;
    std::vector<const char*> device_extensions;

    instance_extensions.push_back("VK_KHR_surface");
    instance_extensions.push_back("VK_KHR_android_surface");

    device_extensions.push_back("VK_KHR_swapchain");

    // **********************************************************
    // Create the Vulkan instance
    VkInstanceCreateInfo instanceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .pApplicationInfo = appInfo,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount =
            static_cast<uint32_t>(instance_extensions.size()),
            .ppEnabledExtensionNames = instance_extensions.data(),
    };
    CALL_VK(vkCreateInstance(&instanceCreateInfo, nullptr, &instance_));
    VkAndroidSurfaceCreateInfoKHR createInfo{
            .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .window = platformWindow};

    CALL_VK(vkCreateAndroidSurfaceKHR(instance_, &createInfo, nullptr,
                                      &surface_));
    // Find one GPU to use:
    // On Android, every GPU device is equal -- supporting
    // graphics/compute/present
    // for this sample, we use the very first GPU device found on the system
    uint32_t gpuCount = 0;
    CALL_VK(vkEnumeratePhysicalDevices(instance_, &gpuCount, nullptr));
    VkPhysicalDevice tmpGpus[gpuCount];
    CALL_VK(vkEnumeratePhysicalDevices(instance_, &gpuCount, tmpGpus));
    gpuDevice_ = tmpGpus[0];  // Pick up the first GPU Device

    vkGetPhysicalDeviceMemoryProperties(gpuDevice_,
                                        &gpuMemoryProperties_);

    // Find a GFX queue family
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(gpuDevice_, &queueFamilyCount,
                                             nullptr);
    assert(queueFamilyCount);
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(gpuDevice_, &queueFamilyCount,
                                             queueFamilyProperties.data());

    uint32_t queueFamilyIndex;
    for (queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount;
         queueFamilyIndex++) {
        if (queueFamilyProperties[queueFamilyIndex].queueFlags &
            VK_QUEUE_GRAPHICS_BIT) {
            break;
        }
    }
    assert(queueFamilyIndex < queueFamilyCount);
    queueFamilyIndex_ = queueFamilyIndex;
    // Create a logical device (vulkan device)
    float priorities[] = {
            1.0f,
    };
    VkDeviceQueueCreateInfo queueCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = queueFamilyIndex_,
            .queueCount = 1,
            .pQueuePriorities = priorities,
    };

    VkDeviceCreateInfo deviceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &queueCreateInfo,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(device_extensions.size()),
            .ppEnabledExtensionNames = device_extensions.data(),
            .pEnabledFeatures = nullptr,
    };

    CALL_VK(vkCreateDevice(gpuDevice_, &deviceCreateInfo, nullptr,
                           &device_));
    vkGetDeviceQueue(device_, 0, 0, &queue_);
}


void Device::DeleteBuffers(VkBuffer& buffer) {
    vkDestroyBuffer(device_, buffer, nullptr);
}

// Create our vertex buffer
VkBuffer Device::CreateBuffers(float* vertexData, uint32_t sizeInByte) {

    VkBuffer buffers;
    // Create a vertex buffer
    VkBufferCreateInfo createBufferInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = sizeInByte,
            .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &queueFamilyIndex_,
    };

    CALL_VK(vkCreateBuffer(device_, &createBufferInfo, nullptr,
                           &buffers));

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(device_, buffers, &memReq);

    VkMemoryAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memReq.size,
            .memoryTypeIndex = 0,  // Memory type assigned in the next step
    };

    // Assign the proper memory type for that buffer
    MapMemoryTypeToIndex(memReq.memoryTypeBits,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         &allocInfo.memoryTypeIndex);

    // Allocate memory for the buffer
    VkDeviceMemory deviceMemory;
    CALL_VK(vkAllocateMemory(device_, &allocInfo, nullptr, &deviceMemory));

    void* data;
    CALL_VK(vkMapMemory(device_, deviceMemory, 0, allocInfo.allocationSize,
                        0, &data));
    memcpy(data, vertexData, sizeInByte);
    vkUnmapMemory(device_, deviceMemory);

    CALL_VK(
            vkBindBufferMemory(device_, buffers, deviceMemory, 0));
    return buffers;
}


// A helper function
bool Device::MapMemoryTypeToIndex(uint32_t typeBits, VkFlags requirements_mask,
                          uint32_t* typeIndex) {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(gpuDevice_, &memoryProperties);
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < 32; i++) {
        if ((typeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((memoryProperties.memoryTypes[i].propertyFlags & requirements_mask) ==
                requirements_mask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    return false;
}

Device::~Device() {
    vkDestroyDevice(device_, nullptr);
    vkDestroyInstance(instance_, nullptr);
}

