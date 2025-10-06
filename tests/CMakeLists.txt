# Minimum CMake version
cmake_minimum_required(VERSION 3.31.0)

# Find or fetch GoogleTest
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    message(STATUS "GTest not found. Trying to download...")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
    )
    # Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    message(STATUS "GTest downloaded and ready to use.")
else()
    message(STATUS "GTest found ^_^. Version: ${GTest_VERSION}")
endif()

# Create test executable
add_executable(TaskTest
    task_test.cpp
)

# Set include directories for the test target
target_include_directories(TaskTest
    PRIVATE
    ${TASK_INCLUDE_DIR}
)

# Link libraries to the test target
if(GTest_FOUND)
    target_link_libraries(TaskTest
        PRIVATE
        task
        GTest::GTest
        GTest::Main
        nlohmann_json::nlohmann_json
    )
else()
    target_link_libraries(TaskTest
        PRIVATE
        task
        gtest
        gtest_main
        nlohmann_json::nlohmann_json
    )
endif()

# Add test to CTest
add_test(NAME TaskTest COMMAND TaskTest)
