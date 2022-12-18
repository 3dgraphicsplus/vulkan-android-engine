cmake_minimum_required(VERSION 3.18.1)

project(vulkan_android_engine_test)
set(CMAKE_CXX_STANDARD 17)

# build native_app_glue as a static lib
add_library(native_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
# build Vulkan app
set(SRC_DIR ${CMAKE_SOURCE_DIR})

include_directories(
        ./
        libs/headers/vae
        libs/headers/common
        libs/headers/common/src
        libs/headers/common/vulkan_wrapper
        libs/headers/third_party
        libs/shaderc/include
   ${ANDROID_NDK}/sources/android/native_app_glue)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
       -std=c++17 -Wall -Wno-unused-variable \
       -Wno-delete-non-virtual-dtor -DVK_USE_PLATFORM_ANDROID_KHR")

if (${ANDROID_ABI} STREQUAL "armeabi-v7a")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
       -mhard-float -D_NDK_MATH_NO_SOFTFP=1 -mfloat-abi=hard")
   set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} \
       -Wl,--no-warn-mismatch")
endif()
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

link_directories(libs/shaderc/libs/${ANDROID_STL}/${ANDROID_ABI})
link_directories(../jniLibs/${ANDROID_ABI}/)

message(WARN "LIBRARY " libs/shaderc/libs/${ANDROID_STL}/${ANDROID_ABI})

message(WARN "LIBRARY " ../jniLibs/${ANDROID_ABI}/)

#Never let it up before you declare enough dependencies details
add_library(vaetest SHARED
        ${SRC_DIR}/AndroidMain.cpp
        ${SRC_DIR}/VulkanMain.cpp
        )

target_link_libraries(vaetest vae native_glue log android shaderc)
