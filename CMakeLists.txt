cmake_minimum_required(VERSION 3.5)
project(cql2cpp
  VERSION 0.1.0
  DESCRIPTION ""
  HOMEPAGE_URL "https://github.com/IndoorSpatial/cql2cpp"
  LANGUAGES CXX
)

find_package(GTest REQUIRED)
find_package(geos REQUIRED)
find_package(glog REQUIRED)

find_package(GFlags QUIET)
find_package(gflags QUIET)
if (NOT GFlags_FOUND AND NOT gflags_FOUND)
  message(FATAL_ERROR "can not find gflags")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Lexer
set(FLEX_FILE ${CMAKE_SOURCE_DIR}/src/cql2_lexer.l)
set(FLEX_OUTPUT ${CMAKE_BINARY_DIR}/cql2_lexer.cc)
add_custom_command(
  OUTPUT ${FLEX_OUTPUT}
  COMMAND flex++ --c++ --outfile=${FLEX_OUTPUT} --yyclass=Cql2Lexer ${FLEX_FILE}
  DEPENDS ${FLEX_FILE}
  COMMENT "Generating ${FLEX_OUTPUT} from ${FLEX_FILE} using flex++"
)

# Parser
set(BISON_FILE ${CMAKE_SOURCE_DIR}/src/cql2_parser.y)
set(BISON_OUTPUT ${CMAKE_BINARY_DIR}/cql2_parser_base.cc)
add_custom_command(
  OUTPUT ${BISON_OUTPUT}
  COMMAND bison++ -d -o ${BISON_OUTPUT} ${BISON_FILE}
  DEPENDS ${BISON_FILE}
  COMMENT "Generating ${BISON_OUTPUT} from ${BISON_FILE} using bison++"
)

add_definitions(-DUSE_UNSTABLE_GEOS_CPP_API)

# library
add_library(cql2cpp src/id_generator.cc src/ast_node.cc ${FLEX_OUTPUT} ${BISON_OUTPUT})
target_compile_options(cql2cpp PRIVATE -Wno-register -Wno-write-strings)
target_link_libraries(cql2cpp GEOS::geos glog::glog)

# CLI tool
add_executable(cql2 src/main.cc)
target_compile_options(cql2 PRIVATE -Wno-register -Wno-write-strings)
target_link_libraries(cql2 cql2cpp GEOS::geos glog::glog)

# Test
enable_testing()
add_executable(test_parse test/test_parse.cc)
target_link_libraries(test_parse GTest::GTest GTest::Main)
add_test(NAME test_parse COMMAND test_parse)
