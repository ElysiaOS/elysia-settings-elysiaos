cmake_minimum_required(VERSION 3.16)
project(ElysiaSettings)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
# Ensure compatibility with older CPUs
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=x86-64 -mtune=generic")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
    main.cpp
    MainWindow.cpp
    AnimatedTile.cpp
    translations.cpp
    components/About.cpp
    components/Applications.cpp
    components/Battery.cpp
    components/Bluetooth.cpp
    components/Display.cpp
    components/Network.cpp
    components/Power.cpp
    components/Sound.cpp
    components/Storage.cpp
    components/Wallpaper.cpp
)

set(HEADERS
    MainWindow.h
    AnimatedTile.h
    translations.h
    components/About.h
    components/Applications.h
    components/Battery.h
    components/Bluetooth.h
    components/Display.h
    components/Network.h
    components/Power.h
    components/Sound.h
    components/Storage.h
    components/Wallpaper.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

# Set properties for Qt6
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
) 