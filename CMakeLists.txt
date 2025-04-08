cmake_minimum_required(VERSION 3.12)
project(Fencing)

# LLVM uses C++17.
set(CMAKE_CXX_STANDARD 17)

# Load LLVMConfig.cmake. If this fails, consider setting `LLVM_DIR` to point
# to your LLVM installation's `lib/cmake/llvm` directory.
find_package(LLVM REQUIRED CONFIG)

# Include the part of LLVM's CMake libraries that defines
# `add_llvm_pass_plugin`.
include(AddLLVM)

# Use LLVM's preprocessor definitions, include directories, and library search
# paths.
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

# Our pass lives in this subdirectory.
add_subdirectory(fencing)

enable_testing()

# Find llvm-lit (customize if needed)
find_program(LLVM_LIT llvm-lit)
if(NOT LLVM_LIT)
  message(FATAL_ERROR "llvm-lit not found. Please install LLVM or set LLVM_LIT path.")
endif()

# Set default lit arguments
set(LIT_ARGS "-v")

set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")

# Add test target
add_custom_target(run-lit-tests
  COMMAND ${LLVM_LIT} ${LIT_ARGS} ${TEST_DIR}
  DEPENDS opt  # Or your custom-built opt tool or pass
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Running LLVM IR tests with lit..."
)

# Make `make test` run lit tests
add_test(NAME run-all-lit-tests
         COMMAND ${LLVM_LIT} ${LIT_ARGS} ${TEST_DIR})
