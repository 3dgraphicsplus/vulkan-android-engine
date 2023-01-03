//
// Created by HuongNguyenXuan on 8/29/2021.
//

#ifndef VULKAN_ANDROID_ENGINE_COMMANDPOOL_H
#define VULKAN_ANDROID_ENGINE_COMMANDPOOL_H

#include "vulkan_wrapper.h"
#include "Log.hpp"
#include "Device.h"

class CommandPool {
public:
    CommandPool(Device* device);
    ~CommandPool();
    void allocateCommand(uint32_t maxCount = 1);
    void recordCommand(uint32_t index = 0);
    void closeCommand(uint32_t index = 0);
    void submitCommand(VkFence fence = VK_NULL_HANDLE);
    void submitSyncCommand();


    //vk
    VkCommandPool cmdPool_;
    Device* device;

    VkCommandBuffer* gfxCmd_;
    uint32_t commandCount;

    uint32_t  activeCmd;

    //predefine
    void setImageLayout(VkImage image,
                        VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
                        VkPipelineStageFlags srcStages,
                        VkPipelineStageFlags destStages);

    void copyImage(VkImage src, VkImageLayout layout, VkImage dest, VkImageLayout layout1, uint32_t regionCount,
                   const VkImageCopy* pRegions);
};


#endif //VULKAN_ANDROID_ENGINE_COMMANDPOOL_H
