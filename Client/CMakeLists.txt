cmake_minimum_required(VERSION 3.5)

project(client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-pedantic-errors -Wall -Wextra -Werror -Wshadow")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fno-omit-frame-pointer -fsanitize=address")
 
find_package(Qt6 COMPONENTS Qml Quick Widgets Core Gui Network Sql REQUIRED)


include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)
    
set(PROJECT_SOURCES
        src/main.cpp
        src/client.cpp
        src/boardmodel.cpp
        src/listmodel.cpp
        src/json_parser.cpp
        src/boardmenu.cpp
        src/qml.qrc
        include/client.hpp
        include/boardmodel.hpp
        include/listmodel.hpp
        include/boardmenu.hpp
        shared/logging.hpp
        shared/json_parser.hpp
        #shared/base_classes.hpp
        #shared/base_classes.cpp
        ../shared/include/element_classes.hpp
        ../shared/include/hashes.hpp
        ../shared/src/element_classes.cpp
        ../shared/src/hashes.cpp
)

add_executable(client
    ${PROJECT_SOURCES}
)

target_include_directories(client PRIVATE include shared ../shared/include)
target_link_libraries(client PRIVATE 
    Qt6::Widgets 
    Qt6::Quick
    Qt6::Core
    Qt6::Gui
    Qt6::Network 
    Qt6::Sql
    nlohmann_json::nlohmann_json
)
