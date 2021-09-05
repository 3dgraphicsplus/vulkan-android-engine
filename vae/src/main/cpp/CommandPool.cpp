//
// Created by HuongNguyenXuan on 8/29/2021.
//

#include "CommandPool.h"

CommandPool::CommandPool(Device *device) {
    this->device = device;
    // -----------------------------------------------
    // Create a pool of command buffers to allocate command buffer from
    VkCommandPoolCreateInfo cmdPoolCreateInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = 0,
    };
    CALL_VK(vkCreateCommandPool(this->device->device_, &cmdPoolCreateInfo, nullptr,
                                &cmdPool_));
}

void CommandPool::allocateCommand(uint32_t maxCount) {
    //FIXME for now, just allocate a number you need
    commandCount = maxCount;
    const VkCommandBufferAllocateInfo cmd = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = cmdPool_,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = commandCount,
    };

    gfxCmd_ = new VkCommandBuffer[commandCount];
    CALL_VK(vkAllocateCommandBuffers(this->device->device_, &cmd, gfxCmd_));

    activeCmd = 0;
}

void CommandPool::recordCommand(uint32_t index) {
    activeCmd = index;
    VkCommandBufferBeginInfo cmd_buf_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr};
    CALL_VK(vkBeginCommandBuffer(gfxCmd_[index], &cmd_buf_info));
}

void CommandPool::closeCommand(uint32_t index) {
    CALL_VK(vkEndCommandBuffer(gfxCmd_[index]));
}

void CommandPool::submitCommand(VkFence fence) {

    VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = nullptr,
            .commandBufferCount = commandCount,
            .pCommandBuffers = gfxCmd_,
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr,
    };
    CALL_VK(vkQueueSubmit(this->device->queue_, 1, &submitInfo, fence) != VK_SUCCESS);
}

void CommandPool::submitSyncCommand() {

    VkFenceCreateInfo fenceInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
    };
    VkFence fence;
    CALL_VK(vkCreateFence(this->device->device_, &fenceInfo, nullptr, &fence));

    this->submitCommand(fence);

    CALL_VK(vkWaitForFences(device->device_, 1/*no of fence*/, &fence, VK_TRUE, 100000000) !=
            VK_SUCCESS);
    vkDestroyFence(device->device_, fence, nullptr);
}

CommandPool::~CommandPool() {
    vkFreeCommandBuffers(device->device_, cmdPool_, commandCount, gfxCmd_);
    vkDestroyCommandPool(device->device_, cmdPool_, nullptr);
}


//predefine

void CommandPool::setImageLayout(VkImage image,
                                 VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
                                 VkPipelineStageFlags srcStages,
                                 VkPipelineStageFlags destStages) {
    VkImageMemoryBarrier imageMemoryBarrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = NULL,
            .srcAccessMask = 0,
            .dstAccessMask = 0,
            .oldLayout = oldImageLayout,
            .newLayout = newImageLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange =
                    {
                            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                            .baseMipLevel = 0,
                            .levelCount = 1,
                            .baseArrayLayer = 0,
                            .layerCount = 1,
                    },
    };

    switch (oldImageLayout) {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;

        default:
            break;
    }

    switch (newImageLayout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.dstAccessMask =
                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        default:
            break;
    }

    vkCmdPipelineBarrier(gfxCmd_[activeCmd], srcStages, destStages, 0, 0, NULL, 0, NULL, 1,
                         &imageMemoryBarrier);
}

void
CommandPool::copyImage(VkImage src, VkImageLayout layout, VkImage dest, VkImageLayout layout1,
                       uint32_t regionCount,
                       const VkImageCopy *pRegions) {
    vkCmdCopyImage(gfxCmd_[activeCmd], src, layout,
                   dest, layout1, regionCount,
                   pRegions);
}
