//
// Created by HuongNguyenXuan on 8/8/2021.
//

#ifndef VULKAN_ANDROID_ENGINE_OBJECT_H
#define VULKAN_ANDROID_ENGINE_OBJECT_H

#include <vector>
#include "Log.hpp"
#include "vulkan_wrapper.h"

class Object {

public:
    Object(const float vertices[], uint32_t size, uint32_t stride);
    std::vector<float> vertices;
    VkBuffer vkBuffer;


    uint32_t stride_;
};


#endif //VULKAN_ANDROID_ENGINE_OBJECT_H
