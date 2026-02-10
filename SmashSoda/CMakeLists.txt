cmake_minimum_required(VERSION 3.18)

if(NOT WIN32)
    message(FATAL_ERROR "Smash Soda can only be built on Windows.")
endif()

project(SmashSoda LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --------------------------
# C++ settings
# --------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --------------------------
# Unity Builds
# --------------------------
set(CMAKE_UNITY_BUILD ON)

# --------------------------
# Source and header files
# --------------------------
file(GLOB_RECURSE ALL_HEADERS "*.h" "*.hpp")
file(GLOB_RECURSE ALL_SOURCES "*.cpp" "*.cxx" "*.cc")

# Add resource file for app icon
set(RESOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Resource.rc")

# Combine all files
set(ALL_FILES ${ALL_HEADERS} ${ALL_SOURCES} ${RESOURCE_FILE})

# Visual Studio friendly grouping
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ALL_FILES})

# --------------------------
# Runtime library fix (ViGEmClient)
# --------------------------
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

# --------------------------
# Add executable
# --------------------------
add_executable("${PROJECT_NAME}" ${ALL_FILES})

# --------------------------
# Include directories
# --------------------------
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/chatbot"
    "${CMAKE_CURRENT_SOURCE_DIR}/services"
    "${CMAKE_CURRENT_SOURCE_DIR}/core"
    "${CMAKE_CURRENT_SOURCE_DIR}/fonts"
    "${CMAKE_CURRENT_SOURCE_DIR}/globals"
    "${CMAKE_CURRENT_SOURCE_DIR}/helpers"
    "${CMAKE_CURRENT_SOURCE_DIR}/icons"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/lists"
    "${CMAKE_CURRENT_SOURCE_DIR}/models"
    "${CMAKE_CURRENT_SOURCE_DIR}/sfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/structs"
    "${CMAKE_CURRENT_SOURCE_DIR}/widgets"
    "${CMAKE_CURRENT_SOURCE_DIR}/../external"
    "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/parsecsdk"
    "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/matoya"
    "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/vigem/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/sdl/include"
)

# --------------------------
# Precompiled headers
# --------------------------
target_precompile_headers(${PROJECT_NAME} PRIVATE pch.h)

# --------------------------
# Compile definitions
# --------------------------
target_compile_definitions(${PROJECT_NAME} PRIVATE
    _CRT_SECURE_NO_WARNINGS
    _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    UNICODE
    _UNICODE
    _CONSOLE
)

# --------------------------
# Compiler options for MSVC
# --------------------------
if(MSVC)
    # Suppress warnings, enable secure & large object support
    target_compile_options(${PROJECT_NAME} PRIVATE /W0 /permissive- /sdl /bigobj /wd4996)
    
    target_link_options(${PROJECT_NAME} PRIVATE
        /SUBSYSTEM:WINDOWS
        /DEBUG:FULL
        /NODEFAULTLIB:LIBCMT
        $<$<CONFIG:Release>:/OPT:REF;/OPT:ICF>
    )
endif()

# --------------------------
# Link libraries (x86/x64 aware)
# --------------------------
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(PARSECLIB parsec)
    set(SDL2LIB_DIR "${CMAKE_SOURCE_DIR}/Dependencies/sdl/lib/x64")
    set(VIGEM_DIR "${CMAKE_SOURCE_DIR}/Dependencies/vigem/lib/release/x64")
    set(MATOYA_DIR "${CMAKE_SOURCE_DIR}/Dependencies/matoya/64")
else()
    set(PARSECLIB parsec32)
    set(SDL2LIB_DIR "${CMAKE_SOURCE_DIR}/Dependencies/sdl/lib/x86")
    set(VIGEM_DIR "${CMAKE_SOURCE_DIR}/Dependencies/vigem/bin/release32")
    set(MATOYA_DIR "${CMAKE_SOURCE_DIR}/Dependencies/matoya/32")
endif()

target_link_directories(${PROJECT_NAME} PRIVATE
    ${VIGEM_DIR}
    "${CMAKE_SOURCE_DIR}/Dependencies/parsecsdk/windows"
    ${MATOYA_DIR}
    ${SDL2LIB_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${PARSECLIB}
    matoya
    ViGEmClient
    SDL2
    SDL2main
    Winhttp
    Crypt32
    Secur32
    kernel32
    gdi32
    winmm
    imm32
    shell32
    advapi32
    ole32
    oleaut32
    opengl32
    user32
    uuid
    version
    setupapi
    hid
    dxgi
    ws2_32
    bcrypt
    windowscodecs
    xinput9_1_0
    d3d9
    shlwapi
    d3d11
)

# --------------------------
# Post-build: copy assets
# --------------------------
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    # Fonts, icons, sfx
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/SmashSoda/fonts" $<TARGET_FILE_DIR:${PROJECT_NAME}>/fonts
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/SmashSoda/icons" $<TARGET_FILE_DIR:${PROJECT_NAME}>/icons
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/SmashSoda/sfx" $<TARGET_FILE_DIR:${PROJECT_NAME}>/sfx
    # DLLs
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/Dependencies/parsecsdk/windows/parsec.dll" $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy
        "${SDL2LIB_DIR}/SDL2.dll" $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
