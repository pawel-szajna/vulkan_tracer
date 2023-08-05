cmake_minimum_required(VERSION 3.24)
project(vulkan_Test)

set(CMAKE_CXX_STANDARD 23)

set(SDK_PATH "/opt/homebrew/Cellar/molten-vk/1.2.4")
set(Vulkan_INCLUDE_DIRS "${SDK_PATH}/include")
set(Vulkan_INCLUDE_DIRS_Extra "${SDK_PATH}/libexec/include")
set(Vulkan_LIBRARY_DIRS "${SDK_PATH}/lib")

find_package(spdlog REQUIRED)
find_package(yaml-cpp REQUIRED)

set(GLSL_SOURCES
    shaders/main.glsl)

file(GLOB GLSL_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/shaders/*.glsl")
message(${GLSL_FILES})

foreach(GLSL_SOURCE ${GLSL_SOURCES})
    get_filename_component(FILE_NAME ${GLSL_SOURCE} NAME_WE)
    set(SPIRV_COMPILED "${CMAKE_BINARY_DIR}/${FILE_NAME}.spv")
    set(PREPROCESSED "${CMAKE_BINARY_DIR}/${FILE_NAME}_preprocessed.glsl")
    add_custom_command(
        OUTPUT "${SPIRV_COMPILED}"
        COMMAND python3 ARGS "${CMAKE_SOURCE_DIR}/tool/glsl_preprocessor.py" "${CMAKE_SOURCE_DIR}/${GLSL_SOURCE}" "${PREPROCESSED}"
        COMMAND glslang ARGS -V -S comp -o ${SPIRV_COMPILED} ${PREPROCESSED}
        DEPENDS ${GLSL_FILES})
    list(APPEND SHADER_BINARIES ${SPIRV_COMPILED})
endforeach()

add_custom_target(
    shaders
    DEPENDS ${SHADER_BINARIES}
)

add_executable(
    vulkan_tracer
        src/helpers.cpp
        src/main.cpp
        src/scene.cpp
        src/scene_reader.cpp
        src/vulkan_compute.cpp
)

add_dependencies(
    vulkan_tracer
        shaders
)

target_include_directories(
    vulkan_tracer
    PRIVATE
        include
        ${Vulkan_INCLUDE_DIRS}
        ${Vulkan_INCLUDE_DIRS_Extra}
)

target_link_directories(
    vulkan_tracer
    PRIVATE
        ${Vulkan_LIBRARY_DIRS}
)

target_link_libraries(
    vulkan_tracer
    PRIVATE
        spdlog::spdlog
        yaml-cpp
        MoltenVK
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(
        vulkan_tracer
        PRIVATE
            SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG
    )
endif()