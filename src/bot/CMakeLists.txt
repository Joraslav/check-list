file(GLOB BOT_INCLUDE *.hpp *.h)
file(GLOB BOT_SOURCE *.cpp)

add_library(bot STATIC
    ${BOT_INCLUDE}
    ${BOT_SOURCE}
)

target_include_directories(bot PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/task
)

target_include_directories(bot PUBLIC
    ${Boost_INCLUDE_DIRS}
    ${OpenSSL_INCLUDE_DIRS}
    ${nlohmann_json_INCLUDE_DIRS}
)

target_link_libraries(bot PRIVATE
    Boost::system
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
    task
)

if(WIN32)
    target_link_libraries(bot PRIVATE ws2_32)
endif()

message(STATUS "Bot library created")