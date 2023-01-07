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

#include <android/log.h>
#include <cassert>
#include <vector>
#include "vulkan_wrapper.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb/stb_image.h>
#include "FileUtils.h"
#include "VulkanMain.hpp"
#include "Renderer.h"
#include "Texture.h"
#include "Object.h"



#define TUTORIAL_TEXTURE_COUNT 1
const char* texFiles[TUTORIAL_TEXTURE_COUNT] = {
    "sample_tex.png",
};
Texture* textures[TUTORIAL_TEXTURE_COUNT];

struct VulkanBufferInfo {
  VkBuffer vertexBuf_;
};
VulkanBufferInfo buffers;



// Android Native App pointer...
android_app* androidAppCtx = nullptr;

Renderer* renderer;


void CreateTexture(void) {
  for (uint32_t i = 0; i < TUTORIAL_TEXTURE_COUNT; i++) {

    // Read the file:
    //FIXME: bring to outside
    std::vector<char> fileContent = FileUtils::getFileFromApp(androidAppCtx, texFiles[i]);

    uint32_t imgWidth, imgHeight, n;
    unsigned char* imageData = stbi_load_from_memory(
            reinterpret_cast<const stbi_uc *>(fileContent.data()), fileContent.size(), reinterpret_cast<int*>(&imgWidth),
            reinterpret_cast<int*>(&imgHeight), reinterpret_cast<int*>(&n), 4);
    //only 4 channels
    assert(n == 4);
    assert(imageData != NULL);
    textures[i] = new Texture(renderer->getDevice());

    VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    VkFlags required_props = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    textures[i]->setData(imageData,imgWidth,imgHeight, usage,required_props);

    stbi_image_free(imageData);
  }
}


// Vertex positions + uv interlace
const float vertexData[] = {
        1.0f,-1.0f, 0.0f, 1.0f,0.0f,
        -1.0f, -1.0f, 0.0f,0.0f, 0.0f,
        1.0f, 1.0f,0.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,0.0f, 0.0f, 1.0f,
};

Object triangle(vertexData,sizeof(vertexData)/sizeof(float), 5);
Material material;




// InitVulkan:
//   Initialize Vulkan Context when android application window is created
//   upon return, vulkan is ready to draw frames
bool InitVulkan(android_app* app) {
  androidAppCtx = app;

  if (!InitVulkan()) {
    LOGW("Vulkan is unavailable, install vulkan and re-start");
    return false;
  }

  VkApplicationInfo appInfo = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = "VAETest",
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "VAE",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = VK_MAKE_VERSION(1, 0, 0),
  };

  // create a device

  LOGI("Create Render");
  renderer = new Renderer(app->window, &appInfo);

  LOGI("Create Texture");
  CreateTexture();
  material.vertString = FileUtils::getFileFromApp(androidAppCtx, "shaders/tri.vert");
  material.fragString = FileUtils::getFileFromApp(androidAppCtx, "shaders/tri.frag");


  // Create graphics pipeline - Temp
    material.map = textures[0];

  return true;
}

void DeleteVulkan() {
  delete  renderer;
}
// Draw one frame
bool VulkanUpdate(void) {

  LOGI("prerender");
  //TODO scene graph
  renderer->preRender(triangle,material);

  LOGI("render");
  renderer->render(triangle,material);

  LOGI("swapbuffer");
  renderer->swapbuffer();

  return true;
}

