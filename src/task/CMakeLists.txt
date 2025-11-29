file(GLOB TASK_INCLUDE *.hpp *.h)
file(GLOB TASK_SOURCE *.cpp)

add_library(task STATIC
    ${TASK_INCLUDE}
    ${TASK_SOURCE}
)

target_include_directories(task PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(task PRIVATE ${nlohmann_json_INCLUDE_DIRS})
target_link_libraries(task PRIVATE nlohmann_json::nlohmann_json)

message(STATUS "Task library created")