add_library(
    vulkan_tracer_types
    INTERFACE
)

add_library(
    vktracer::types
    ALIAS vulkan_tracer_types
)

target_include_directories(
    vulkan_tracer_types
    INTERFACE
        include
)
