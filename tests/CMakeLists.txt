# Minimum CMake version
cmake_minimum_required(VERSION 3.31.0)

# Find or fetch GoogleTest
find_package(GTest REQUIRED)

add_executable(Task
    TestTask.cpp
)
add_executable(Parser
    TestParser.cpp
)

target_link_libraries(Task
    PRIVATE
    task
    GTest::GTest
    GTest::Main
)
target_link_libraries(Parser
    PRIVATE
    parser
    GTest::GTest
    GTest::Main
)

add_test(NAME Parser COMMAND Parser)
add_test(NAME Task COMMAND Task)
