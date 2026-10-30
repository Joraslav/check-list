add_my_library(config
  INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Config.cpp
  DEPS todo::tools glaze::glaze
)

