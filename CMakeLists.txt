cmake_minimum_required(VERSION 3.16)

project(hellyeah LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Multimedia Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(hellyeah
    src/main.cpp
    src/api/HifiClient.cpp
    src/api/HifiClient.hpp
    src/player/AudioPlayer.cpp
    src/player/AudioPlayer.hpp
    src/ui/MainWindow.cpp
    src/ui/MainWindow.hpp
)

target_link_libraries(hellyeah PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Multimedia
    Qt6::Widgets
)
