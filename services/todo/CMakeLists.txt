add_service(todo
  MAIN ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  DEPS todo::tools todo::task todo::config
)
