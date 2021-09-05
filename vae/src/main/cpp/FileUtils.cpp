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

#include "FileUtils.h"
#include <android/log.h>
namespace FileUtils {
// Create VK shader module from given glsl shader file
// filePath: glsl shader file (including path ) in APK's asset folder
    std::vector<char> getBufferFromFile(android_app *appInfo, const char *filePath) {
      // read file from Assets
      AAsset *file = AAssetManager_open(appInfo->activity->assetManager, filePath,
                                        AASSET_MODE_BUFFER);
      size_t len = AAsset_getLength(file);
      assert(len > 0);
      std::vector<char> buffer;
      buffer.resize(len);

      AAsset_read(file, static_cast<void *>(buffer.data()), len);
      AAsset_close(file);
      return buffer;
    }
}
