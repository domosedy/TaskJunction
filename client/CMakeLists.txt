cmake_minimum_required(VERSION 3.5)

project(client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-pedantic-errors -g -Wall -Wextra -Werror -Wshadow")
 
find_package(Qt6 COMPONENTS Qml Quick Widgets Core Gui Network Sql REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(nlohmann_json REQUIRED)
    
set(PROJECT_SOURCES
        src/main.cpp
        src/client.cpp
        src/boardmodel.cpp
        src/listmodel.cpp
        src/boardmenu.cpp
        src/jsonparser.cpp
        src/qml.qrc
        include/client.hpp
        include/boardmodel.hpp
        include/listmodel.hpp
        include/boardmenu.hpp
        include/jsonparser.hpp
        ../shared/include/element_classes.hpp
        ../shared/include/hashes.hpp
        ../shared/src/element_classes.cpp
        ../shared/src/hashes.cpp
        ../database/include/database.hpp
        ../database/src/database.cpp
)

add_executable(client
    ${PROJECT_SOURCES}
)

target_include_directories(client PRIVATE include shared ../shared/include ../database/include)
target_link_libraries(client PRIVATE 
    Qt6::Widgets 
    Qt6::Quick
    Qt6::Core
    Qt6::Gui
    Qt6::Network 
    Qt6::Sql
    nlohmann_json::nlohmann_json
    pq
)