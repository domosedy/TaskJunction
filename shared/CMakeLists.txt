cmake_minimum_required(VERSION 3.5)
project(TestProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ../)
find_package(Qt6 REQUIRED COMPONENTS Core)

add_executable(json-test
include/element_classes.hpp

src/element_classes.cpp
src/test_json.cpp)

target_include_directories(json-test PRIVATE include)
target_link_libraries(json-test PRIVATE Qt6::Core)

add_executable(hash-test
include/hashes.hpp

src/hashes.cpp
src/test_hashes.cpp)

target_include_directories(hash-test PRIVATE include)
target_link_libraries(hash-test PRIVATE Qt6::Core)
