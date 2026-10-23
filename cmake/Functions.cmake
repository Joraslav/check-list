function(find_lib LIB_NAME)
  find_package(${LIB_NAME} QUIET)
  if(NOT ${LIB_NAME}_FOUND)
    unset(${LIB_NAME}_DIR CACHE)
    list(PREPEND CMAKE_PREFIX_PATH
      ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/generators)
    find_package(${LIB_NAME} QUIET PATHS
      ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/generators NO_DEFAULT_PATH)
    if(NOT ${LIB_NAME}_FOUND)
      message(FATAL_ERROR "Lib ${LIB_NAME} not found. Execute 'create_env'")
    endif()
  endif()
endfunction()

# ADD_LOGGER_LIBRARY(<name> SOURCES <files...> [INCLUDE_DIR <dir>] [DEPS <targets...>])
# Creates STATIC target <name> with alias logger::<name>; headers live next to sources.
function(add_my_library TARGET_NAME)
  cmake_parse_arguments(ARG "" "INCLUDE_DIR" "SOURCES;DEPS" ${ARGN})

  if(NOT ARG_INCLUDE_DIR)
    set(ARG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
  endif()

  add_library(${TARGET_NAME} STATIC ${ARG_SOURCES})
  add_library(talker::${TARGET_NAME} ALIAS ${TARGET_NAME})
  target_include_directories(${TARGET_NAME} PUBLIC
    $<BUILD_INTERFACE:${ARG_INCLUDE_DIR}>
  )
  target_link_libraries(${TARGET_NAME} PUBLIC ${ARG_DEPS})
endfunction()

# ADD_HEADER_ONLY_LIBRARY(<name> [INCLUDE_DIR <dir>] [DEPS <targets...>])
# Creates an INTERFACE library with the multiecho::<name> alias.
function(add_header_only_library TARGET_NAME)
  cmake_parse_arguments(ARG "" "INCLUDE_DIR" "DEPS" ${ARGN})

  if(NOT ARG_INCLUDE_DIR)
    set(ARG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
  endif()

  add_library(${TARGET_NAME} INTERFACE)
  add_library(talker::${TARGET_NAME} ALIAS ${TARGET_NAME})
  target_include_directories(${TARGET_NAME} INTERFACE
    $<BUILD_INTERFACE:${ARG_INCLUDE_DIR}>
  )
  target_link_libraries(${TARGET_NAME} INTERFACE ${ARG_DEPS})
endfunction()

# ADD_SERVICE(<name> MAIN <file> [DOMAIN_SOURCES <files...>] [EXECUTABLE <name>] [DEPS <targets...>])
# Creates an optional testable STATIC <name>_domain library and an executable
# (default <name>-service).
function(add_service SERVICE_NAME)
  cmake_parse_arguments(ARG "" "MAIN;EXECUTABLE" "DOMAIN_SOURCES;DEPS" ${ARGN})

  if(NOT ARG_EXECUTABLE)
    set(ARG_EXECUTABLE ${SERVICE_NAME})
  endif()

  add_executable(${ARG_EXECUTABLE} ${ARG_MAIN})
  if(ARG_DOMAIN_SOURCES)
    add_my_library(${SERVICE_NAME}_domain
      SOURCES ${ARG_DOMAIN_SOURCES}
      DEPS ${ARG_DEPS}
    )
    target_link_libraries(${ARG_EXECUTABLE} PRIVATE ${SERVICE_NAME}_domain)
  else()
    target_link_libraries(${ARG_EXECUTABLE} PRIVATE ${ARG_DEPS})
  endif()
endfunction()

# ADD_GTEST_TARGET(<name> [SOURCES <files...>] [LINK_LIBS <targets...>] [COMPILE_DEFINITIONS <defs...>])
function(add_gtest_target TEST_NAME)
  cmake_parse_arguments(ARG "" "" "SOURCES;LINK_LIBS;COMPILE_DEFINITIONS" ${ARGN})

  if(NOT ARG_SOURCES)
    set(ARG_SOURCES ${TEST_NAME}.cpp)
  endif()

  add_executable(${TEST_NAME} ${ARG_SOURCES})
  target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest_main ${ARG_LINK_LIBS})

  if(ARG_COMPILE_DEFINITIONS)
    target_compile_definitions(${TEST_NAME} PRIVATE ${ARG_COMPILE_DEFINITIONS})
  endif()

  gtest_discover_tests(${TEST_NAME} NO_PRETTY_VALUES)
endfunction()
