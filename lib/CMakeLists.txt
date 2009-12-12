find_package(BISON)
find_package(FLEX)

# Make sure that we include the bison header file for the compiler rather than any
# other that may spuriously show up.
include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})

bison_target(parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp VERBOSE parser.output)

set(FLEX_scanner_FLAGS "-Cf -Cr")
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(FLEX_scanner_FLAGS "${FLEX_scanner_FLAGS} -d -s")
endif()

flex_target(scanner scanner.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp COMPILE_FLAGS ${FLEX_scanner_FLAGS})
add_flex_bison_dependency(scanner parser)

add_library(environment symboltable.cpp symboltableentry.cpp)
add_library(analyzer ${BISON_parser_OUTPUTS} ${FLEX_scanner_OUTPUTS})
add_library(generator instruction.cpp programstore.cpp)
add_library(compiler compiler.cpp)

add_dependencies(environment parser)

target_link_libraries(analyzer generator environment)
target_link_libraries(compiler analyzer)
