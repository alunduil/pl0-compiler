find_package(FLEX)
flex_target(scanner scanner.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)

add_library(environment errorqueue.cpp token.cpp symboltable.cpp symboltableentry.cpp)
add_library(analyzer parser.cpp tokenizer.cpp ${FLEX_scanner_OUTPUTS})
add_library(generator instruction.cpp programstore.cpp)
add_library(compiler compiler.cpp)

target_link_libraries(analyzer environment)
target_link_libraries(compiler analyzer generator)
