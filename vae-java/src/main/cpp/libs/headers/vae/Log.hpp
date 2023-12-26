//
// Created by HuongNguyenXuan on 8/1/2021.
//

#ifndef VULKAN_ANDROID_ENGINE_LOG_HPP
#define VULKAN_ANDROID_ENGINE_LOG_HPP

#include <cassert>
#include <android/log.h>

// Android log function wrappers
static const char* kTAG = "Vulkan-Android-Engine";
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, kTAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))

// Vulkan call wrapper
#define CALL_VK(func)                                                 \
  if (VK_SUCCESS != (func)) {                                         \
    __android_log_print(ANDROID_LOG_ERROR, kTAG,               \
                        "Vulkan error. File[%s], line[%d]", __FILE__, \
                        __LINE__);                                    \
    assert(false);                                                    \
  }
// A macro to check value is VK_SUCCESS
// Used also for non-vulkan functions but return VK_SUCCESS
#define VK_CHECK(x) CALL_VK(x)
#endif //VULKAN_ANDROID_ENGINE_LOG_HPP
