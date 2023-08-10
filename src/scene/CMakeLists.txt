add_library(
    vulkan_tracer_scene
    STATIC
        src/Scene.cpp
        src/Reader.cpp
)

add_library(
    vktracer::scene
    ALIAS vulkan_tracer_scene
)

set_target_properties(
    vulkan_tracer_scene
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

target_include_directories(
    vulkan_tracer_scene
    PRIVATE
        include/scene
    PUBLIC
        include
)

target_link_libraries(
    vulkan_tracer_scene
    PUBLIC
        vktracer::types
    PRIVATE
        vktracer::utils
        vktracer::generated::interfaces
        spdlog::spdlog
)