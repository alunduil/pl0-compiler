add_library(tokenizer tokenizer.cpp token.cpp)
add_library(symboltable symboltableentry.cpp symboltable.cpp)
add_library(parser parser.cpp)
target_link_libraries(parser tokenizer)