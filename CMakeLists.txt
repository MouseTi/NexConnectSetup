cmake_minimum_required(VERSION 3.16)
project(NexConnectSetup VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

add_executable(NexConnectSetup WIN32
    src/main.cpp
    src/SplashScreen.cpp
    src/SplashScreen.h
    src/SetupManager.cpp
    src/SetupManager.h
)

target_link_libraries(NexConnectSetup PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)

# Windows specific: embed icon and version info
if(WIN32)
    target_sources(NexConnectSetup PRIVATE src/resources.rc)
endif()

set_target_properties(NexConnectSetup PROPERTIES
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME "NexConnectSetup"
)
