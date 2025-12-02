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
if(DEV_MODE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_TEST ON)
endif()

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

# Include directories for executables
target_include_directories(${BOT_NAME} PRIVATE src/task)

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

# CPack configuration for packaging and installation
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Joraslav")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Check-list application for task management and Telegram Bot")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "https://github.com/Joraslav/check-list")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# Installation targets
install(TARGETS ${TODO_NAME}
    RUNTIME DESTINATION bin
)

# Install documentation
install(FILES README.md README_RU.md LICENSE
    DESTINATION share/doc/${PROJECT_NAME}
)

# CPack generators for different platforms
if(DEV_MODE)
    set(CPACK_GENERATOR "TGZ;DEB;NSIS")
else()
    if(WIN32)
        set(CPACK_GENERATOR "NSIS")
    elseif(UNIX AND NOT APPLE)
        set(CPACK_GENERATOR "TGZ;DEB")
    elseif(APPLE)
        set(CPACK_GENERATOR "TGZ")
    endif()
endif()

include(CPack)
