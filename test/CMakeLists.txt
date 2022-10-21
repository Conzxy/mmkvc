#enable_testing()
file(GLOB MMKVC_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*/*test*.cc")
file(GLOB MMKVC_BENCH_SOURCES "${PROJECT_SOURCE_DIR}/test/*/*bench*.cc")

#list(APPEND MMKVC_TEST_BENCH_SOURCES ${MMKVC_TEST_SOURCES} ${MMKVC_BENCH_SOURCES})

############################################
# User can determine whether to build all tests when build target all
# e.g. cmake --build */mmkvc/build [--target all -j 2]
############################################
set(BUILD_ALL_TESTS OFF CACHE BOOL "Don't exclude tests from target all")

message(STATUS "mmkvc test source files: ${MMKVC_TEST_SOURCES}")
message(STATUS "mmkvc bench source files: ${MMKVC_BENCH_SOURCES}")

if (NOT ${BUILD_ALL_TESTS})
  message(STATUS "[The mmkvc test and bench files are excluded from the build of target all]")
endif (NOT ${BUILD_ALL_TESTS})

############################################
# generate test and bench program
############################################
macro (GenTest SRC)
  foreach (mmkvc_test_source ${${SRC}})
    get_filename_component(mmkvc_test_filename ${mmkvc_test_source} NAME_WE)

    if (${BUILD_ALL_TESTS})
      add_executable(${mmkvc_test_filename} ${mmkvc_test_source})
    else ()
      message(STATUS "${mmkvc_test_filename} is excluded from target all")
      add_executable(${mmkvc_test_filename} EXCLUDE_FROM_ALL ${mmkvc_test_source})
    endif (${BUILD_ALL_TESTS})

    target_link_libraries(${mmkvc_test_filename} ${ARGN})
    set_target_properties(${mmkvc_test_filename}
      PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
      COMMAND ${mmkvc_test_filename}
    )

    add_test(NAME ${mmkvc_test_filename} COMMAND ${mmkvc_test_filename})
  endforeach (mmkvc_test_source ${MMKVC_TEST_SOURCES})
endmacro()

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
  set(MMKVC_LIB mmkvc)
else ()
  set(MMKVC_LIB mmkvc_debug)
endif()

GenTest(MMKVC_TEST_SOURCES gtest gtest_main ${MMKVC_LIB} kanon_net kanon_base)
GenTest(MMKVC_BENCH_SOURCES benchmark benchmark_main ${MMKVC_LIB} kanon_net kanon_base)
