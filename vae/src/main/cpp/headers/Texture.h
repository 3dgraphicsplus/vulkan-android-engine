//
// Created by HuongNguyenXuan on 8/1/2021.
//

#ifndef VULKAN_ANDROID_ENGINE_TEXTURE_H
#define VULKAN_ANDROID_ENGINE_TEXTURE_H

#include "Log.hpp"
#include "vulkan_wrapper.h"

//FIXME will bring out later
#include "Device.h"
#include "CommandPool.h"

static const VkFormat kTexFmt = VK_FORMAT_R8G8B8A8_UNORM;
class Texture {
    static constexpr  char* TAG = "Texture";
public:
    //FIXME device should be outside?
    Texture(Device* device = NULL);

    VkResult CreateTexture(unsigned char* imageData, uint32_t  imgWidth, uint32_t  imgHeight,
                           VkImageUsageFlags usage, VkFlags required_props);

private:

    Device* device;

    VkResult AllocateMemoryTypeFromProperties(uint32_t typeBits,
                                                       VkFlags requirements_mask,
                                                       uint32_t* typeIndex);



public:
//texture_object
    VkSampler sampler_;
    VkImage image_;
    VkImageLayout imageLayout;
    VkDeviceMemory mem;
    VkImageView view_;
    int32_t tex_width;
    int32_t tex_height;
};


#endif //VULKAN_ANDROID_ENGINE_TEXTURE_H
