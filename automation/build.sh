#!/usr/bin/env bash

set -e  # Exit immediately if a pipeline returns a non-zero status

if [ -n "${1}" ]; then
    if [ "${1}" = "Debug" ] || [ "${1}" = "Release" ]; then
        BUILD_TYPE=${1}
    elif [ "${1}" = "Clean" ] || [ "${1}" = "clean" ]; then
        echo "Removing /opt/grpc-cpp-async/bin and /opt/grpc-cpp-async/cmake/"
        rm -rf /opt/grpc-cpp-async/bin
        rm -rf /opt/grpc-cpp-async/cmake
        rm -rf /opt/grpc-cpp-async/build
        rm -rf code.*
        exit 0
    else
        echo "Unexpected argument: '${1}'"
        exit -1
    fi
fi

echo "BUILD_TYPE: ${BUILD_TYPE}"

mkdir -p /opt/grpc-cpp-async/cmake/${BUILD_TYPE}
cd /opt/grpc-cpp-async/cmake/${BUILD_TYPE} 
cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
      -DCMAKE_INSTALL_PREFIX=/opt/grpc-cpp-async \
      /opt/grpc-cpp-async/src
make -j4 install
