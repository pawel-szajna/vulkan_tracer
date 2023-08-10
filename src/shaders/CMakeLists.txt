set(GLSL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.glsl)
set(INTERFACE_DIR "${CMAKE_BINARY_DIR}/generated")

file(GLOB_RECURSE GLSL_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.glsl")
foreach(GLSL_SOURCE ${GLSL_SOURCES})
    get_filename_component(FILE_NAME ${GLSL_SOURCE} NAME_WE)
    set(SPIRV_COMPILED "${CMAKE_BINARY_DIR}/bin/${FILE_NAME}.spv")
    set(PREPROCESSED "${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}_preprocessed.glsl")
    if(${GLSL_COMPILER_NAME} STREQUAL "glslc")
        set(GLSL_ARGS -fshader-stage=compute -o ${SPIRV_COMPILED} ${PREPROCESSED})
    else()
        set(GLSL_ARGS -V -S comp -o ${SPIRV_COMPILED} ${PREPROCESSED})
    endif()
    add_custom_command(
        OUTPUT "${SPIRV_COMPILED}"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${PYTHON_INTERPRETER} ARGS "${CMAKE_SOURCE_DIR}/tool/glsl_preprocessor.py" "${GLSL_SOURCE}" "${PREPROCESSED}" "${INTERFACE_DIR}"
        COMMAND ${CMAKE_SOURCE_DIR}/tool/wrap_glsl_compiler ARGS ${GLSL_COMPILER} ${GLSL_ARGS}
        DEPENDS ${GLSL_FILES}
    )
    list(APPEND SHADER_BINARIES ${SPIRV_COMPILED})
endforeach()

add_custom_target(
    shaders
    DEPENDS
        ${SHADER_BINARIES}
        vktracer::generated::interfaces
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
