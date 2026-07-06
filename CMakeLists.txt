cmake_minimum_required(VERSION 3.10)
project(libmatrix)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Production Library
add_library(matrix src/matrix.c)
target_include_directories(matrix PUBLIC include)
target_compile_options(matrix PRIVATE -O3 -mavx2 -mfma)

# Test-suite
add_executable(test_suite src/matrix.c tests/test_matrix.c)
target_link_libraries(test_suite PRIVATE matrix)
target_compile_options(test_suite PRIVATE -mavx2 -mfma)

# mmul_small benchmark
add_executable(mmul_small_bench benchmarks/mmul_small_bench.c src/matrix.c)
target_include_directories(mmul_small_bench PRIVATE include)
target_compile_options(mmul_small_bench PRIVATE -O3 -mavx2 -mfma)
target_compile_definitions(mmul_small_bench PRIVATE MATRIX_TEST_BUILD)

# mmul_lt benchmark
add_executable(mmul_tpose_bench benchmarks/mmul_tpose_bench.c src/matrix.c)
target_include_directories(mmul_tpose_bench PRIVATE include)
target_compile_options(mmul_tpose_bench PRIVATE -O3 -mavx2 -mfma)
target_compile_definitions(mmul_tpose_bench PRIVATE MATRIX_TEST_BUILD)

# mmul_lt benchmark
add_executable(mmul_lt_bench benchmarks/mmul_lt_bench.c src/matrix.c)
target_include_directories(mmul_lt_bench PRIVATE include)
target_compile_options(mmul_lt_bench PRIVATE -O3 -mavx2 -mfma)
target_compile_definitions(mmul_lt_bench PRIVATE MATRIX_TEST_BUILD)
