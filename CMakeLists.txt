cmake_minimum_required(VERSION 3.13)
project(PerlCheckModule C CXX)

set(EXPORT_COMPILE_COMMANDS 1)

get_filename_component(tmp "${CMAKE_CURRENT_SOURCE_DIR}" PATH)
get_filename_component(CMake_SOURCE_DIR "${tmp}" PATH)

find_package(Clang REQUIRED)

if(LLVM_VERSION_MAJOR GREATER_EQUAL 16)
  set(CMAKE_CXX_STANDARD 17)
else()
  set(CMAKE_CXX_STANDARD 14)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_library(perl-check MODULE
  PerlCheck.cpp
  PerlLiteralFunctionCheck.cpp
  PerlUndefSetsvCheck.cpp
  PerlMortalFunctionCheck.cpp
  PerlUndefSetpvXCheck.cpp
  )
target_include_directories(perl-check PRIVATE ${CLANG_INCLUDE_DIRS})
target_link_libraries(perl-check PRIVATE clang-tidy)
# FIXME: this results in lots of warnings from the LLVM header
#target_compile_options(perl-check PRIVATE -Wall -Wextra)
target_compile_options(perl-check PRIVATE -fno-rtti)

option(RUN_TESTS "Run the tests for the perl-check module" ON)
if(RUN_TESTS)
  enable_testing()
  add_subdirectory(Tests)
endif()
