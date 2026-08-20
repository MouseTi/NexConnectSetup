cmake_minimum_required(VERSION 3.16)
project(NexConnectSetup VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Static linking for single exe
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(BUILD_SHARED_LIBS OFF)

# Find Qt6 with static components
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

# Set Qt to use static plugins
set(QT_USE_STATIC_PLUGINS ON)

add_executable(NexConnect-Setup WIN32
    src/main.cpp
    src/SplashScreen.cpp
    src/SplashScreen.h
    src/SetupWindow.cpp
    src/SetupWindow.h
    src/DownloadManager.cpp
    src/DownloadManager.h
    src/SetupManager.cpp
    src/SetupManager.h
    src/resources.rc
)

target_link_libraries(NexConnect-Setup PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Shell32
    Ole32
    Advapi32
)

# Static linking flags for MSVC
if(MSVC)
    set_target_properties(NexConnect-Setup PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )
    target_compile_options(NexConnect-Setup PRIVATE /MT$<$<CONFIG:Debug>:d>)
endif()

# Install
install(TARGETS NexConnect-Setup DESTINATION .)
