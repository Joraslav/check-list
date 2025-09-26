cmake_minimum_required(VERSION 3.31.0)
project(check-list VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройка опций сборки
option(BUILD_TEST "Build tests" ON)

# Консольное имя
set(CONSOLE_NAME todo)

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

add_executable(${CONSOLE_NAME} src/main.cpp)

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
