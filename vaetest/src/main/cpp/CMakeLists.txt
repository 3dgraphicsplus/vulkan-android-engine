cmake_minimum_required(VERSION 3.16)

project(vulkan_android_engine_test)
set(CMAKE_CXX_STANDARD 17)

# build native_app_glue as a static lib
add_library(native_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
# build Vulkan app
set(SRC_DIR ${CMAKE_SOURCE_DIR})
get_filename_component(REPO_ROOT_DIR
   ${CMAKE_SOURCE_DIR}/../../../..  ABSOLUTE)
set(COMMON_DIR ${REPO_ROOT_DIR}/common)
set(THIRD_PARTY_DIR ${REPO_ROOT_DIR}/third_party)

include_directories(
        ${REPO_ROOT_DIR}/vae/src/main/cpp
   ${COMMON_DIR}/vulkan_wrapper
   ${COMMON_DIR}/src
   ${THIRD_PARTY_DIR}
   ${ANDROID_NDK}/sources/android/native_app_glue
   ${CMAKE_CURRENT_SOURCE_DIR}/shaderc/include)

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

link_directories(${REPO_ROOT_DIR}/vae/src/main/cpp/shaderc/libs/${ANDROID_STL}/${ANDROID_ABI})
link_directories(../jniLibs/${ANDROID_ABI}/)

message(WARN "LIBRARY " ${REPO_ROOT_DIR}/vae/src/main/cpp/shaderc/libs/${ANDROID_STL}/${ANDROID_ABI})

message(WARN "LIBRARY " ../jniLibs/${ANDROID_ABI}/)

#Never let it up before you declare enough dependencies details
add_library(vaetest SHARED
        ${SRC_DIR}/AndroidMain.cpp)

target_link_libraries(vaetest vae native_glue log android shaderc)
