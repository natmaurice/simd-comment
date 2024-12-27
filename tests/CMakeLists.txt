include_directories(
  ${PROJECT_SOURCE_DIR}/tests
)
include_directories(
  ${PROJECT_SOURCE_DIR}/src
)

set(tests_SOURCES
  ${PROJECT_SOURCE_DIR}/tests/main.cpp
)

if (NOT TARGET Catch2)
  add_subdirectory(${PROJECT_SOURCE_DIR}/tests/lib/catch2)
endif()

add_executable(simdcomment_tests ${tests_SOURCES})
target_link_libraries(simdcomment_tests PRIVATE simdcomment-slib Catch2::Catch2WithMain)

set_target_properties(simdcomment_tests PROPERTIES OUTPUT_NAME tests)

