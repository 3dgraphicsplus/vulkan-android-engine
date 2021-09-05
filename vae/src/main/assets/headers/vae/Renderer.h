//
// Created by HuongNguyenXuan on 8/1/2021.
//

#ifndef VULKAN_ANDROID_ENGINE_RENDERER_H
#define VULKAN_ANDROID_ENGINE_RENDERER_H

#include "vulkan_wrapper.h"
#include "Device.h"
#include "SwapChain.h"
#include "GraphicsPipe.h"
#include "Object.h"
#include "Material.h"
#include <shaderc/shaderc.hpp>


// Translate Vulkan Shader Type to shaderc shader type
shaderc_shader_kind getShadercShaderType(VkShaderStageFlagBits type) {
    switch (type) {
        case VK_SHADER_STAGE_VERTEX_BIT:
            return shaderc_glsl_vertex_shader;
        case VK_SHADER_STAGE_FRAGMENT_BIT:
            return shaderc_glsl_fragment_shader;
        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            return shaderc_glsl_tess_control_shader;
        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            return shaderc_glsl_tess_evaluation_shader;
        case VK_SHADER_STAGE_GEOMETRY_BIT:
            return shaderc_glsl_geometry_shader;
        case VK_SHADER_STAGE_COMPUTE_BIT:
            return shaderc_glsl_compute_shader;
        default:
            __android_log_assert("invalid VKShaderStageFlagBits",
                                 "tutorial06_texture", "type = %08x", type);
    }
    return static_cast<shaderc_shader_kind>(-1);
}


class Renderer {
public:
    Renderer(ANativeWindow* platformWindow,
             VkApplicationInfo* appInfo);
    ~Renderer();
    Device* getDevice(){return device;}

    void preRender(Object& object);

    VkResult compileShader(std::vector<char> glslShader, VkShaderStageFlagBits type,VkShaderModule *shaderOut);

    void preRender(Object &object, Material &material);

    void render(Object& object, Material& material);

    bool draw();
    bool isReady();

private:

    VkRenderPass renderPass_;
    CommandPool* cmdPool;
    VkSemaphore semaphore_;
    VkFence fence_;

    GraphicsPipe* gfxPipeline;
    Device* device;
    SwapChain* swapchain;

    void CreateRenderPass(VkDevice device);
    void CreateFrameBuffers(VkRenderPass& renderPass,
    VkImageView depthView = VK_NULL_HANDLE);


    void createCommandPool();

    VkResult CreateDescriptorSet(void);

};


#endif //VULKAN_ANDROID_ENGINE_RENDERER_H
