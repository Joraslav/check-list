cmake_minimum_required(VERSION 4.3.0)
project(todo VERSION 0.4.0 LANGUAGES CXX)

option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_TEST "Build test" ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(CompilerOptions)
include(Functions)
include(Dependencies)

find_dependencies()

add_subdirectory(libs)
add_subdirectory(services)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BUILD_TESTING)
  include(GoogleTest)
  include(CTest)
  add_subdirectory(tests)
endif()

# clangd looks for the compilation database in the project root (see .clangd).
file(CREATE_LINK ${CMAKE_BINARY_DIR}/compile_commands.json
  ${CMAKE_SOURCE_DIR}/compile_commands.json SYMBOLIC)
