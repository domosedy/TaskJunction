cmake_minimum_required(VERSION 3.5)

project(TestProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Sql)
find_package(Qt6 REQUIRED COMPONENTS WebSockets)

add_executable(server 
include/client_socket.hpp 
include/data_handler.hpp
include/json.hpp
include/logging.hpp 
include/query.hpp
include/server.hpp 
# include/datas.hpp
../database/include/database.hpp
../shared/include/element_classes.hpp

../database/src/database.cpp
../shared/src/element_classes.cpp
src/client_socket.cpp
src/data_handler.cpp
# src/datas.cpp
src/server_main.cpp
src/server.cpp)


target_include_directories(server PUBLIC include)
target_include_directories(server PUBLIC ../database/include)
target_include_directories(server PUBLIC ../shared/include)
target_link_libraries(server PRIVATE Qt6::Network)
target_link_libraries(server PRIVATE Qt6::WebSockets)
target_link_libraries(server PRIVATE Qt6::Sql)
target_link_libraries(server PRIVATE Qt6::Core)
