//
// Created by HuongNguyenXuan on 8/6/2021.
//

#ifndef VULKAN_ANDROID_ENGINE_GRAPHICSPIPE_H
#define VULKAN_ANDROID_ENGINE_GRAPHICSPIPE_H

#include "vulkan_wrapper.h"
#include "Log.hpp"
#include "Texture.h"

class GraphicsPipe {

private:
    VkDevice device_;

    //info
    VkDescriptorSetLayout dscLayout_;
    VkDescriptorPool descPool_;
    VkPipelineCache cache_;
public:
    ~GraphicsPipe();
    VkResult CreateGraphicsPipeline(VkShaderModule vertexShader, VkShaderModule fragmentShader,
                                    uint32_t textureCount, VkExtent2D displaySize,
                                    VkRenderPass renderPass);
    void DeleteGraphicsPipeline(void);

    VkResult CreateDescriptorSet(std::vector<Texture*> textures);

    VkPipeline pipeline_;
    VkPipelineLayout layout_;
    VkDescriptorSet descSet_;

    GraphicsPipe(VkDevice& device);
};


#endif //VULKAN_ANDROID_ENGINE_GRAPHICSPIPE_H
