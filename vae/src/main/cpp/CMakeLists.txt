cmake_minimum_required(VERSION 3.18.1)

project(vulkan_android_engine)
set(CMAKE_CXX_STANDARD 17)

message(WARN  "Start compiling vae")


add_definitions(-DUSE_NATIVE_ACTIVITY=1)

# build Vulkan Android Engine
set(SRC_DIR ${CMAKE_SOURCE_DIR})
get_filename_component(REPO_ROOT_DIR
   ${CMAKE_SOURCE_DIR}/../../../..  ABSOLUTE)
set(COMMON_DIR ${REPO_ROOT_DIR}/common)
set(THIRD_PARTY_DIR ${REPO_ROOT_DIR}/third_party)


#check env
message(WARN    "REPO_ROOT_DIR " ${REPO_ROOT_DIR})
message(WARN    "CMAKE_SOURCE_DIR " ${CMAKE_SOURCE_DIR})
message(WARN    "THIRD_PARTY_DIR " ${THIRD_PARTY_DIR})
message(WARN    "COMMON_DIR " ${COMMON_DIR})


include_directories(
        ./headers
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

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/shaderc/libs/${ANDROID_STL}/${ANDROID_ABI}/)
message(WARN "library " ${CMAKE_LIBRARY_PATH})

add_library(vae SHARED
   ${SRC_DIR}/FileUtils.cpp
   ${COMMON_DIR}/vulkan_wrapper/vulkan_wrapper.cpp
        ${SRC_DIR}/Renderer.cpp
        ${SRC_DIR}/SwapChain.cpp
        ${SRC_DIR}/CommandPool.cpp
        ${SRC_DIR}/Device.cpp
        ${SRC_DIR}/Texture.cpp
        ${SRC_DIR}/GraphicsPipe.cpp
        ${SRC_DIR}/Object.cpp
        ${SRC_DIR}/Material.cpp)


target_link_libraries(vae log android shaderc)
