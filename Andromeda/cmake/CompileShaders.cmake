if(glslang_EXECUTABLE)
    set(GLSLANG_VALIDATOR ${glslang_EXECUTABLE})
elseif(DEFINED glslang_BIN_DIRS)
    set(GLSLANG_VALIDATOR "${glslang_BIN_DIRS}/glslangValidator${CMAKE_EXECUTABLE_SUFFIX}")
else()
    find_program(GLSLANG_VALIDATOR NAMES glslangValidator REQUIRED)
endif()

message(STATUS "Andromeda Shader Pipeline -> Validator found at: ${GLSLANG_VALIDATOR}")

function(compile_shaders)
    cmake_parse_arguments(ARG "" "TARGET;OUT_DIR" "SHADERS" ${ARGN})
    
    if(NOT ARG_OUT_DIR)
        set(ARG_OUT_DIR "${CMAKE_BINARY_DIR}/shaders/spirv")
    endif()
    
    file(MAKE_DIRECTORY ${ARG_OUT_DIR})
    
    set(SPV_OUTPUTS "")

    foreach(SHADER ${ARG_SHADERS})
        get_filename_component(SHADER_NAME ${SHADER} NAME)
        
        get_filename_component(SHADER_EXT ${SHADER} EXT)
        
        set(SPV_OUT "${ARG_OUT_DIR}/${SHADER_NAME}.spv")
        
        if("${SHADER_EXT}" STREQUAL ".vert")
            set(SHADER_STAGE_FLAG "vert")
        elseif("${SHADER_EXT}" STREQUAL ".frag")
            set(SHADER_STAGE_FLAG "frag")
        elseif("${SHADER_EXT}" STREQUAL ".comp")
            set(SHADER_STAGE_FLAG "comp")
        else()
            message(FATAL_ERROR "Andromeda Compiler Error: Unknown shader extension ${SHADER_EXT} on file ${SHADER_NAME}!")
        endif()
        
        add_custom_command(
            OUTPUT  ${SPV_OUT}
            COMMAND ${GLSLANG_VALIDATOR}
                    -G                          
                    -S ${SHADER_STAGE_FLAG}
                    -o ${SPV_OUT}
                    ${SHADER}
            DEPENDS ${SHADER}
            COMMENT "Andromeda Compiler: ${SHADER_NAME} -> ${SHADER_NAME}.spv"
            VERBATIM
        )
        list(APPEND SPV_OUTPUTS ${SPV_OUT})
    endforeach()
    
    add_custom_target(${ARG_TARGET}_shaders ALL DEPENDS ${SPV_OUTPUTS})
    add_dependencies(${ARG_TARGET} ${ARG_TARGET}_shaders)
    
    target_compile_definitions(${ARG_TARGET} PRIVATE
        SHADER_SPV_PATH="${ARG_OUT_DIR}/"
    )
endfunction()