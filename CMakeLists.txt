cmake_minimum_required(VERSION 3.20)

project(tiny_tensor LANGUAGES CXX)

option(ENABLE_CUDA "Build CUDA implementations when a CUDA compiler is available" ON)
option(ENABLE_STRICT_WARNINGS "Treat compiler warnings as errors" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

function(tt_warnings target)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4)
    if(ENABLE_STRICT_WARNINGS)
      target_compile_options(${target} PRIVATE /WX)
    endif()
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    if(ENABLE_STRICT_WARNINGS)
      target_compile_options(${target} PRIVATE -Werror)
    endif()
  endif()
endfunction()

add_library(tt_cpu engine/src/cpu/resize_cpu.cpp)
target_include_directories(tt_cpu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/engine/include)
tt_warnings(tt_cpu)

set(TT_HAVE_CUDA OFF)
if(ENABLE_CUDA)
  include(CheckLanguage)
  check_language(CUDA)
  if(CMAKE_CUDA_COMPILER)
    enable_language(CUDA)
    set(TT_HAVE_CUDA ON)
    find_package(CUDAToolkit REQUIRED)

    add_library(tt_cuda engine/src/cuda/resize_cuda.cu)
    target_include_directories(tt_cuda PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/engine/include)
    target_link_libraries(tt_cuda PUBLIC CUDA::cudart)
    target_compile_definitions(tt_cuda PUBLIC TT_HAS_CUDA=1)
    set_target_properties(tt_cuda PROPERTIES
      CUDA_STANDARD 17
      CUDA_STANDARD_REQUIRED ON
      CUDA_SEPARABLE_COMPILATION ON)
  else()
    message(WARNING "ENABLE_CUDA=ON but nvcc was not found; building CPU targets only")
  endif()
endif()

add_executable(resize_demo engine/apps/resize_demo.cpp)
target_link_libraries(resize_demo PRIVATE tt_cpu)
tt_warnings(resize_demo)
if(TT_HAVE_CUDA)
  target_link_libraries(resize_demo PRIVATE tt_cuda)
endif()

add_executable(resize_benchmark engine/bench/benchmark_resize.cpp)
target_link_libraries(resize_benchmark PRIVATE tt_cpu)
tt_warnings(resize_benchmark)
# docs/benchmark-methodology.md requires build type and compiler on every run.
# The commit is passed at run time via TT_COMMIT instead, so it cannot go
# stale when the source is rebuilt without reconfiguring.
target_compile_definitions(resize_benchmark PRIVATE
  TT_BUILD_TYPE="$<IF:$<BOOL:${CMAKE_BUILD_TYPE}>,${CMAKE_BUILD_TYPE},unspecified>"
  TT_COMPILER="${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

enable_testing()
add_executable(resize_tests engine/tests/test_resize.cpp)
target_link_libraries(resize_tests PRIVATE tt_cpu)
target_include_directories(resize_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/engine/tests)
tt_warnings(resize_tests)
add_test(NAME resize_correctness COMMAND resize_tests)

if(TT_HAVE_CUDA)
  # The CPU reference is the oracle for the CUDA kernel. Without this target
  # `ctest --test-dir build-cuda` would run only CPU tests, so no automated
  # check would cover the CUDA path at all.
  # A .cu source keeps CUDA as the target's language so CMake performs the
  # device link that tt_cuda's CUDA_SEPARABLE_COMPILATION requires.
  add_executable(resize_cuda_tests engine/tests/test_resize_cuda.cu)
  target_link_libraries(resize_cuda_tests PRIVATE tt_cpu tt_cuda)
  target_include_directories(resize_cuda_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/engine/tests)
  set_target_properties(resize_cuda_tests PROPERTIES
    CUDA_STANDARD 17
    CUDA_STANDARD_REQUIRED ON)
  add_test(NAME resize_cuda_parity COMMAND resize_cuda_tests)
  # Reported as skipped, not failed, when the toolkit is present but no GPU is.
  set_tests_properties(resize_cuda_parity PROPERTIES SKIP_RETURN_CODE 77)
endif()

message(STATUS "CUDA targets enabled: ${TT_HAVE_CUDA}")

