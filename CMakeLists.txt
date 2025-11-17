cmake_minimum_required(VERSION 3.31.0)

# Project information
project(check-list
    VERSION 0.3.0
    LANGUAGES C CXX
    DESCRIPTION "Check-list application for task management"
)

# C++ standard requirements
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler options
add_compile_options(
    -Wall
    -Wextra
    -pedantic
    -Wno-unused-parameter
)

# Build options
option(BUILD_TEST "Build tests" OFF)
option(DEV_MODE "Using develop mode" ON)

# Task module directories
set(TASK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/task)

# Parse module directories
set(PARSER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/parser)

# Bot module directories
set(BOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/bot)

# Find packages
find_package(nlohmann_json REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)

# Create task library
file(GLOB TASK_INCLUDE ${TASK_DIR}/*.hpp ${TASK_DIR}/*.h)
file(GLOB TASK_SOURCE ${TASK_DIR}/*.cpp)
add_library(task STATIC
    ${TASK_INCLUDE}
    ${TASK_SOURCE}
)
target_include_directories(task PUBLIC ${TASK_DIR})

# Create parser library
file(GLOB PARSER_INCLUDE ${PARSER_DIR}/*.hpp ${PARSER_DIR}/*.h)
file(GLOB PARSER_SOURCE ${PARSER_DIR}/*.cpp)
add_library(parser STATIC
    ${PARSER_INCLUDE}
    ${PARSER_SOURCE}
)
target_include_directories(parser PUBLIC ${PARSER_DIR})

# Create bot library
file(GLOB BOT_INCLUDE ${BOT_DIR}/*.hpp ${BOT_DIR}/*.h)
file(GLOB BOT_SOURCE ${BOT_DIR}/*.cpp)
add_library(bot STATIC
    ${BOT_INCLUDE}
    ${BOT_SOURCE}
)
target_include_directories(bot PUBLIC ${BOT_DIR})

# Create executables
set(TODO_NAME todo)
add_executable(${TODO_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/todo.cpp)
message(STATUS "Maked ${TODO_NAME} executable")

set(BOT_NAME tg-bot)
add_executable(${BOT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/bot.cpp)
message(STATUS "Maked ${BOT_NAME} executable")

# Linking libs
# task-lib
target_include_directories(task PRIVATE ${nlohmann_json_INCLUDE_DIRS})
target_link_libraries(task PRIVATE nlohmann_json::nlohmann_json)
message(STATUS "nlohmann_json linked to ${TODO_NAME}-lib")

# bot-lib
target_include_directories(bot PUBLIC
    ${Boost_INCLUDE_DIRS}
    ${OpenSSL_INCLUDE_DIRS}
    ${nlohmann_json_INCLUDE_DIRS}
)
target_link_libraries(bot PRIVATE
    Boost::system
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
)
if(WIN32)
    target_link_libraries(bot PRIVATE ws2_32)
endif()
message(STATUS "Boost, OpenSSL, nlohmann_json linked to ${BOT_NAME}-lib")

# link todo-executable
target_link_libraries(${TODO_NAME}
    PRIVATE
    task
    parser
)

# link bot-executable
target_link_libraries(${BOT_NAME}
    PRIVATE
    bot
)

# Enable testing if requested
if(BUILD_TEST)
    message(STATUS "Building tests")
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()

# Display project information
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
