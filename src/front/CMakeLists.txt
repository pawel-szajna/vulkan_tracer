add_executable(
    vulkan_tracer
        src/main.cpp
)

add_dependencies(
    vulkan_tracer
        shaders
)

set_target_properties(
    vulkan_tracer
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_include_directories(
    vulkan_tracer
    PRIVATE
        include
        ${Vulkan_INCLUDE_DIRS}
        ${Vulkan_INCLUDE_DIRS_Extra}
        ${INTERFACE_DIR}
        ${EXTERNALS_DIR}
)

target_link_directories(
    vulkan_tracer
    PRIVATE
        ${Vulkan_LIBRARY_DIRS}
)

target_link_libraries(
    vulkan_tracer
    PRIVATE
        vktracer::preview
        vktracer::runner
        vktracer::scene
        vktracer::utils

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
