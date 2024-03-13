cmake_minimum_required(VERSION 3.5)

project(TestProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core)

set(FILES src/main.cpp)
add_executable(TestProject ${FILES})
target_include_directories(TestProject PRIVATE include)
target_link_libraries(TestProject PRIVATE Qt6::Core)
