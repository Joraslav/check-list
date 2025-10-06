cmake_minimum_required(VERSION 3.31.0)

# Project information
project(check-list
    VERSION 0.2.0
    LANGUAGES C CXX
    DESCRIPTION "Check-list application for task management"
)

# C++ standard requirements
set(CMAKE_CXX_STANDARD 20)
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
option(DEV_MODE "Using develope mode" ON)

# Project directories
set(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Task module directories
set(TASK_INCLUDE_DIR "${PROJECT_INCLUDE_DIR}/task")
set(TASK_SOURCE_DIR "${PROJECT_SOURCE_DIR}/task")

# Find or fetch nlohmann_json
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found. Trying to download...")
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
    )
    FetchContent_MakeAvailable(nlohmann_json)
    message(STATUS "nlohmann_json downloaded and ready to use.")
else()
    message(STATUS "nlohmann_json found ^_^. Version: ${nlohmann_json_VERSION}")
endif()

# Create task library
add_library(task STATIC
    "${TASK_INCLUDE_DIR}/Task.hpp"
    "${TASK_SOURCE_DIR}/Task.cpp"
)

target_include_directories(task
    PUBLIC
    "$<BUILD_INTERFACE:${TASK_INCLUDE_DIR}>"
    "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(task
    PUBLIC
    nlohmann_json::nlohmann_json
)

# Create main executable
set(CONSOLE_NAME todo)
add_executable(${CONSOLE_NAME}
    src/main.cpp
)

target_include_directories(${CONSOLE_NAME}
    PRIVATE
    ${TASK_INCLUDE_DIR}
)

target_link_libraries(${CONSOLE_NAME}
    PRIVATE
    task
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
