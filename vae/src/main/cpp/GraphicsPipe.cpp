//
// Created by HuongNguyenXuan on 8/6/2021.
//

#include "GraphicsPipe.h"
#include "Texture.h"

// Create Graphics Pipeline
VkResult GraphicsPipe::CreateGraphicsPipeline(VkShaderModule vertexShader, VkShaderModule fragmentShader,
                                              uint32_t textureCount, VkExtent2D displaySize,
                                              VkRenderPass renderPass) {

    const VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = textureCount,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
    };
    const VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .bindingCount = 1,
            .pBindings = &descriptorSetLayoutBinding,
    };

    LOGI("vkCreateDescriptorSetLayout");
    CALL_VK(vkCreateDescriptorSetLayout(device_,
                                        &descriptorSetLayoutCreateInfo, nullptr,
                                        &dscLayout_));
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .setLayoutCount = 1,
            .pSetLayouts = &dscLayout_,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
    };

    LOGI("vkCreatePipelineLayout");
    CALL_VK(vkCreatePipelineLayout(device_, &pipelineLayoutCreateInfo,
                                   nullptr, &layout_));

    // No dynamic state in that tutorial
    VkPipelineDynamicStateCreateInfo dynamicStateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .dynamicStateCount = 0,
            .pDynamicStates = nullptr
    };

    // Specify vertex and fragment shader stages
    VkPipelineShaderStageCreateInfo shaderStages[2]{
            {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = VK_SHADER_STAGE_VERTEX_BIT,
                    .module = vertexShader,
                    .pName = "main",
                    .pSpecializationInfo = nullptr,
            },
            {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .module = fragmentShader,
                    .pName = "main",
                    .pSpecializationInfo = nullptr,
            }};

    VkViewport viewports {
            .x = 0,
            .y = 0,
            .width = (float)displaySize.width,
            .height = (float)displaySize.height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
    };

    VkRect2D scissor = {
            .offset = {.x = 0, .y = 0,},
            .extent = displaySize,
    };
    // Specify viewport info
    VkPipelineViewportStateCreateInfo viewportInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .viewportCount = 1,
            .pViewports = &viewports,
            .scissorCount = 1,
            .pScissors = &scissor,
    };

    // Specify multisample info
    VkSampleMask sampleMask = ~0u;
    VkPipelineMultisampleStateCreateInfo multisampleInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 0,
            .pSampleMask = &sampleMask,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE,
    };

    // Specify color blend state
    VkPipelineColorBlendAttachmentState attachmentStates{
            .blendEnable = VK_FALSE,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };
    VkPipelineColorBlendStateCreateInfo colorBlendInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &attachmentStates,
    };

    // Specify rasterizer info
    VkPipelineRasterizationStateCreateInfo rasterInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1,
    };

    // Specify input assembler state
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,//FIXME I am triangle strip only
            .primitiveRestartEnable = VK_FALSE,
    };

    // Specify vertex input state
    VkVertexInputBindingDescription vertex_input_bindings{
            .binding = 0,
            .stride = 5 * sizeof(float),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };
    VkVertexInputAttributeDescription vertex_input_attributes[2]{
            {
                    .location = 0,
                    .binding = 0,
                    .format = VK_FORMAT_R32G32B32_SFLOAT,
                    .offset = 0,
            },
            {
                    .location = 1,
                    .binding = 0,
                    .format = VK_FORMAT_R32G32_SFLOAT,
                    .offset = sizeof(float) * 3,
            }};
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &vertex_input_bindings,
            .vertexAttributeDescriptionCount = 2,
            .pVertexAttributeDescriptions = vertex_input_attributes,
    };

    // Create the pipeline cache
    VkPipelineCacheCreateInfo pipelineCacheInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,  // reserved, must be 0
            .initialDataSize = 0,
            .pInitialData = nullptr,
    };

    LOGI("vkCreatePipelineCache");
    CALL_VK(vkCreatePipelineCache(device_, &pipelineCacheInfo, nullptr,
                                  &cache_));

    // Create the pipeline
    VkGraphicsPipelineCreateInfo pipelineCreateInfo{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount = 2,
            .pStages = shaderStages,
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssemblyInfo,
            .pTessellationState = nullptr,
            .pViewportState = &viewportInfo,
            .pRasterizationState = &rasterInfo,
            .pMultisampleState = &multisampleInfo,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &colorBlendInfo,
            .pDynamicState = &dynamicStateInfo,
            .layout = layout_,
            .renderPass = renderPass,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = 0,
    };

    LOGI("vkCreateGraphicsPipelines");
    VkResult pipelineResult = vkCreateGraphicsPipelines(
            device_, cache_, 1, &pipelineCreateInfo, nullptr,
            &pipeline_);

    // We don't need the shaders anymore, we can release their memory
    vkDestroyShaderModule(device_, vertexShader, nullptr);
    vkDestroyShaderModule(device_, fragmentShader, nullptr);

    return pipelineResult;
}

// initialize descriptor set
VkResult GraphicsPipe::CreateDescriptorSet(std::vector<Texture*> textures) {
    const VkDescriptorPoolSize type_count = {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = (uint32_t)textures.size(),
    };
    const VkDescriptorPoolCreateInfo descriptor_pool = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .maxSets = 1,
            .poolSizeCount = 1,//???
            .pPoolSizes = &type_count,
    };

    CALL_VK(vkCreateDescriptorPool(device_, &descriptor_pool, nullptr,
                                   &descPool_));

    VkDescriptorSetAllocateInfo alloc_info{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool = descPool_,
            .descriptorSetCount = 1,
            .pSetLayouts = &dscLayout_};
    CALL_VK(vkAllocateDescriptorSets(device_, &alloc_info,
                                     &descSet_));

    VkDescriptorImageInfo texDsts[textures.size()];
    memset(texDsts, 0, sizeof(texDsts));
    for (int32_t idx = 0; idx < textures.size(); idx++) {
        texDsts[idx].sampler = textures[idx]->sampler_;
        texDsts[idx].imageView = textures[idx]->view_;
        texDsts[idx].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    }

    VkWriteDescriptorSet writeDst{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = descSet_,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = (uint32_t)textures.size(),
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = texDsts,
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr};
    vkUpdateDescriptorSets(device_, 1, &writeDst, 0, nullptr);
    return VK_SUCCESS;
}

void GraphicsPipe::DeleteGraphicsPipeline(void) {
    if (pipeline_ == VK_NULL_HANDLE) return;
    vkDestroyPipeline(device_, pipeline_, nullptr);
    vkDestroyPipelineCache(device_, cache_, nullptr);
    vkFreeDescriptorSets(device_, descPool_, 1,
                         &descSet_);
    vkDestroyDescriptorPool(device_, descPool_, nullptr);
    vkDestroyPipelineLayout(device_, layout_, nullptr);
}

GraphicsPipe::~GraphicsPipe() {
    this->DeleteGraphicsPipeline();
}

GraphicsPipe::GraphicsPipe(VkDevice &device) {
    this->device_ = device;
}
