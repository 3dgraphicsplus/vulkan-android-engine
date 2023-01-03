//
// Created by HuongNguyenXuan on 8/1/2021.
//

#ifndef VULKAN_ANDROID_ENGINE_SWAPCHAIN_H
#define VULKAN_ANDROID_ENGINE_SWAPCHAIN_H

#include "Log.hpp"
#include "vulkan_wrapper.h"
#include "Device.h"

class SwapChain {
public:
    SwapChain(Device* device);
    ~SwapChain();
private:
    void CreateSwapChain(void);
    void DeleteSwapChain(void);

public:
    Device* device;
//VulkanSwapchainInfo
    VkSwapchainKHR swapchain_;
    uint32_t swapchainLength_;

    VkExtent2D displaySize_;
    VkFormat displayFormat_;

    // array of frame buffers and views
    VkFramebuffer* framebuffers_;
    VkImage* displayImages_;
    VkImageView* displayViews_;

};


#endif //VULKAN_ANDROID_ENGINE_SWAPCHAIN_H
