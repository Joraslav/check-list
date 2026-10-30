add_my_library(io
  INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Json.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Yaml.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Cli.cpp
  DEPS glaze::glaze todo::tools todo::task
)
