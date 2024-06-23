find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(c-ares CONFIG)
# find_package(Threads)

#
# Protobuf/Grpc source files
#
set(PROTO_FILES
    raft.proto
)

#
# Add Library target with protobuf sources
#
add_library(raft_proto ${PROTO_FILES})
target_link_libraries(raft_proto
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc
        gRPC::grpc++
)
target_include_directories(raft_proto PUBLIC
${PROTOBUF_INCLUDE_DIRS} 
${CMAKE_CURRENT_BINARY_DIR})

#
# Compile protobuf and grpc files in mointor proto target to cpp
#
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
message(STATUS "plugin:${grpc_cpp_plugin_location}")

protobuf_generate(TARGET raft_proto LANGUAGE cpp)
protobuf_generate(TARGET raft_proto LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")
