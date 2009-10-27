find_package(FLEX)
flex_target(scanner scanner.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)

add_library(tokenizer tokenizer.cpp token.cpp ${FLEX_scanner_OUTPUTS})
add_library(symboltable symboltableentry.cpp symboltable.cpp)
add_library(parser instruction.cpp programstore.cpp errorqueue.cpp parser.cpp)

target_link_libraries(parser tokenizer)