#!/bin/bash

usage()
{
    echo "usage: build [-t | --type [debug|release]] [-h | --help]"
    echo "example: build --type release"
    exit 
}

build_type=debug

while [ "$1" != "" ]; do
    case $1 in
        -t | --type)    shift 
                        build_type=$1 
                        ;;
        -h | --help)    usage 
                        ;;
    esac
    shift
done

if [ ! -d build ]; then
    mkdir build
fi

case "$build_type" in
    debug)      cd build && cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug && make -j $(nproc) ;;
    release)    cd build && cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release && make -j $(nproc) ;;
    *)          echo "invalid build type: $build_type"
                echo "must be one of: debug, release"
                ;;
esac
