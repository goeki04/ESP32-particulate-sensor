if(glslang_EXECUTABLE)
    set(GLSLANG_VALIDATOR ${glslang_EXECUTABLE})
elseif(DEFINED glslang_BIN_DIRS)
    set(GLSLANG_VALIDATOR "${glslang_BIN_DIRS}/glslangValidator${CMAKE_EXECUTABLE_SUFFIX}")
else()
    find_program(GLSLANG_VALIDATOR NAMES glslangValidator REQUIRED)
endif()

if(directx-shader-compiler_EXECUTABLE)
    set(DXC_EXECUTABLE ${directx-shader-compiler_EXECUTABLE})
elseif(DEFINED directx-shader-compiler_BIN_DIRS)
    set(DXC_EXECUTABLE "${directx-shader-compiler_BIN_DIRS}/dxc${CMAKE_EXECUTABLE_SUFFIX}")
else()
    find_program(DXC_EXECUTABLE NAMES dxc REQUIRED)
endif()

message(STATUS "Andromeda Shader Pipeline -> Validator found at: ${GLSLANG_VALIDATOR}")
message(STATUS "Andromeda Shader Pipeline -> DXC found at: ${DXC_EXECUTABLE}")

function(compile_hlsl_shader)
    cmake_parse_arguments(ARG "" "TARGET;SOURCE;PROFILE;OUT_DIR;OUT_SPV_LIST" "" ${ARGN})

    if(NOT ARG_OUT_DIR)
        set(ARG_OUT_DIR "${CMAKE_BINARY_DIR}/shaders/spirv")
    endif()

    file(MAKE_DIRECTORY ${ARG_OUT_DIR})

    get_filename_component(SHADER_NAME ${ARG_SOURCE} NAME)

    # Unlike CONFIGURE_DEPENDS on a glob (which only catches files being
    # added/removed), the kernel list here comes from this file's *contents*.
    # Registering it as a configure-time dependency makes CMake reconfigure
    # (re-scan for [numthreads] entry points) whenever it's edited - e.g. a
    # kernel gets renamed - not just when the build re-runs DXC on it.
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${ARG_SOURCE})

    # Wipe any .spv from a previous configure for this source before rebuilding
    # the list below. Without this, renaming/removing a kernel would leave its
    # old <SHADER_NAME>.<oldEntry>.spv orphaned on disk - Ninja just stops
    # tracking it, it doesn't get deleted on its own.
    file(GLOB STALE_SPV "${ARG_OUT_DIR}/${SHADER_NAME}.*.spv")
    if(STALE_SPV)
        file(REMOVE ${STALE_SPV})
    endif()

    # Kernel entry points are auto-detected from the source instead of being
    # listed manually: every function annotated with [numthreads(...)] is
    # compiled as its own DXC invocation (-E <name>), producing one .spv per
    # kernel (<SHADER_NAME>.<ENTRY_NAME>.spv). Adding a new kernel to the file
    # is enough - no CMakeLists.txt changes required.
    file(READ ${ARG_SOURCE} HLSL_CONTENT)
    string(REGEX MATCHALL "\\[numthreads\\([^)]*\\)\\][ \t\r\n]*void[ \t\r\n]+[A-Za-z_][A-Za-z0-9_]*" KERNEL_MATCHES "${HLSL_CONTENT}")

    set(SPV_OUTPUTS "")

    foreach(MATCH ${KERNEL_MATCHES})
        string(REGEX REPLACE ".*void[ \t\r\n]+([A-Za-z_][A-Za-z0-9_]*)" "\\1" ENTRY_NAME "${MATCH}")
        set(SPV_OUT "${ARG_OUT_DIR}/${SHADER_NAME}.${ENTRY_NAME}.spv")
        add_custom_command(
            OUTPUT ${SPV_OUT}
            COMMAND ${DXC_EXECUTABLE} -spirv -T ${ARG_PROFILE} -E ${ENTRY_NAME} -Fo ${SPV_OUT} ${ARG_SOURCE}
            DEPENDS ${ARG_SOURCE}
            COMMENT "Andromeda Compiler (DXC): ${SHADER_NAME} [${ENTRY_NAME}] -> ${SHADER_NAME}.${ENTRY_NAME}.spv"
            VERBATIM
        )
        list(APPEND SPV_OUTPUTS ${SPV_OUT})
    endforeach()

    add_custom_target(${ARG_TARGET}_hlsl_shaders ALL DEPENDS ${SPV_OUTPUTS})
    add_dependencies(${ARG_TARGET} ${ARG_TARGET}_hlsl_shaders)

    if(ARG_OUT_SPV_LIST)
        set(${ARG_OUT_SPV_LIST} ${SPV_OUTPUTS} PARENT_SCOPE)
    endif()
endfunction()

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
            message(FATAL_ERROR "Andromeda Compiler Error: Unknown shader extension ${SHADER_EXT} on file ${SHADER_NAME}! (HLSL sources are handled separately via compile_hlsl_shader())")
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