cmake_minimum_required(VERSION 3.5)

project(client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ../)

set(CMAKE_CXX_FLAGS "-pedantic-errors -Wall -Wextra -Werror")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fno-omit-frame-pointer -fsanitize=address")
 
find_package(Qt6 COMPONENTS Qml Quick Widgets Core Gui Network REQUIRED)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

set(PROJECT_SOURCES
        src/main.cpp
        src/client.cpp
        src/board.cpp
        src/list.cpp
        src/card.cpp
        src/boardmodel.cpp
        src/listmodel.cpp
        src/json_parser.cpp
        src/qml.qrc
        include/client.hpp
        include/board.hpp
        include/list.hpp
        include/card.hpp
        include/boardmodel.hpp
        include/listmodel.hpp
        include/json_parser.hpp
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
    nlohmann_json::nlohmann_json
)
