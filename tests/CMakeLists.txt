cmake_minimum_required(VERSION 3.31.0)

# Подключение GTest
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    message(STATUS "GTest not found. Trying to download...")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
    )
    FetchContent_MakeAvailable(googletest)
    find_package(GTest REQUIRED)
    if(GTest_FOUND)
        message(STATUS "GTest downloaded and found ^_^. Version: ${GTest_VERSION}")
    else()
        message(FATAL_ERROR "GTest not found after download")
    endif()
else()
    message(STATUS "GTest found ^_^. Version: ${GTest_VERSION}")
endif()

add_executable(TaskTest task_test.cpp)
target_include_directories(TaskTest PRIVATE ${TASK_INCLUDE_DIR} ${nlohmann_json_INCLUDE_DIRS})
target_link_libraries(TaskTest PRIVATE task GTest::GTest GTest::Main)
add_test(NAME Task COMMAND TaskTest)