cmake_minimum_required(VERSION 3.5)

project(client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ../)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")

find_package(Qt6 COMPONENTS Qml Quick Widgets Core Gui Network REQUIRED)

set(PROJECT_SOURCES
        src/main.cpp
        src/client.cpp
        src/board.cpp
        src/list.cpp
        src/card.cpp
        src/boardmodel.cpp
        src/qml.qrc
        include/client.hpp
        include/board.hpp
        include/list.hpp
        include/card.hpp
        include/boardmodel.hpp
)

add_executable(client
    ${PROJECT_SOURCES}
)


target_include_directories(client PRIVATE include)
target_link_libraries(client PRIVATE 
    Qt6::Widgets 
    Qt6::Quick
    Qt6::Core
    Qt6::Gui
    Qt6::Network 
)
