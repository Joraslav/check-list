# Minimum CMake version
cmake_minimum_required(VERSION 3.31.0)

# Find or fetch GoogleTest
find_package(GTest REQUIRED)

# Create test executable
add_executable(TaskTest
    task_test.cpp
)

# Link libraries to the test target
target_link_libraries(TaskTest
    PRIVATE
    task
    GTest::GTest
    GTest::Main
)

# Add test to CTest
add_test(NAME TaskTest COMMAND TaskTest)
