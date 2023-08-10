set(INTERFACE_DIR "${CMAKE_BINARY_DIR}/generated")

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
    interfaces_generate
        DEPENDS ${INTERFACE_FILES}
)

add_library(
    vulkan_tracer_interfaces
    INTERFACE
)

add_library(
    vktracer::generated::interfaces
    ALIAS vulkan_tracer_interfaces
)

add_dependencies(
    vulkan_tracer_interfaces
        interfaces_generate
)

target_include_directories(
    vulkan_tracer_interfaces
    INTERFACE
        ${INTERFACE_DIR}
)
