mmkvc_gen_proto_code(PROTOC_PATH "protoc" 
  FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/client/configuration.proto"
  "${CMAKE_CURRENT_SOURCE_DIR}/client/configd.proto"
)

set(PB_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "mmkv pb output dir = ${PB_OUTPUT_DIR}")

set(
  PB_SRC
  ${PB_OUTPUT_DIR}/configuration.pb.cc
  ${PB_OUTPUT_DIR}/configd.pb.cc
)

file(GLOB XXHASH_SRC ${XXHASH_DIR}/*.c)

file(GLOB_RECURSE MMKVC_SRC
  protocol/*.cc
  client/*.cc
  mmkv.cc
  ${THIRD_PARTY_DIR}/kvarint/kvarint.c
)

list(APPEND MMKVC_SRC ${XXHASH_SRC})
list(APPEND MMKVC_SRC ${PB_SRC})

SetLibName(MMKVC_LIB mmkvc)
GenLib(${MMKVC_LIB} ${MMKVC_SRC})

message(STATUS "XXHASH_DIR = ${XXHASH_DIR}")
target_include_directories(${MMKVC_LIB} 
  PRIVATE ${XXHASH_DIR}
  PRIVATE "${PB_OUTPUT_DIR}")
set(LIB_LIST ${MMKVC_LIB} kanon_protobuf kanon_net kanon_base)
