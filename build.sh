#!/bin/bash

build_client () {
    cd ./client
    mkdir -p build
    cd ./build
    cmake ..
    make
    cp client ../../client_app
    cd ../../
}

build_server () {
    cd ./server
    mkdir -p build
    cd ./build
    cmake ..
    make
    cp server ../../server_app
    cd ../../
}

is_client_builded=0
is_server_builded=0

if [[ $# -eq 0 ]]; then
    echo 'Provide build options please!'
    exit 0
fi

while [[ $# -gt 0 ]]; do
    case $1 in
        -c|--client)
            build_client
            is_client_builded=1
            shift
            ;;
        -s|--server)
            build_server
            is_server_builded=1
            shift
            ;;
        -a|--all)
            if [[ is_client_builded -eq 0 ]]; then
                build_client
                is_client_builded=1;
            fi
            if [[ is_server_builded -eq 0 ]]; then
                build_server
                is_server_builded=1
            fi
            shift
            ;;
        -*|--*)
            echo "Unknown option $1"
            exit 1
    esac
done