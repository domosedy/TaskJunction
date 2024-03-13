cmake_minimum_required(VERSION 3.5)

project(TestProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ../)
find_package(Qt6 REQUIRED COMPONENTS Core Network)

set(FILES include/logging.hpp src/main.cpp)
add_executable(TestProject ${FILES})
target_include_directories(TestProject PRIVATE include)
target_link_libraries(TestProject PRIVATE Qt6::Core)

add_executable(server 
include/client_socket.hpp 
include/data_handler.hpp
include/json.hpp
include/logging.hpp 
include/query.hpp
include/server.hpp 
include/datas.hpp

src/client_socket.cpp
src/data_handler.cpp
src/datas.cpp
src/server_main.cpp
src/server.cpp)


target_include_directories(server PUBLIC include)
target_link_libraries(server PRIVATE Qt6::Network)
target_link_libraries(server PRIVATE Qt6::Core)

add_executable(json-test
    include/datas.hpp

    src/datas.cpp
    src/test_json.cpp)

target_include_directories(json-test PRIVATE include)