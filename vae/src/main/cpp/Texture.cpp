//
// Created by HuongNguyenXuan on 8/1/2021.
//

#include <cassert>
#include "Texture.h"
#include "CommandPool.h"

Texture::Texture(Device* device) {
    this->device = device;
}

VkResult Texture::setData(unsigned char* imageData, uint32_t  imgWidth, uint32_t  imgHeight,
                          VkImageUsageFlags usage, VkFlags required_props) {
    if (!(usage | required_props)) {
        __android_log_print(ANDROID_LOG_ERROR, TAG,
                            "No usage and required_pros");
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }

    // Check for linear supportability
    VkFormatProperties props;
    bool needBlit = true;
    vkGetPhysicalDeviceFormatProperties(device->gpuDevice_, kTexFmt, &props);
    assert((props.linearTilingFeatures | props.optimalTilingFeatures) &
           VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

    if (props.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) {
        // linear format supporting the required texture
        needBlit = false;
        __android_log_print(ANDROID_LOG_INFO, TAG,
                            "You're good, no Blit is required");
    }



    tex_width = imgWidth;
    tex_height = imgHeight;

    // Allocate the linear texture so texture could be copied over
    VkImageCreateInfo image_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = kTexFmt,
            .extent = {static_cast<uint32_t>(tex_width),
                       static_cast<uint32_t>(tex_height), 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_LINEAR,
            .usage = (needBlit ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                               : VK_IMAGE_USAGE_SAMPLED_BIT),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &device->queueFamilyIndex_,
            .initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED,
    };
    VkMemoryAllocateInfo mem_alloc = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = 0,
            .memoryTypeIndex = 0,
    };

    VkMemoryRequirements mem_reqs;
    CALL_VK(vkCreateImage(device->device_, &image_create_info, nullptr,
                          &image_));
    vkGetImageMemoryRequirements(device->device_, image_, &mem_reqs);
    mem_alloc.allocationSize = mem_reqs.size;
    VK_CHECK(AllocateMemoryTypeFromProperties(mem_reqs.memoryTypeBits,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                              &mem_alloc.memoryTypeIndex));
    CALL_VK(vkAllocateMemory(device->device_, &mem_alloc, nullptr, &mem));
    CALL_VK(vkBindImageMemory(device->device_, image_, mem, 0));

    if (required_props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        const VkImageSubresource subres = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .arrayLayer = 0,
        };
        VkSubresourceLayout layout;
        void* data;

        vkGetImageSubresourceLayout(device->device_, image_, &subres,
                                    &layout);
        CALL_VK(vkMapMemory(device->device_, mem, 0,
                            mem_alloc.allocationSize, 0, &data));
        VkDeviceSize imageSize = mem_alloc.allocationSize;
        memcpy(data, imageData, static_cast<size_t>(imageSize));
//        for (int32_t y = 0; y < imgHeight; y++) {
//            unsigned char* row = (unsigned char*)((char*)data + layout.rowPitch * y);
//            for (int32_t x = 0; x < imgWidth; x++) {
//                row[x * 4] = imageData[(x + y * imgWidth) * 4];
//                row[x * 4 + 1] = imageData[(x + y * imgWidth) * 4 + 1];
//                row[x * 4 + 2] = imageData[(x + y * imgWidth) * 4 + 2];
//                row[x * 4 + 3] = imageData[(x + y * imgWidth) * 4 + 3];
//            }
//        }

        vkUnmapMemory(device->device_, mem);
    }

    imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    CommandPool cmdPool(device);
    cmdPool.allocateCommand();
    cmdPool.recordCommand();

    // If linear is supported, we are done
    VkImage stageImage = VK_NULL_HANDLE;
    VkDeviceMemory stageMem = VK_NULL_HANDLE;
    if (!needBlit) {
        cmdPool.setImageLayout(image_, VK_IMAGE_LAYOUT_PREINITIALIZED,
                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                       VK_PIPELINE_STAGE_HOST_BIT,
                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    }
    else {
        // save current image and mem as staging image and memory
        stageImage = image_;
        stageMem = mem;
        image_ = VK_NULL_HANDLE;
        mem = VK_NULL_HANDLE;

        // Create a new tile texture to blit into
        image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_create_info.usage =
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        CALL_VK(vkCreateImage(device->device_, &image_create_info, nullptr,
                              &image_));
        vkGetImageMemoryRequirements(device->device_, image_, &mem_reqs);

        mem_alloc.allocationSize = mem_reqs.size;
        VK_CHECK(AllocateMemoryTypeFromProperties(
                mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                &mem_alloc.memoryTypeIndex));
        CALL_VK(
                vkAllocateMemory(device->device_, &mem_alloc, nullptr, &mem));
        CALL_VK(vkBindImageMemory(device->device_, image_, mem, 0));

        // transitions image out of UNDEFINED type
        cmdPool.setImageLayout(stageImage, VK_IMAGE_LAYOUT_PREINITIALIZED,
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
        cmdPool.setImageLayout(image_, VK_IMAGE_LAYOUT_UNDEFINED,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
        VkImageCopy bltInfo{
                {
                        VK_IMAGE_ASPECT_COLOR_BIT,
                            0,
                                      0,
                        1,
                },
                { 0, 0, 0 },
                {
                        VK_IMAGE_ASPECT_COLOR_BIT,
                            0,
                                      0,
                        1,
                },
                { 0, 0, 0},
                { imgWidth, imgHeight,1,},
        };
        cmdPool.copyImage(stageImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       image_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                       &bltInfo);

        cmdPool.setImageLayout(image_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    }

    cmdPool.closeCommand();
    cmdPool.submitSyncCommand();

    if (stageImage != VK_NULL_HANDLE) {
        vkDestroyImage(device->device_, stageImage, nullptr);
        vkFreeMemory(device->device_, stageMem, nullptr);
    }

    //create view
    const VkSamplerCreateInfo sampler = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext = nullptr,
            .magFilter = VK_FILTER_NEAREST,
            .minFilter = VK_FILTER_NEAREST,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .mipLodBias = 0.0f,
            .maxAnisotropy = 1,
            .compareOp = VK_COMPARE_OP_NEVER,
            .minLod = 0.0f,
            .maxLod = 0.0f,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
            .unnormalizedCoordinates = VK_FALSE,
    };
    VkImageViewCreateInfo viewInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = VK_NULL_HANDLE,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = kTexFmt,
            .components =
                    {
                            VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
                            VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A,
                    },
            .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
    };

    CALL_VK(vkCreateSampler(device->device_, &sampler, nullptr,
                            &sampler_));
    viewInfo.image = image_;
    CALL_VK(
            vkCreateImageView(device->device_, &viewInfo, nullptr, &view_));

    return VK_SUCCESS;
}



// A help function to map required memory property into a VK memory type
// memory type is an index into the array of 32 entries; or the bit index
// for the memory type ( each BIT of an 32 bit integer is a type ).
VkResult Texture::AllocateMemoryTypeFromProperties(uint32_t typeBits,
                                          VkFlags requirements_mask,
                                          uint32_t* typeIndex) {
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < 32; i++) {
        if ((typeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((device->gpuMemoryProperties_.memoryTypes[i].propertyFlags &
                 requirements_mask) == requirements_mask) {
                *typeIndex = i;
                return VK_SUCCESS;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return VK_ERROR_MEMORY_MAP_FAILED;
}


