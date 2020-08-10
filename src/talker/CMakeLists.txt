cmake_minimum_required(VERSION 3.5.1)

project(grpc_cpp_async_server CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

find_package(Threads REQUIRED)

# We assume that gRPC and all its dependencies are already installed on this system
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP gRPC::grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

# Use installed fmtlib/fmt
find_package(fmt)
set(_FMT_LIB fmt::fmt)

# Source proto files location
set(proto_dir "${CMAKE_SOURCE_DIR}/protos")
message(STATUS "Using gRPC proto dir: ${proto_dir}")

# Generated proto file location
get_filename_component(GENERATED_PROTO_DIR "${CMAKE_BINARY_DIR}/../generated_proto" ABSOLUTE)
message(STATUS "Using gRPC generated C++ source dir: ${GENERATED_PROTO_DIR}")
file(MAKE_DIRECTORY ${GENERATED_PROTO_DIR})

# Generated proto file names
set(all_grpc_srcs)

# Generate the proto sources
foreach(_proto_name "pinger" "talker")
  get_filename_component(proto_file "${proto_dir}/${_proto_name}.proto" ABSOLUTE)
  set(proto_base "${GENERATED_PROTO_DIR}/${_proto_name}")
  set(proto_srcs "${proto_base}.pb.cc" "${proto_base}.grpc.pb.cc")
  set(proto_hdrs "${proto_base}.pb.h" "${proto_base}.grpc.pb.h")

  list(APPEND all_grpc_srcs ${proto_srcs})

  add_custom_command(
        OUTPUT ${proto_srcs} ${proto_hdrs}
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${GENERATED_PROTO_DIR}" --cpp_out "${GENERATED_PROTO_DIR}"
          --proto_path "${proto_dir}"
          --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
          ${proto_file}
        DEPENDS ${proto_file})
endforeach()

# Include generated *.pb.h files
include_directories("${GENERATED_PROTO_DIR}")
include_directories("${CMAKE_SOURCE_DIR}")

file(GLOB all_but_main_srcs "*.cpp")
list(FILTER all_but_main_srcs EXCLUDE REGEX "${PROJECT_SOURCE_DIR}/main_*."  )


foreach(_target talker_async_server talker_client)

  add_executable("${_target}.exe" "main_${_target}.cpp" ${all_but_main_srcs} ${all_grpc_srcs})
  target_link_libraries("${_target}.exe" ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF} ${_FMT_LIB})

  install(TARGETS "${_target}.exe" CONFIGURATIONS Debug DESTINATION bin/Debug)
  install(TARGETS "${_target}.exe" CONFIGURATIONS Release DESTINATION bin/Release)

endforeach()
