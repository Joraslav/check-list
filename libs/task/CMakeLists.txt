add_my_library(task
  SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/ListTasks.cpp
  INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  DEPS todo::tools glaze::glaze
)
