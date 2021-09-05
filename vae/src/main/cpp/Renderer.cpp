//
// Created by HuongNguyenXuan on 8/1/2021.
//

#include <shaderc/shaderc.h>
#include "Renderer.h"
#include "FileUtils.h"
#include "Material.h"

Renderer::Renderer(ANativeWindow* platformWindow,
                   VkApplicationInfo* appInfo) {
    device = new Device(platformWindow,appInfo);
    gfxPipeline = new GraphicsPipe(device->device_);
    swapchain = new SwapChain(device);
    cmdPool = new CommandPool(device);
    this->CreateRenderPass(device->device_);
    this->CreateFrameBuffers(renderPass_);
}

Renderer::~Renderer(){
    delete cmdPool;

}

//FIXME not dynamic
void Renderer::preRender(Object& object, Material& material){

    LOGI("CreateBuffers");
    object.vkBuffer = device->CreateBuffers(object.vertices.data(), object.vertices.size() * sizeof(float));

    LOGI("compileShader vertString");
    compileShader(material.vertString,
                            VK_SHADER_STAGE_VERTEX_BIT,
                            &material.vertexShader);

    LOGI("compileShader fragString");
    compileShader(material.fragString,
                            VK_SHADER_STAGE_FRAGMENT_BIT,
                            &material.fragmentShader);

    LOGI("CreateGraphicsPipeline");
    gfxPipeline->CreateGraphicsPipeline(material.vertexShader, material.fragmentShader,
                                       material.map ? 1:0,swapchain->displaySize_,renderPass_);

    std::vector<Texture*> mapSet;
    mapSet.push_back(material.map);

    LOGI("CreateDescriptorSet");
    gfxPipeline->CreateDescriptorSet(mapSet);
}


void Renderer::CreateFrameBuffers(VkRenderPass& renderPass,
                        VkImageView depthView) {
    // query display attachment to swapchain
    uint32_t SwapchainImagesCount = 0;
    CALL_VK(vkGetSwapchainImagesKHR(device->device_, swapchain->swapchain_,
                                    &SwapchainImagesCount, nullptr));
    swapchain->displayImages_ = new VkImage[SwapchainImagesCount];
    CALL_VK(vkGetSwapchainImagesKHR(device->device_, swapchain->swapchain_,
                                    &SwapchainImagesCount,
                                    swapchain->displayImages_));

    // create image view for each swapchain image
    swapchain->displayViews_ = new VkImageView[SwapchainImagesCount];
    for (uint32_t i = 0; i < SwapchainImagesCount; i++) {
        VkImageViewCreateInfo viewCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .image = swapchain->displayImages_[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = swapchain->displayFormat_,
                .components =
                        {
                                .r = VK_COMPONENT_SWIZZLE_R,
                                .g = VK_COMPONENT_SWIZZLE_G,
                                .b = VK_COMPONENT_SWIZZLE_B,
                                .a = VK_COMPONENT_SWIZZLE_A,
                        },
                .subresourceRange =
                        {
                                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                .baseMipLevel = 0,
                                .levelCount = 1,
                                .baseArrayLayer = 0,
                                .layerCount = 1,
                        },
        };
        CALL_VK(vkCreateImageView(device->device_, &viewCreateInfo, nullptr,
                                  &swapchain->displayViews_[i]));
    }

    // create a framebuffer from each swapchain image
    swapchain->framebuffers_ = new VkFramebuffer[swapchain->swapchainLength_];
    for (uint32_t i = 0; i < swapchain->swapchainLength_; i++) {
        VkImageView attachments[2] = {
                swapchain->displayViews_[i], depthView,
        };
        VkFramebufferCreateInfo fbCreateInfo{
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = nullptr,
                .renderPass = renderPass,
                .attachmentCount = 1,  // 2 if using depth
                .pAttachments = attachments,
                .width = static_cast<uint32_t>(swapchain->displaySize_.width),
                .height = static_cast<uint32_t>(swapchain->displaySize_.height),
                .layers = 1,
        };
        fbCreateInfo.attachmentCount = (depthView == VK_NULL_HANDLE ? 1 : 2);

        CALL_VK(vkCreateFramebuffer(device->device_, &fbCreateInfo, nullptr,
                                    &swapchain->framebuffers_[i]));
    }
}
void Renderer::CreateRenderPass(VkDevice device) {
    // -----------------------------------------------------------------
    // Create render pass
    VkAttachmentDescription attachmentDescriptions{
            .format = swapchain->displayFormat_,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    VkAttachmentReference colourReference = {
            .attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

    VkSubpassDescription subpassDescription{
            .flags = 0,
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .inputAttachmentCount = 0,
            .pInputAttachments = nullptr,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colourReference,
            .pResolveAttachments = nullptr,
            .pDepthStencilAttachment = nullptr,
            .preserveAttachmentCount = 0,
            .pPreserveAttachments = nullptr,
    };

    VkRenderPassCreateInfo renderPassCreateInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .attachmentCount = 1,
            .pAttachments = &attachmentDescriptions,
            .subpassCount = 1,
            .pSubpasses = &subpassDescription,
            .dependencyCount = 0,
            .pDependencies = nullptr,
    };

    CALL_VK(vkCreateRenderPass(device, &renderPassCreateInfo, nullptr,
                               &renderPass_));
    }

VkResult Renderer::compileShader(std::vector<char> glslShader, VkShaderStageFlagBits type,VkShaderModule *shaderOut) {

    // compile into spir-V shader
    shaderc_compiler_t compiler = shaderc_compiler_initialize();
    shaderc_compilation_result_t spvShader = shaderc_compile_into_spv(
            compiler, glslShader.data(), glslShader.size(), getShadercShaderType(type),
            "shaderc_error", "main", nullptr);
    if (shaderc_result_get_compilation_status(spvShader) !=
        shaderc_compilation_status_success) {
        return static_cast<VkResult>(-1);
    }

    // build vulkan shader module
    VkShaderModuleCreateInfo shaderModuleCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = shaderc_result_get_length(spvShader),
            .pCode = (const uint32_t *) shaderc_result_get_bytes(spvShader),
    };
    VkResult result = vkCreateShaderModule(device->device_, &shaderModuleCreateInfo,
                                           nullptr, shaderOut);

    shaderc_result_release(spvShader);
    shaderc_compiler_release(compiler);

    return result;
}


void Renderer::render(Object& object, Material& material) {
    // Record a command buffer that just clear the screen
    // 1 command buffer draw in 1 framebuffer
    // In our case we need 2 command as we have 2 framebuffer
    size_t cmdBufferLen_ = swapchain->swapchainLength_;
    cmdPool->allocateCommand(cmdBufferLen_);

    for (int bufferIndex = 0; bufferIndex < swapchain->swapchainLength_;
         bufferIndex++) {
        // We start by creating and declare the "beginning" our command buffer
        cmdPool->recordCommand(bufferIndex);

        // transition the buffer into color attachment
        cmdPool->setImageLayout(swapchain->displayImages_[bufferIndex],
                       VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

        // Now we start a renderpass. Any draw command has to be recorded in a
        // renderpass
        VkClearValue clearVals{
                {{ 0.0f, 0.34f, 0.90f, 1.0f,}},
        };

        VkRenderPassBeginInfo renderPassBeginInfo{
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .pNext = nullptr,
                .renderPass = renderPass_,
                .framebuffer = swapchain->framebuffers_[bufferIndex],
                .renderArea = {.offset =
                        {
                                .x = 0, .y = 0,
                        },
                        .extent = swapchain->displaySize_},
                .clearValueCount = 1,
                .pClearValues = &clearVals};
        //FIXME cmdPool->gfxCmd_
        vkCmdBeginRenderPass(cmdPool->gfxCmd_[bufferIndex], &renderPassBeginInfo,
                             VK_SUBPASS_CONTENTS_INLINE);
        // Bind what is necessary to the command buffer
        //FIXME gfxPipeline->pipeline_
        vkCmdBindPipeline(cmdPool->gfxCmd_[bufferIndex],
                          VK_PIPELINE_BIND_POINT_GRAPHICS, gfxPipeline->pipeline_);

        //texture map
        vkCmdBindDescriptorSets(
                cmdPool->gfxCmd_[bufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS,
                gfxPipeline->layout_, 0, 1, &gfxPipeline->descSet_, 0, nullptr);

        //vertex array
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(cmdPool->gfxCmd_[bufferIndex], 0, 1,
                               &object.vkBuffer, &offset);

        // Draw Triangle
        //VkCommandBuffer commandBuffer,uint32_t vertexCount,
        //uint32_t instanceCount,uint32_t firstVertex,uint32_t firstInstance);
        uint32_t  vertexCount = object.vertices.size()/object.stride_;
        vkCmdDraw(cmdPool->gfxCmd_[bufferIndex], vertexCount, 1, 0, 0);

        vkCmdEndRenderPass(cmdPool->gfxCmd_[bufferIndex]);
        cmdPool->setImageLayout(swapchain->displayImages_[bufferIndex],
                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                       VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

        cmdPool->closeCommand(bufferIndex);
    }

    // We need to create a fence to be able, in the main loop, to wait for our
    // draw command(s) to finish before swapping the framebuffers
    VkFenceCreateInfo fenceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
    };
    CALL_VK(
            vkCreateFence(device->device_, &fenceCreateInfo, nullptr, &fence_));

    // We need to create a semaphore to be able to wait, in the main loop, for our
    // framebuffer to be available for us before drawing.
    VkSemaphoreCreateInfo semaphoreCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
    };
    CALL_VK(vkCreateSemaphore(device->device_, &semaphoreCreateInfo, nullptr,
                              &semaphore_));

    device->initialized_ = true;

}

bool Renderer::draw() {
    uint32_t nextIndex;
    // Get the framebuffer index we should draw in
    CALL_VK(vkAcquireNextImageKHR(device->device_, swapchain->swapchain_,
                                  UINT64_MAX, semaphore_, VK_NULL_HANDLE,
                                  &nextIndex));

    LOGI("vkResetFences");
    CALL_VK(vkResetFences(device->device_, 1, &fence_));

    VkPipelineStageFlags waitStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &semaphore_,
            .pWaitDstStageMask = &waitStageMask,
            .commandBufferCount = 1,
            .pCommandBuffers = &cmdPool->gfxCmd_[nextIndex],
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr};

    LOGI("vkQueueSubmit");
    CALL_VK(vkQueueSubmit(device->queue_, 1, &submit_info, fence_));

    LOGI("vkWaitForFences");
    CALL_VK(vkWaitForFences(device->device_, 1, &fence_, VK_TRUE, 100000000));

    VkResult result;
    VkPresentInfoKHR presentInfo{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .swapchainCount = 1,
            .pSwapchains = &swapchain->swapchain_,
            .pImageIndices = &nextIndex,
            .pResults = &result,
    };

    LOGI("vkQueuePresentKHR");
    vkQueuePresentKHR(device->queue_, &presentInfo);
    return true;
}

bool Renderer::isReady(){
    return device->initialized_;
}



