cmake_minimum_required(VERSION 3.18.1)

project(vulkan_android_engine_test_java)
set(CMAKE_CXX_STANDARD 17)

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
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
       -std=c++17 -Wall -Wno-unused-variable \
       -Wno-delete-non-virtual-dtor -DVK_USE_PLATFORM_ANDROID_KHR")

if (${ANDROID_ABI} STREQUAL "armeabi-v7a")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
       -mhard-float -D_NDK_MATH_NO_SOFTFP=1 -mfloat-abi=hard")
   set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} \
       -Wl,--no-warn-mismatch")
endif()

link_directories(libs/shaderc/libs/${ANDROID_STL}/${ANDROID_ABI})
link_directories(../jniLibs/${ANDROID_ABI}/)

message(WARN "LIBRARY " libs/shaderc/libs/${ANDROID_STL}/${ANDROID_ABI})

message(WARN "LIBRARY " ../jniLibs/${ANDROID_ABI}/)

#Never let it up before you declare enough dependencies details
add_library(vaetestjava SHARED
        ${SRC_DIR}/AndroidMain.cpp
        ${SRC_DIR}/VulkanMain.cpp
        )

target_link_libraries(vaetestjava vae log android shaderc)
