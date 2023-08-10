add_library(
    vulkan_tracer_runner
    STATIC
        src/ComputeRunner.cpp
        src/VulkanCompute.cpp
)

add_library(
    vktracer::runner
    ALIAS vulkan_tracer_runner
)

set_target_properties(
    vulkan_tracer_runner
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

target_include_directories(
    vulkan_tracer_runner
    PRIVATE
        include/runner
        ${EXTERNALS_DIR}
    PUBLIC
        include
)

target_link_libraries(
    vulkan_tracer_runner
    PUBLIC
        vktracer::types
        vktracer::utils
        vktracer::generated::interfaces
    PRIVATE
        spdlog::spdlog
)
