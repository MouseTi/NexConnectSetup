cmake_minimum_required(VERSION 3.15)
project(NexConnectSetup)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows-specific settings for single exe
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup")
    add_compile_definitions(UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
    
    # Static linking runtime - no DLL dependencies
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    
    # Additional linker flags for static linking
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /INCREMENTAL:NO /OPT:REF /OPT:ICF")
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/Installer.cpp
    src/Downloader.cpp
    src/SplashWindow.cpp
    src/ManifestParser.cpp
    src/Utils.cpp
)

set(HEADERS
    src/Installer.h
    src/Downloader.h
    src/SplashWindow.h
    src/ManifestParser.h
    src/Utils.h
    src/resource.h
)

# Resource file
set(RESOURCES
    src/resources.rc
)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${RESOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    wininet
    urlmon
    shell32
    ole32
    comctl32
    gdi32
    user32
)

# Set output name
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "NexConnect-Setup"
)
