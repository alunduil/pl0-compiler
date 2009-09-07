include_directories(${COMPILER_SRC_DIR}/include)
link_directories(${COMPILER_SRC_DIR}/lib)

add_executable(compiler main.cpp)
target_link_libraries(compiler symboltable)

