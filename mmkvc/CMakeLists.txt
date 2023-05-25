file(GLOB XXHASH_SRC ${XXHASH_DIR}/*.c)

file(GLOB_RECURSE MMKVC_SRC
  protocol/*.cc
  client/*.cc
  mmkv.cc
  ${THIRD_PARTY_DIR}/kvarint/kvarint.c
)

list(APPEND MMKV_SRC XXHASH_SRC)

SetLibName(MMKVC_LIB mmkvc)
GenLib(${MMKVC_LIB} ${MMKVC_SRC})

message(STATUS "XXHASH_DIR = ${XXHASH_DIR}")
target_include_directories(${MMKVC_LIB} PRIVATE ${XXHASH_DIR})
set(LIB_LIST ${MMKVC_LIB} kanon_net kanon_base)
