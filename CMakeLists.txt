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
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
    )
    FetchContent_MakeAvailable(nlohmann_json)
    find_package(nlohmann_json REQUIRED)
    if(nlohmann_json_FOUND)
        message(STATUS "nlohmann_json downloaded and found ^_^. Version: ${nlohmann_json_VERSION}")
    else()
        message(FATAL_ERROR "nlohmann_json not found after download")
    endif()
else()
    message(STATUS "nlohmann_json found ^_^. Version: ${nlohmann_json_VERSION}")
endif()

add_library(task STATIC ${TASK_INCLUDE_DIR}/Task.hpp ${TASK_SOURCE_DIR}/Task.cpp)
target_include_directories(task PRIVATE ${TASK_INCLUDE_DIR} ${nlohmann_json_INCLUDE_DIRS})
target_link_libraries(task PRIVATE nlohmann_json::nlohmann_json)

add_executable(${CONSOLE_NAME} src/main.cpp)
target_include_directories(${CONSOLE_NAME} PRIVATE ${TASK_INCLUDE_DIR})
target_link_libraries(${CONSOLE_NAME} PRIVATE task)

if(BUILD_TEST)
    message(STATUS "Builded tests")
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

message(STATUS "Version: ${PROJECT_VERSION}")
