cmake_minimum_required(VERSION 3.15)
project(NexConnectSetup)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows-specific settings for single exe
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
    add_compile_definitions(UNICODE _UNICODE)
    
    # Static linking runtime
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/Installer.cpp
    src/Downloader.cpp
    src/UI.cpp
    src/Utils.cpp
)

set(HEADERS
    src/Installer.h
    src/Downloader.h
    src/UI.h
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
