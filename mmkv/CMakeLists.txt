file(GLOB_RECURSE MMKVC_SRC
  internal/*.cc
  *.cc
  ${XXHASH_DIR}/*.c
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
  GenLib(mmkvc ${MMKVC_SRC})
  set(MMKVC_LIB mmkvc)
else ()
  GenLib(mmkvc_debug ${MMKVC_SRC})
  set(MMKVC_LIB mmkvc_debug)
endif()

message(STATUS "XXHASH_DIR = ${XXHASH_DIR}")
target_include_directories(${MMKVC_LIB} PRIVATE ${XXHASH_DIR})
set(LIB_LIST ${MMKVC_LIB} kanon_net kanon_base)
