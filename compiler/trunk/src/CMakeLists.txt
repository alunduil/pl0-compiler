include_directories(${COMPILER_SRC_DIR}/include)
link_directories(${COMPILER_SRC_DIR}/lib)

add_executable(pl0 main.cpp compiler.cpp)
target_link_libraries(pl0 symboltable tokenizer boost_program_options parser)

