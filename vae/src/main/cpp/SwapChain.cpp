//
// Created by HuongNguyenXuan on 8/1/2021.
//

#include <cassert>
#include <vector>

#include "SwapChain.h"
#include "Log.hpp"

SwapChain::SwapChain(Device* device){
    this->device = device;
    this->CreateSwapChain();
}

SwapChain::~SwapChain() {
    this->DeleteSwapChain();
}

void SwapChain::CreateSwapChain(void) {
    LOGI("->createSwapChain");
    //memset(&swapchain, 0, sizeof(swapchain));

    // **********************************************************
    // Get the surface capabilities because:
    //   - It contains the minimal and max length of the chain, we will need it
    //   - It's necessary to query the supported surface format (R8G8B8A8 for
    //   instance ...)
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->gpuDevice_, device->surface_,
                                              &surfaceCapabilities);
    // Query the list of supported surface format and choose one we like
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->gpuDevice_, device->surface_,
                                         &formatCount, nullptr);
    VkSurfaceFormatKHR* formats = new VkSurfaceFormatKHR[formatCount];
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->gpuDevice_, device->surface_,
                                         &formatCount, formats);
    LOGI("Got %d formats", formatCount);

    uint32_t chosenFormat;
    for (chosenFormat = 0; chosenFormat < formatCount; chosenFormat++) {
        if (formats[chosenFormat].format == VK_FORMAT_R8G8B8A8_UNORM) break;
    }
    assert(chosenFormat < formatCount);

    displaySize_ = surfaceCapabilities.currentExtent;
    displayFormat_ = formats[chosenFormat].format;

    // **********************************************************
    // Create a swap chain (here we choose the minimum available number of surface
    // in the chain)
    VkSwapchainCreateInfoKHR swapchainCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .surface = device->surface_,
            .minImageCount = surfaceCapabilities.minImageCount,
            .imageFormat = formats[chosenFormat].format,
            .imageColorSpace = formats[chosenFormat].colorSpace,
            .imageExtent = surfaceCapabilities.currentExtent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &device->queueFamilyIndex_,
            .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            .presentMode = VK_PRESENT_MODE_FIFO_KHR,
            .clipped = VK_FALSE,
            .oldSwapchain = VK_NULL_HANDLE,
    };
    CALL_VK(vkCreateSwapchainKHR(device->device_, &swapchainCreateInfo, nullptr,
                                 &swapchain_));

    // Get the length of the created swap chain
    CALL_VK(vkGetSwapchainImagesKHR(device->device_, swapchain_,
                                    &swapchainLength_, nullptr));
    delete[] formats;
    LOGI("<-createSwapChain");
}

void SwapChain::DeleteSwapChain(void) {
    for (int i = 0; i < swapchainLength_; i++) {
        vkDestroyFramebuffer(device->device_, framebuffers_[i], nullptr);
        vkDestroyImageView(device->device_, displayViews_[i], nullptr);
    }
    delete[] framebuffers_;
    delete[] displayViews_;
    delete[] displayImages_;

    vkDestroySwapchainKHR(device->device_, swapchain_, nullptr);
}