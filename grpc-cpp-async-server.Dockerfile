# docker build -f grpc-cpp-async-server.Dockerfile --no-cache -t drepin/grpc-cpp:grpc-cpp-async-server .
ARG PARENT_VERSION="grpc-ubuntu2004"
FROM drepin/grpc-cpp:${PARENT_VERSION}
LABEL MAINTAINER="Dmitriy Repin <drepin@hotmail.com>"
ARG PARENT_VERSION
RUN echo "Using the base image: drepin/grpc-cpp:${PARENT_VERSION}"

# Copy source
COPY src/          /opt/grpc-cpp-async/src
COPY automation/   /opt/grpc-cpp-async/automation

# Build and deploy 
RUN /opt/grpc-cpp-async/automation/build.sh clean && \
    /opt/grpc-cpp-async/automation/build.sh Release

EXPOSE 50051
RUN useradd --uid 1000 --user-group -d /home/drepin --create-home drepin
# USER 1000

WORKDIR /opt/grpc-cpp-async/src/

