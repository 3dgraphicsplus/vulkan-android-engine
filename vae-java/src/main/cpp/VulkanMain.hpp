// Copyright 2016 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef __VULKANMAIN_HPP__
#define __VULKANMAIN_HPP__

// Initialize vulkan device context
// after return, vulkan is ready to draw
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/asset_manager.h>

#include <string>

bool InitVulkan(ANativeWindow* app);

// delete vulkan device context when application goes away
void DeleteVulkan(void);

void SetShader(std::string ver, std::string frag);

// Pre render operation
void PreRender();

// Ask Vulkan to Render a frame
bool VulkanDrawFrame(void);

//
void CreateTexture(AAssetManager* assetManager, std::string path);

#endif // __VULKANMAIN_HPP__


