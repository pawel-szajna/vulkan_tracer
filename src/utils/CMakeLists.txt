add_library(
    vulkan_tracer_utils
    STATIC
        src/Helpers.cpp
        src/Timers.cpp
)

add_library(
    vktracer::utils
    ALIAS vulkan_tracer_utils
)

set_target_properties(
    vulkan_tracer_utils
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

target_include_directories(
    vulkan_tracer_utils
    PRIVATE
        include/utils
    PUBLIC
        include
)

target_link_libraries(
    vulkan_tracer_utils
    PUBLIC
        vktracer::types
        vktracer::generated::interfaces
    PRIVATE
        spdlog::spdlog
)
