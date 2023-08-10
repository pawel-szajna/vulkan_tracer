add_library(
    vulkan_tracer_preview
    STATIC
        src/live_view.cpp
)

add_library(
    vktracer::preview
    ALIAS vulkan_tracer_preview
)

set_target_properties(
    vulkan_tracer_preview
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

target_include_directories(
    vulkan_tracer_preview
    PRIVATE
        include/preview
    PUBLIC
        include
)

target_link_libraries(
    vulkan_tracer_preview
    PRIVATE
        vktracer::utils
        vktracer::runner
        vktracer::scene
        spdlog::spdlog
)

if(LIVE_VIEW)
    find_package(SDL2 REQUIRED COMPONENTS SDL2)
    target_link_libraries(
        vulkan_tracer_preview
        PRIVATE
            ${SDL2_LIBRARIES}
    )
    target_compile_definitions(
        vulkan_tracer_preview
        PRIVATE
            HAS_LIVE_VIEW
    )
endif()
