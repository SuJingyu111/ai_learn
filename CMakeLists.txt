cmake_minimum_required(VERSION 3.20)

project(cuda_ai_systems LANGUAGES CXX)

option(ENABLE_CUDA "Build CUDA implementations when a CUDA compiler is available" ON)
option(ENABLE_STRICT_WARNINGS "Treat compiler warnings as errors" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

function(cuda_ai_warnings target)
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

add_library(cuda_ai_cpu src/cpu/resize_cpu.cpp)
target_include_directories(cuda_ai_cpu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
cuda_ai_warnings(cuda_ai_cpu)

set(CUDA_AI_HAVE_CUDA OFF)
if(ENABLE_CUDA)
  include(CheckLanguage)
  check_language(CUDA)
  if(CMAKE_CUDA_COMPILER)
    enable_language(CUDA)
    set(CUDA_AI_HAVE_CUDA ON)
    find_package(CUDAToolkit REQUIRED)

    add_library(cuda_ai_cuda src/cuda/resize_cuda.cu)
    target_include_directories(cuda_ai_cuda PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(cuda_ai_cuda PUBLIC CUDA::cudart)
    target_compile_definitions(cuda_ai_cuda PUBLIC CUDA_AI_HAS_CUDA=1)
    set_target_properties(cuda_ai_cuda PROPERTIES
      CUDA_STANDARD 17
      CUDA_STANDARD_REQUIRED ON
      CUDA_SEPARABLE_COMPILATION ON)
  else()
    message(WARNING "ENABLE_CUDA=ON but nvcc was not found; building CPU targets only")
  endif()
endif()

add_executable(resize_demo apps/resize_demo.cpp)
target_link_libraries(resize_demo PRIVATE cuda_ai_cpu)
cuda_ai_warnings(resize_demo)
if(CUDA_AI_HAVE_CUDA)
  target_link_libraries(resize_demo PRIVATE cuda_ai_cuda)
endif()

add_executable(resize_benchmark benchmarks/benchmark_resize.cpp)
target_link_libraries(resize_benchmark PRIVATE cuda_ai_cpu)
cuda_ai_warnings(resize_benchmark)
# docs/benchmark-methodology.md requires build type and compiler on every run.
# The commit is passed at run time via CUDA_AI_COMMIT instead, so it cannot go
# stale when the source is rebuilt without reconfiguring.
target_compile_definitions(resize_benchmark PRIVATE
  CUDA_AI_BUILD_TYPE="$<IF:$<BOOL:${CMAKE_BUILD_TYPE}>,${CMAKE_BUILD_TYPE},unspecified>"
  CUDA_AI_COMPILER="${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

enable_testing()
add_executable(resize_tests tests/test_resize.cpp)
target_link_libraries(resize_tests PRIVATE cuda_ai_cpu)
cuda_ai_warnings(resize_tests)
add_test(NAME resize_correctness COMMAND resize_tests)

message(STATUS "CUDA targets enabled: ${CUDA_AI_HAVE_CUDA}")

