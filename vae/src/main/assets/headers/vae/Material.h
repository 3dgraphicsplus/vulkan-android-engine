//
// Created by HuongNguyenXuan on 8/13/2021.
//

#ifndef VULKAN_ANDROID_ENGINE_MATERIAL_H
#define VULKAN_ANDROID_ENGINE_MATERIAL_H

#include "vulkan_wrapper.h"
#include "Texture.h"

class Material {
public:
    Material();
    VkShaderModule vertexShader, fragmentShader;
    Texture* map = NULL;
    std::vector<char> vertString;
    std::vector<char> fragString;
};


#endif //VULKAN_ANDROID_ENGINE_MATERIAL_H
