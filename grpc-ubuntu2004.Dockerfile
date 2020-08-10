# docker build -f grpc-ubuntu2004.Dockerfile --no-cache -t drepin/grpc-cpp:grpc-ubuntu2004 .
FROM ubuntu:20.04
LABEL MAINTAINER="Dmitriy Repin <drepin@hotmail.com>"

# This container builds the $GRPC_VER version of the gRPC library (https://github.com/grpc/grpc)
# To find out the latest available version one can do `curl -L http://grpc.io/release`
ARG GRPC_VER=v1.30.2
ENV GRPC_VERSION=$GRPC_VER
RUN echo "Building Docker image of the gRPC version '$GRPC_VER'"

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    # Install the minimal set of tools required to build gRPC
    git curl wget jq make unzip cmake gdb \
    # Install the tools advised for gRPC C++ build on Linux
    # https://github.com/grpc/grpc/blob/v1.18.0/BUILDING.md
    build-essential autoconf libtool pkg-config

#-----------------------------------------------------------------------------------------------
# 1) Fetch and build the gRPC library from the source (https://github.com/grpc/grpc)
#-----------------------------------------------------------------------------------------------
RUN git clone -b $GRPC_VER https://github.com/grpc/grpc /opt/grpc/grpc

WORKDIR /opt/grpc/grpc

RUN git submodule update --init && \
    mkdir -p /opt/grpc/grpc/cmake/build && cd /opt/grpc/grpc/cmake/build && \
    cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DgRPC_INSTALL=ON \
    -DgRPC_BUILD_TESTS=OFF \
    -DgRPC_SSL_PROVIDER=module \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    ../.. && \
    make -j4 install

# NOTES: 
# * The resulting artifacts are installed in:
#     /usr/local/bin 
#     /usr/local/include 
#     /usr/local/lib
#     /usr/local/share
# * Consumers will need to set
#     LD_LIBRARY_PATH="/usr/local/lib"
#     CMAKE_INSTALL_PREFIX=/usr/local

#-----------------------------------------------------------------------------------------------
# 2) Fetch and build the latest version of {fmt} an open-source formatting library for C++. 
# (https://github.com/fmtlib/fmt)
#-----------------------------------------------------------------------------------------------
WORKDIR /tmp
RUN FMT_VER=$(curl -s https://api.github.com/repos/fmtlib/fmt/releases/latest | jq -r '.name') && \
    echo "Fetching fmtlib/fmt ${FMT_VER}" && \
    wget -nv -O /tmp/fmt-${FMT_VER}.tgz https://github.com/fmtlib/fmt/archive/${FMT_VER}.tar.gz && \
    cd /tmp && tar xf /tmp/fmt-${FMT_VER}.tgz && cd /tmp/fmt-${FMT_VER} && \
    mkdir -p /tmp/fmt-${FMT_VER}/build && cd /tmp/fmt-${FMT_VER}/build && \
    cmake .. && make && make install && \
    rm -rf /tmp/fmt-${FMT_VER}*
# NOTES: 
# * The resulting artifacts are installed in:
#     /usr/local/lib64/libfmt.a 
#     /usr/local/include/fmt/ 
#     /usr/local/lib64/cmake/fmt/ 
#     /usr/local/lib64/pkgconfig/fmt.pc
# * Consumers will need to set
#     PKG_CONFIG_PATH="/usr/local/lib64/pkgconfig"

