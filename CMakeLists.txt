cmake_minimum_required(VERSION 3.31.0)
project(check-list
    VERSION 0.3.0
    LANGUAGES C CXX
    DESCRIPTION "Check-list application for task management and Telegram Bot"
    HOMEPAGE_URL "https://github.com/Joraslav/check-list"
)

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
add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)

# Build options
option(BUILD_TEST "Build tests" OFF)
option(DEV_MODE "Using develop mode" ON)

# Find packages
find_package(nlohmann_json REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)

# Add subdirectories for each module
add_subdirectory(src/task)
add_subdirectory(src/parser)
add_subdirectory(src/bot)

# Create executables
set(TODO_NAME todo)
add_executable(${TODO_NAME} src/todo.cpp)
message(STATUS "Maked ${TODO_NAME} executable")

set(BOT_NAME tg-bot)
add_executable(${BOT_NAME} src/bot.cpp)
message(STATUS "Maked ${BOT_NAME} executable")

# Linking libs
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
