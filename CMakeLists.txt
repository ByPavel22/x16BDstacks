cmake_minimum_required(VERSION 3.20)
project(NexusAIEmulator VERSION 0.1.0 LANGUAGES CXX C)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(ENABLE_AI "Enable AI upscaling modules" ON)
option(ENABLE_CUDA "Enable CUDA backend for AI" OFF)
option(ENABLE_DIRECTML "Enable DirectML backend for AI" ON)
option(ENABLE_VULKAN "Enable Vulkan renderer" ON)
option(ENABLE_OPENGL "Enable OpenGL renderer" ON)
option(BUILD_TESTS "Build unit tests" ON)
option(USE_QT "Use Qt for UI" OFF)

# Platform check
if(NOT WIN32)
    message(WARNING "This project is designed for Windows. Some features may not work on other platforms.")
endif()

# =============================================================================
# Dependencies (Optional for compilation check)
# =============================================================================

set(QT_FOUND FALSE)
set(VULKAN_FOUND FALSE)
set(OPENGL_FOUND FALSE)
set(ONNX_FOUND FALSE)

if(USE_QT)
    find_package(Qt6 COMPONENTS Core Gui Widgets Quick QUIET)
    if(Qt6_FOUND)
        set(QT_FOUND TRUE)
        message(STATUS "Qt6 found - building with full UI")
    else()
        message(WARNING "Qt6 not found - building without UI")
    endif()
endif()

find_package(Vulkan QUIET)
if(Vulkan_FOUND)
    set(VULKAN_FOUND TRUE)
    message(STATUS "Vulkan found")
else()
    message(WARNING "Vulkan not found - using fallback")
endif()

find_package(OpenGL QUIET)
if(OpenGL_FOUND)
    set(OPENGL_FOUND TRUE)
    message(STATUS "OpenGL found")
else()
    message(WARNING "OpenGL not found - using fallback")
endif()

# =============================================================================
# Source Files
# =============================================================================

# Core emulator sources
set(CORE_SOURCES
    src/core/emulator.cpp
    src/core/emulator.h
    src/core/qemu_wrapper.cpp
    src/core/qemu_wrapper.h
    src/core/hypervisor.cpp
    src/core/hypervisor.h
    src/core/android_system.cpp
    src/core/android_system.h
)

# AI Core sources
set(AI_SOURCES
    src/ai/ai_core.cpp
    src/ai/ai_core.h
    src/ai/frame_generator.cpp
    src/ai/frame_generator.h
    src/ai/texture_enhancer.cpp
    src/ai/texture_enhancer.h
    src/ai/cnn_upscaler.cpp
    src/ai/cnn_upscaler.h
    src/ai/inference_engine.cpp
    src/ai/inference_engine.h
)

# UI sources
set(UI_SOURCES
    src/ui/overlay_window.cpp
    src/ui/overlay_window.h
    src/ui/glassmorphism_renderer.cpp
    src/ui/glassmorphism_renderer.h
    src/ui/metrics_display.cpp
    src/ui/metrics_display.h
    src/ui/gesture_handler.cpp
    src/ui/gesture_handler.h
    src/ui/voice_controller.cpp
    src/ui/voice_controller.h
)

# Memory management sources
set(MEMORY_SOURCES
    src/memory/memory_manager.cpp
    src/memory/memory_manager.h
    src/memory/intelligent_paging.cpp
    src/memory/intelligent_paging.h
    src/memory/process_analyzer.cpp
    src/memory/process_analyzer.h
)

# Shader cache sources
set(SHADER_SOURCES
    src/shader/shader_cache.cpp
    src/shader/shader_cache.h
    src/shader/aot_compiler.cpp
    src/shader/aot_compiler.h
    src/shader/jit_compiler.cpp
    src/shader/jit_compiler.h
    src/shader/predictive_cache.cpp
    src/shader/predictive_cache.h
)

# Input sources
set(INPUT_SOURCES
    src/input/input_handler.cpp
    src/input/input_handler.h
)

# Main application
set(MAIN_SOURCES
    src/main.cpp
    src/application.cpp
    src/application.h
)

# All sources combined
set(ALL_SOURCES
    ${MAIN_SOURCES}
    ${CORE_SOURCES}
    ${AI_SOURCES}
    ${UI_SOURCES}
    ${MEMORY_SOURCES}
    ${SHADER_SOURCES}
    ${INPUT_SOURCES}
)

# =============================================================================
# Executable
# =============================================================================

add_executable(NexusEmulator
    ${ALL_SOURCES}
)

# Include directories
target_include_directories(NexusEmulator PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Use Qt stubs if Qt not found
if(NOT QT_FOUND)
    target_compile_definitions(NexusEmulator PRIVATE USE_QT_STUBS)
endif()

# Link libraries (conditional)
if(QT_FOUND)
    target_link_libraries(NexusEmulator PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Quick
    )
    target_compile_definitions(NexusEmulator PRIVATE USE_QT)
endif()

if(VULKAN_FOUND)
    target_link_libraries(NexusEmulator PRIVATE Vulkan::Vulkan)
    target_compile_definitions(NexusEmulator PRIVATE USE_VULKAN)
endif()

if(OPENGL_FOUND)
    target_link_libraries(NexusEmulator PRIVATE OpenGL::GL)
    target_compile_definitions(NexusEmulator PRIVATE USE_OPENGL)
endif()

# Platform-specific libraries
if(WIN32)
    target_link_libraries(NexusEmulator PRIVATE
        d3d12
        dxgi
        dwmapi
        ole32
    )
endif()

# Linux libraries
if(UNIX AND NOT APPLE)
    target_link_libraries(NexusEmulator PRIVATE
        pthread
        dl
    )
endif()

# =============================================================================
# Compile Definitions
# =============================================================================

target_compile_definitions(NexusEmulator PRIVATE
    $<$<CONFIG:Debug>:DEBUG_BUILD>
    $<$<CONFIG:Release>:RELEASE_BUILD>
)

# CUDA support
if(ENABLE_CUDA)
    find_package(CUDAToolkit QUIET)
    if(CUDAToolkit_FOUND)
        add_compile_definitions(CUDA_ENABLED)
        target_link_libraries(NexusEmulator PRIVATE
            CUDA::cudart
            CUDA::cublas
        )
        message(STATUS "CUDA enabled")
    endif()
endif()

# =============================================================================
# Compiler Options
# =============================================================================

if(MSVC)
    target_compile_options(NexusEmulator PRIVATE
        /W4
        /WX-
        /permissive-
        /Zc:__cplusplus
        /utf-8
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/GL>
    )
    target_link_options(NexusEmulator PRIVATE
        $<$<CONFIG:Release>:/LTCG>
    )
else()
    target_compile_options(NexusEmulator PRIVATE
        -Wall
        -Wextra
        -O2
        -Wno-unused-parameter
        -Wno-unused-variable
    )
endif()

# =============================================================================
# Post-build events
# =============================================================================

# Copy config file to build directory
add_custom_command(TARGET NexusEmulator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/configs/nexus-config.yaml
    $<TARGET_FILE_DIR:NexusEmulator>/configs/nexus-config.yaml
    COMMENT "Copying configuration file..."
)

# Create output directories
add_custom_command(TARGET NexusEmulator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    $<TARGET_FILE_DIR:NexusEmulator>/models
    $<TARGET_FILE_DIR:NexusEmulator>/cache
    $<TARGET_FILE_DIR:NexusEmulator>/data
    $<TARGET_FILE_DIR:NexusEmulator>/logs
    COMMENT "Creating output directories..."
)

# =============================================================================
# Tests (only if Qt is available)
# =============================================================================

if(BUILD_TESTS AND QT_FOUND)
    enable_testing()

    add_executable(NexusTests
        tests/test_ai_core.cpp
        tests/test_memory_manager.cpp
        tests/test_shader_cache.cpp
        tests/test_input_handler.cpp
    )

    target_link_libraries(NexusTests PRIVATE
        Qt6::Core
        Qt6::Test
    )

    add_test(NAME NexusTests COMMAND NexusTests)
endif()

# =============================================================================
# Installation
# =============================================================================

install(TARGETS NexusEmulator
    RUNTIME DESTINATION bin
)

install(DIRECTORY configs/
    DESTINATION bin/configs
)

# =============================================================================
# Summary
# =============================================================================

message(STATUS "")
message(STATUS "=== Nexus AI Emulator Configuration ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Qt6: ${QT_FOUND}")
message(STATUS "Vulkan: ${VULKAN_FOUND}")
message(STATUS "OpenGL: ${OPENGL_FOUND}")
message(STATUS "CUDA: ${ENABLE_CUDA}")
message(STATUS "Build Tests: ${BUILD_TESTS}")
message(STATUS "======================================")
message(STATUS "")
