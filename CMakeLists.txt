cmake_minimum_required(VERSION 3.18)


project(simdcomment
  VERSION 1.0
  LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dirs
set(src_dir src)
set(inc_dir src)
set(lib_dir lib)


# Set library name
set(libname simdcomment)

# ========================================
# SIMD Comment Options
# ========================================

option(SIMDCOMMENT_ENABLE_TESTS "Enable tests" OFF)
option(SIMDCOMMENT_ENABLE_AVX512 "Enable AVX512" OFF)

# Log options
message(STATUS "SIMD-comment Parse Options")
message(STATUS "  * SIMDCOMMENT_ENABLE_TESTS ${SIMDCOMMENT_ENABLE_TESTS}")
message(STATUS "  * SIMDCOMMENT_ENABLE_AVX512 ${SIMDCOMMENT_ENABLE_AVX512}")

# Source files
list(APPEND SIMDCOMMENT_SOURCES
  ${src_dir}/simdcomment/scalar.cpp
)

if (SIMDCOMMENT_ENABLE_AVX512)
  list(APPEND SIMDCOMMENT_SOURCES
    ${src_dir}/simdcomment/avx512.cpp
  )
endif()


# Compile library
add_library(simdcomment-obj OBJECT ${SIMDCOMMENT_SOURCES})
set_target_properties(simdcomment-obj PROPERTIES POSITION_INDEPENDENT_CODE ON) # set -fpic


add_library(simdcomment-slib STATIC $<TARGET_OBJECTS:simdcomment-obj>)
set_target_properties(simdcomment-slib PROPERTIES OUTPUT_NAME simdcomment POSITION_INDEPENDENT_CODE ON) # set -fp

target_include_directories(simdcomment-obj  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${inc_dir}/>)
target_include_directories(simdcomment-slib PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${inc_dir}/>)

export(TARGETS simdcomment-slib NAMESPACE simdcomment:: FILE "${lib_dir}/cmake/simdcomment/simdcomment-config.cmake")


if (SIMDCOMMENT_ENABLE_AVX512)
    target_compile_definitions(${libname} PUBLIC SIMDCOMMENT_ENABLE_AVX512)
endif()

if (SIMDCOMMENT_ENABLE_TESTS)
  add_subdirectory(tests)
endif()
