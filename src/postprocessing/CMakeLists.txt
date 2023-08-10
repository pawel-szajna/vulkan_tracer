add_library(
    vulkan_tracer_postprocessing
    STATIC
        src/Approximation.cpp
        src/Executor.cpp
        src/GaussFilter.cpp
)

add_library(
    vktracer::postprocessing
    ALIAS vulkan_tracer_postprocessing
)

set_target_properties(
    vulkan_tracer_postprocessing
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

target_include_directories(
    vulkan_tracer_postprocessing
    PRIVATE
        include/postprocessing
    PUBLIC
        include
)

target_link_libraries(
    vulkan_tracer_postprocessing
    PRIVATE
        vktracer::types
        spdlog::spdlog
)