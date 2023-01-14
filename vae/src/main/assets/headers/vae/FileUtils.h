/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef TUTORIAL06_TEXTURE_CREATESHADERMODULE_H
#define TUTORIAL06_TEXTURE_CREATESHADERMODULE_H

#include <string>
#include <vulkan_wrapper.h>
#if USE_NATIVE_ACTIVITY
#include <android_native_app_glue.h>
#endif
#include <shaderc/shaderc.h>

#include<android/native_window.h>

#include <android/asset_manager.h>

#define n2s(val) std::to_string(val)

namespace FileUtils {

    /**
     *
     * @param appInfo
     * @param filePath
     * @return  Get file buffer using native android app
     */
#if USE_NATIVE_ACTIVITY
    std::string getFileFromApp(
            android_app *appInfo,
            const char *filePath);
#endif

    /**
     *
     * @param appInfo
     * @param filePath
     * @return Get file buffer using asset manager, for java+jni android app
     */
    std::string getFileFromAsset(
            AAssetManager *appInfo,
            const char *filePath);

    /**
     *
     * @param bits
     * @return Get shader name
     */
    shaderc_shader_kind getShadercShaderType(VkShaderStageFlagBits bits);

    /**
     *
     * @param source
     * @param placeholder
     * @param value
     */
    void setPlaceholder(std::string& source, const std::string placeholder, const std::string value);
}
#endif // TUTORIAL06_TEXTURE_CREATESHADERMODULE_H
