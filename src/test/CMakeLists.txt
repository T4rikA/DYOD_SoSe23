set(
    SHARED_SOURCES
    base_test.cpp
    base_test.hpp
    gtest_case_template.cpp
    gtest_main.cpp
)

set(
    OPOSSUM_TEST_SOURCES
    ${SHARED_SOURCES}
    lib/all_type_variant_test.cpp
    storage/chunk_test.cpp
    storage/dictionary_segment_test.cpp
    storage/storage_manager_test.cpp
    storage/table_test.cpp
    storage/value_segment_test.cpp
    storage/fixed_width_integer_vector_test.cpp
)

# Both opossumTest and opossumSanitizers link against these
set(
    LIBRARIES
    gtest
)

# Build special sanitizer version of googletest
include_directories(../../third_party/googletest/googletest/)

set(
    GTEST_SOURCES
    ../../third_party/googletest/googletest/src/gtest-all.cc
)
add_library(gtestSanitizers EXCLUDE_FROM_ALL STATIC ${GTEST_SOURCES})
set_target_properties(gtestSanitizers PROPERTIES SUFFIX "_sanitizers")
set_target_properties(gtestSanitizers PROPERTIES COMPILE_FLAGS "-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer")

set(
    LIBRARIES_SANITIZERS
    gtestSanitizers
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Configure opossumTest
add_executable(opossumTest ${OPOSSUM_TEST_SOURCES})
target_link_libraries(opossumTest opossum ${LIBRARIES})

# Configure opossumCoverageApp
add_executable(opossumCoverage EXCLUDE_FROM_ALL ${OPOSSUM_TEST_SOURCES})
target_link_libraries(opossumCoverage opossumCoverageLib ${LIBRARIES} --coverage)
set_target_properties(opossumCoverage PROPERTIES COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")

# Configure opossumSanitizersApp
add_executable(opossumSanitizers EXCLUDE_FROM_ALL ${OPOSSUM_TEST_SOURCES})
target_link_libraries(opossumSanitizers opossumSanitizersLib ${LIBRARIES_SANITIZERS} -fsanitize=address)
set_target_properties(opossumSanitizers PROPERTIES COMPILE_FLAGS "-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer")
