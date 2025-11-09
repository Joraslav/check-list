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

# Project directories
set(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Task module directories
set(TASK_INCLUDE_DIR "${PROJECT_INCLUDE_DIR}/task")
set(TASK_SOURCE_DIR "${PROJECT_SOURCE_DIR}/task")

# Parse module directories
set(PARSER_INCLUDE_DIR "${PROJECT_INCLUDE_DIR}/parser")
set(PARSER_SOURCE_DIR "${PROJECT_SOURCE_DIR}/parser")

# Try Conan for dependency management
find_program(CONAN_CMD conan)
if(CONAN_CMD)
    message(STATUS "Found Conan, installing dependencies...")
    execute_process(
        COMMAND ${CONAN_CMD} install ${CMAKE_SOURCE_DIR} --output-folder=${CMAKE_BUILD_DIR} --build=missing
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        RESULT_VARIABLE CONAN_RESULT
    )
    if(CONAN_RESULT EQUAL 0)
        message(STATUS "Conan install successful")
        # Include the toolchain file if it exists
        if(EXISTS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
            include("${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
        elseif(EXISTS "${CMAKE_BINARY_DIR}/Release/generators/conan_toolchain.cmake")
            include("${CMAKE_BINARY_DIR}/Release/generators/conan_toolchain.cmake")
        endif()
    else()
        message(WARNING "Conan install failed with result: ${CONAN_RESULT}")
    endif()
endif()

# Find packages
find_package(nlohmann_json QUIET)
find_package(Boost QUIET COMPONENTS system)
find_package(OpenSSL QUIET)

# Create task library
add_library(task STATIC
    "${TASK_INCLUDE_DIR}/Task.hpp"
    "${TASK_SOURCE_DIR}/Task.cpp"
)

# Create parser library
add_library(parser STATIC
    "${PARSER_INCLUDE_DIR}/Parser.hpp"
    "${PARSER_SOURCE_DIR}/Parser.cpp"
)

target_include_directories(task
    PUBLIC
    "$<BUILD_INTERFACE:${TASK_INCLUDE_DIR}>"
    "$<INSTALL_INTERFACE:include>"
)

target_include_directories(parser
    PUBLIC
    "$<BUILD_INTERFACE:${PARSER_INCLUDE_DIR}>"
    "$<INSTALL_INTERFACE:include>"
)

# Link nlohmann_json to task library if found
if(TARGET nlohmann_json::nlohmann_json)
    target_link_libraries(task
        PUBLIC
        nlohmann_json::nlohmann_json
    )
    message(STATUS "Linked nlohmann_json to task library")
elseif(nlohmann_json_FOUND)
    # Alternative way to link if target is not available
    target_link_libraries(task PUBLIC nlohmann_json::nlohmann_json)
    message(STATUS "Linked nlohmann_json through find_package")
else()
    message(WARNING "nlohmann_json not found, compilation may fail if needed")
endif()

# Create main executable
set(CONSOLE_NAME todo)
add_executable(${CONSOLE_NAME}
    src/main.cpp
)

target_include_directories(${CONSOLE_NAME}
    PRIVATE
    ${TASK_INCLUDE_DIR}
    ${PARSER_INCLUDE_DIR}
)

target_link_libraries(${CONSOLE_NAME}
    PRIVATE
    task
    parser
)

# Installation rules
include(GNUInstallDirs)
install(TARGETS ${CONSOLE_NAME}
    EXPORT ${PROJECT_NAME}-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Note: task library is not exported because it depends on nlohmann_json which is fetched externally
# Users of the installed package should fetch nlohmann_json separately if they want to use the task library
# For most users, the todo executable is sufficient

install(DIRECTORY ${PROJECT_INCLUDE_DIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

install(EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}-targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Enable testing if requested
if(BUILD_TEST)
    message(STATUS "Building tests")
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()

# CPack configuration
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

# General CPack settings
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Check-list application for task management")
set(CPACK_PACKAGE_VENDOR "Dol")
set(CPACK_PACKAGE_CONTACT "yar.mozg2002@gmail.com")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")

# Component settings
set(CPACK_COMPONENTS_ALL applications)
set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "Check List")
set(CPACK_COMPONENT_APPLICATIONS_DESCRIPTION "The main check list")

# Debian package settings
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Yaroslav Moskalev yar.mozg2002@gmail.com")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.2.1)")

# Windows package settings
set(CPACK_NSIS_DISPLAY_NAME "Check List")
set(CPACK_NSIS_PACKAGE_NAME "Check List")
set(CPACK_NSIS_CONTACT "yar.mozg2002@gmail.com")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH ON)

# Package generators
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

# Display project information
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")

# Display dependency status
if(TARGET nlohmann_json::nlohmann_json)
    message(STATUS "nlohmann_json: FOUND (target)")
elseif(nlohmann_json_FOUND)
    message(STATUS "nlohmann_json: FOUND (package)")
else()
    message(STATUS "nlohmann_json: NOT FOUND")
endif()

if(Boost_FOUND)
    message(STATUS "Boost: FOUND")
else()
    message(STATUS "Boost: NOT FOUND")
endif()

if(OpenSSL_FOUND)
    message(STATUS "OpenSSL: FOUND")
else()
    message(STATUS "OpenSSL: NOT FOUND")
endif()
