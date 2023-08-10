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
        ${INTERFACE_DIR}
        ${EXTERNALS_DIR}
)

target_link_libraries(
    vulkan_tracer
    PRIVATE
        vktracer::preview
        vktracer::runner
        vktracer::scene
        vktracer::utils
        vktracer::postprocessing

        argparse::argparse
        spdlog::spdlog
        yaml-cpp
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(
        vulkan_tracer
        PRIVATE
            DebugBuild
    )
endif()
