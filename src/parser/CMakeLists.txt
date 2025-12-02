file(GLOB PARSER_INCLUDE *.hpp *.h)
file(GLOB PARSER_SOURCE *.cpp)

add_library(parser STATIC
    ${PARSER_INCLUDE}
    ${PARSER_SOURCE}
)

target_include_directories(parser PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

message(STATUS "Parser library created")