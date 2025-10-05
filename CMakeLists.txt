cmake_minimum_required(VERSION 3.31.0)
project(check-list VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
    -Wall
    -Wextra
    -pedantic
    -Wno-unused-parameter
)

# Настройка опций сборки
option(BUILD_TEST "Build tests" ON)

# Консольное имя
set(CONSOLE_NAME todo)

# Устновка переменных путей
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(TASK_INCLUDE_DIR ${INCLUDE_DIR}/task)
set(TASK_SOURCE_DIR ${SOURCE_DIR}/task)

# Поиск nlohmann_json
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found. Trying to download...")
    include(FetchContent)
    FetchContent_Declare(
        # json
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        # URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
        GIT_TAG v3.11.3
    )
    FetchContent_MakeAvailable(nlohmann_json)
    message(STATUS "nlohmann_json downloaded and ready to use.")
else()
    message(STATUS "nlohmann_json found ^_^. Version: ${nlohmann_json_VERSION}")
endif()

add_library(task STATIC ${TASK_INCLUDE_DIR}/Task.hpp ${TASK_SOURCE_DIR}/Task.cpp)
target_include_directories(task PRIVATE ${TASK_INCLUDE_DIR} ${nlohmann_json_INCLUDE_DIRS})
target_link_libraries(task PRIVATE nlohmann_json::nlohmann_json)

add_executable(${CONSOLE_NAME} src/main.cpp)
target_include_directories(${CONSOLE_NAME} PRIVATE ${TASK_INCLUDE_DIR})
target_link_libraries(${CONSOLE_NAME} PRIVATE task)

# Установка целей
install(TARGETS ${CONSOLE_NAME} task
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Установка заголовочных файлов
install(DIRECTORY ${INCLUDE_DIR}/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

if(BUILD_TEST)
    message(STATUS "Builded tests")
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

# CPack settings
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Check-list application for task management")
set(CPACK_PACKAGE_VENDOR "Dol")
set(CPACK_PACKAGE_CONTACT "yar.mozg2002@gmail.com")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
set(CPACK_PACKAGE_EXECUTABLES "${CONSOLE_NAME}" "Check List Application")

# Component settings
set(CPACK_COMPONENTS_ALL applications)
set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "Check List Application")
set(CPACK_COMPONENT_APPLICATIONS_DESCRIPTION "The main check list application")

# Debian package settings
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name <your.email@example.com>")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.2.1)")

# Windows package settings
set(CPACK_NSIS_DISPLAY_NAME "Check List Application")
set(CPACK_NSIS_PACKAGE_NAME "Check List Application")
set(CPACK_NSIS_CONTACT "info@example.com")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH ON)

# Generators
set(CPACK_GENERATOR "TGZ;DEB;NSIS")

include(CPack)

message(STATUS "Version: ${PROJECT_VERSION}")
