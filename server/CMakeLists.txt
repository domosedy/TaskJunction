cmake_minimum_required(VERSION 3.5)

project(TestProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Sql)
find_package(Qt6 REQUIRED COMPONENTS WebSockets)
find_package(nlohmann_json REQUIRED)

set(INSTALL_EXAMPLEDIR "./build")

qt_add_executable(server 
include/client_socket.hpp 
include/data_handler.hpp
# include/json.hpp
include/logging.hpp 
include/query.hpp
include/server.hpp 
../database/include/database.hpp
../shared/include/element_classes.hpp
../shared/include/hashes.hpp
../shared/include/jsonparser.hpp

../database/src/database.cpp
../shared/src/jsonparser.cpp
../shared/src/element_classes.cpp
../shared/src/hashes.cpp
src/client_socket.cpp
src/data_handler.cpp
src/server_main.cpp
src/server.cpp)

set(securesocketclient_resource_files
    "localhost.cert"
    "localhost.key"
)

qt6_add_resources(server "securesocketclient"
    PREFIX
        "/"
    FILES
        ${securesocketclient_resource_files}
)

install(TARGETS server
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)


target_include_directories(server PUBLIC include)
target_include_directories(server PUBLIC ../database/include)
target_include_directories(server PUBLIC ../shared/include)
target_link_libraries(server PRIVATE Qt6::Network)
target_link_libraries(server PRIVATE Qt6::WebSockets)
target_link_libraries(server PRIVATE Qt6::Sql)
target_link_libraries(server PRIVATE Qt6::Core)
target_link_libraries(server PRIVATE nlohmann_json::nlohmann_json)
