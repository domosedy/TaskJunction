cmake_minimum_required(VERSION 3.5)

project(database VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Sql)
find_package(PostgreSQL REQUIRED)

add_executable(database database.cpp)

target_include_directories(database PUBLIC include)
target_link_libraries(database PRIVATE Qt6::Network)
target_link_libraries(database PRIVATE Qt6::Core)
target_link_libraries(database PRIVATE Qt6::Sql)
target_link_libraries(database PRIVATE pq)
