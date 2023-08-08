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

find_package(argparse REQUIRED)
find_package(spdlog REQUIRED)
find_package(yaml-cpp REQUIRED)

find_program(PYTHON_INTERPRETER NAMES python3 python REQUIRED)
find_program(GLSL_COMPILER NAMES glslang glslangValidator glslc REQUIRED)
message(STATUS "Found GLSL compiler: ${GLSL_COMPILER}")

get_filename_component(GLSL_COMPILER_NAME ${GLSL_COMPILER} NAME_WE)

set(INTERFACE_DIR "${CMAKE_BINARY_DIR}/generated")

set(GLSL_SOURCES
    shaders/main.glsl)

file(GLOB_RECURSE GLSL_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/shaders/*.glsl")
foreach(GLSL_SOURCE ${GLSL_SOURCES})
    get_filename_component(FILE_NAME ${GLSL_SOURCE} NAME_WE)
    set(SPIRV_COMPILED "${CMAKE_BINARY_DIR}/${FILE_NAME}.spv")
    set(PREPROCESSED "${CMAKE_BINARY_DIR}/${FILE_NAME}_preprocessed.glsl")
    if(${GLSL_COMPILER_NAME} STREQUAL "glslc")
        set(GLSL_ARGS -fshader-stage=compute -o ${SPIRV_COMPILED} ${PREPROCESSED})
    else()
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(GLSL_ARGS -gVS -V -S comp -o ${SPIRV_COMPILED} ${PREPROCESSED})
        else()
            set(GLSL_ARGS -V -S comp -o ${SPIRV_COMPILED} ${PREPROCESSED})
        endif()
    endif()
    add_custom_command(
        OUTPUT "${SPIRV_COMPILED}"
        COMMAND ${PYTHON_INTERPRETER} ARGS "${CMAKE_SOURCE_DIR}/tool/glsl_preprocessor.py" "${CMAKE_SOURCE_DIR}/${GLSL_SOURCE}" "${PREPROCESSED}" "${INTERFACE_DIR}"
        COMMAND ${CMAKE_SOURCE_DIR}/tool/wrap_glsl_compiler ARGS ${GLSL_COMPILER} ${GLSL_ARGS}
        DEPENDS ${GLSL_FILES}
    )
    list(APPEND SHADER_BINARIES ${SPIRV_COMPILED})
endforeach()

add_custom_target(
    shaders
    DEPENDS
        ${SHADER_BINARIES}
        interfaces
)

add_library(
    shaders_dummy
    STATIC
        ${GLSL_FILES}
)

set_target_properties(
    shaders_dummy
    PROPERTIES
        EXCLUDE_FROM_ALL TRUE
        LINKER_LANGUAGE CXX
)

target_include_directories(
    shaders_dummy
    PRIVATE
        shaders
        ${CMAKE_BINARY_DIR}/generated
)

file(GLOB INTERFACE_DEFINITIONS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/interface/*.json")
foreach(INTERFACE_DEFINITION ${INTERFACE_DEFINITIONS})
    get_filename_component(FILE_NAME ${INTERFACE_DEFINITION} NAME_WE)
    set(INTERFACE_CPP "${INTERFACE_DIR}/${FILE_NAME}.hpp")
    set(INTERFACE_GLSL "${INTERFACE_DIR}/${FILE_NAME}.glsl")
    add_custom_command(
        OUTPUT "${INTERFACE_CPP}"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${INTERFACE_DIR}
        COMMAND ${PYTHON_INTERPRETER} ARGS "${CMAKE_SOURCE_DIR}/tool/interface_generator.py" ${INTERFACE_DEFINITION} ${INTERFACE_DIR} cpp
        DEPENDS ${INTERFACE_DEFINITION}
    )
    add_custom_command(
        OUTPUT "${INTERFACE_GLSL}"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${INTERFACE_DIR}
        COMMAND ${PYTHON_INTERPRETER} ARGS "${CMAKE_SOURCE_DIR}/tool/interface_generator.py" ${INTERFACE_DEFINITION} ${INTERFACE_DIR} glsl
        DEPENDS ${INTERFACE_DEFINITION}
    )
    list(APPEND INTERFACE_FILES ${INTERFACE_CPP})
    list(APPEND INTERFACE_FILES ${INTERFACE_GLSL})
endforeach()

add_custom_target(
    interfaces
    DEPENDS ${INTERFACE_FILES}
)

add_executable(
    vulkan_tracer
        src/helpers.cpp
        src/main.cpp
        src/runner.cpp
        src/scene.cpp
        src/scene_reader.cpp
        src/timers.cpp
        src/vulkan_compute.cpp
)

add_dependencies(
    vulkan_tracer
        interfaces
        shaders
)

target_include_directories(
    vulkan_tracer
    PRIVATE
        include
        externals
        ${Vulkan_INCLUDE_DIRS}
        ${Vulkan_INCLUDE_DIRS_Extra}
        ${INTERFACE_DIR}
)

target_link_directories(
    vulkan_tracer
    PRIVATE
        ${Vulkan_LIBRARY_DIRS}
)

target_link_libraries(
    vulkan_tracer
    PRIVATE
        argparse::argparse
        spdlog::spdlog
        yaml-cpp
)

if(APPLE)
    target_link_libraries(
        vulkan_tracer
        PRIVATE
            MoltenVK
    )
else()
    target_link_libraries(
        vulkan_tracer
        PRIVATE
            Vulkan::Vulkan
    )
endif()

target_compile_definitions(
    vulkan_tracer
    PRIVATE
        SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(
        vulkan_tracer
        PRIVATE
            DebugBuild
    )
endif()
