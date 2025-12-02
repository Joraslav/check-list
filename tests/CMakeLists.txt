# Minimum CMake version
cmake_minimum_required(VERSION 3.31.0)

# Find or fetch GoogleTest
find_package(GTest REQUIRED)

add_executable(TaskTest
    TestTask.cpp
)
add_executable(ParserTest
    TestParser.cpp
)

target_link_libraries(TaskTest
    PRIVATE
    task
    GTest::GTest
    GTest::Main
)
target_link_libraries(ParserTest
    PRIVATE
    parser
    GTest::GTest
    GTest::Main
)

add_test(NAME Parser COMMAND ParserTest)
add_test(NAME Task COMMAND TaskTest)
