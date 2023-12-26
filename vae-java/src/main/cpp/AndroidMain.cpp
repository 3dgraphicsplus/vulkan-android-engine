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
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "VulkanMain.hpp"
#include "Log.hpp"

// Android Native App pointer...
ANativeWindow* nativeWindow = nullptr;
AAssetManager* assetManager = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_threedgraphicsplus_vae_java_VulkanSurface_setSurfaceView(JNIEnv *env, jobject thiz,
                                                                       jobject surface) {
    // TODO: implement setSurfaceView()

  // The window is being shown, get it ready.
// Assume that surface is a valid Surface object
    nativeWindow = ANativeWindow_fromSurface(env, surface);
    if(!nativeWindow){
        LOGE("Cannot create native surface");
        return;
    }
    InitVulkan(nativeWindow);

}
extern "C"
JNIEXPORT void JNICALL
Java_com_threedgraphicsplus_vae_java_VulkanSurface_setShaders(JNIEnv *env, jobject thiz,
                                                                   jstring ver, jstring frag) {
    const char *verShader = env->GetStringUTFChars(ver, 0);
    const char *fragShader = env->GetStringUTFChars(frag, 0);
    LOGI("verShader %s",verShader);
    LOGI("fragShader %s",fragShader);
    SetShader(verShader, fragShader);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_threedgraphicsplus_vae_java_VulkanSurface_renderVulkan(JNIEnv *env, jobject thiz) {
    // render if vulkan is ready
    VulkanDrawFrame();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_threedgraphicsplus_vae_java_VulkanSurface_setAssetManager(JNIEnv *env, jobject thiz,
                                                                        jobject mgr) {
    // TODO: implement setAssetManager()
    assetManager = AAssetManager_fromJava(env, mgr);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_threedgraphicsplus_vae_java_VulkanSurface_createTexture(JNIEnv *env, jobject thiz,
                                                                      jstring path) {
    // TODO: implement createTexture()
    if(assetManager != nullptr) {
        CreateTexture(assetManager,env->GetStringUTFChars(path, 0));
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_threedgraphicsplus_vae_java_VulkanSurface_cleanup(JNIEnv *env, jobject thiz) {
    // TODO: implement cleanup()
    DeleteVulkan();
    ANativeWindow_release(nativeWindow);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_threedgraphicsplus_vae_java_VulkanSurface_preRender(JNIEnv *env, jobject thiz) {
    // TODO: implement preRender()
    PreRender();
}
