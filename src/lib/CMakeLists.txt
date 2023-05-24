# Sources and libraries shared among the different builds of the lib
set(
    SOURCES
    all_type_variant.hpp
    null_value.hpp
    resolve_type.hpp
    storage/abstract_attribute_vector.hpp
    storage/fixed_width_integer_vector.hpp
    storage/fixed_width_integer_vector.cpp
    storage/abstract_segment.hpp
    storage/chunk.cpp
    storage/chunk.hpp
    storage/dictionary_segment.cpp
    storage/dictionary_segment.hpp
    storage/storage_manager.cpp
    storage/storage_manager.hpp
    storage/table.cpp
    storage/table.hpp
    storage/value_segment.cpp
    storage/value_segment.hpp
    type_cast.hpp
    types.hpp
    utils/assert.hpp
    utils/load_table.cpp
    utils/load_table.hpp
    utils/string_utils.cpp
    utils/string_utils.hpp
)

set(
    LIBRARIES
    pthread
)

# Configure the regular opossum library used for tests/server/playground...
add_library(opossum STATIC ${SOURCES})
target_link_libraries(opossum ${LIBRARIES})

# Configure the lib used for sanitizers
add_library(opossumSanitizersLib EXCLUDE_FROM_ALL STATIC ${SOURCES})
target_link_libraries(
    opossumSanitizersLib
    ${LIBRARIES}
    -fsanitize=address,undefined -fno-sanitize-recover=all
)
set_target_properties(opossumSanitizersLib PROPERTIES COMPILE_FLAGS "-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer")

# Configure the lib used for coverage
add_library(opossumCoverageLib EXCLUDE_FROM_ALL STATIC ${SOURCES})
target_link_libraries(
    opossumCoverageLib
    ${LIBRARIES}
    --coverage
)
set_target_properties(opossumCoverageLib PROPERTIES COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
