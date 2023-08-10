cmake_minimum_required(VERSION 3.24)
project(vulkan_Test)

set(CMAKE_CXX_STANDARD 23)

if(APPLE)
    set(SDK_PATH "/opt/homebrew/Cellar/molten-vk/1.2.4")
    set(Vulkan_INCLUDE_DIRS "${SDK_PATH}/include")
    set(Vulkan_INCLUDE_DIRS_Extra "${SDK_PATH}/libexec/include")
    set(Vulkan_LIBRARY_DIRS "${SDK_PATH}/lib")
else()
    find_package(Vulkan REQUIRED)
endif()

option(LIVE_VIEW "Enable rendering live preview")

find_package(argparse REQUIRED)
find_package(spdlog REQUIRED)
find_package(yaml-cpp REQUIRED)

find_program(PYTHON_INTERPRETER NAMES python3 python REQUIRED)
find_program(GLSL_COMPILER NAMES glslang glslangValidator glslc REQUIRED)
message(STATUS "Found GLSL compiler: ${GLSL_COMPILER}")
get_filename_component(GLSL_COMPILER_NAME ${GLSL_COMPILER} NAME_WE)

set(EXTERNALS_DIR "${CMAKE_SOURCE_DIR}/externals")

add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)

add_subdirectory(interface)
add_subdirectory(src)


