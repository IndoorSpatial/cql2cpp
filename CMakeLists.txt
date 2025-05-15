cmake_minimum_required(VERSION 3.5)
project(cql2cpp
  VERSION 0.3.0
  DESCRIPTION ""
  HOMEPAGE_URL "https://github.com/IndoorSpatial/cql2cpp"
  LANGUAGES CXX
)
add_definitions(-DCQL2CPP_VERSION="${PROJECT_VERSION}")


find_package(catkin QUIET)
find_package(catkin_simple QUIET)
if (catkin_simple_FOUND)
  catkin_package()
  catkin_simple()
endif()

find_package(SQLite3 QUIET)
find_package(libspatialite QUIET)
if (SQLite3_FOUND AND libspatialite_FOUND)
  set(SQLITE_LIBS SQLite::SQLite3 libspatialite::libspatialite)
else()
  set(SQLITE_LIBS sqlite3 spatialite)
endif()

find_package(GTest REQUIRED)
find_package(geos REQUIRED)
find_package(glog REQUIRED)

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

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
set(BISON_OUTPUT_HH ${CMAKE_BINARY_DIR}/cql2cpp/cql2_parser_base.hh)
set(BISON_OUTPUT_CC ${CMAKE_BINARY_DIR}/cql2cpp/cql2_parser_base.cc)
add_custom_command(
  OUTPUT ${BISON_OUTPUT_CC}
  COMMAND mkdir -p ${CMAKE_BINARY_DIR}/cql2cpp && bison -v --header=${BISON_OUTPUT_HH} --output=${BISON_OUTPUT_CC} ${BISON_FILE} -W -Wcounterexamples
  DEPENDS ${BISON_FILE}
  COMMENT "Generating ${BISON_OUTPUT_CC} from ${BISON_FILE} using bison"
)
install(DIRECTORY
  ${CMAKE_BINARY_DIR}/cql2cpp
  DESTINATION include
  FILES_MATCHING PATTERN "*.hh"
)

add_definitions(-DUSE_UNSTABLE_GEOS_CPP_API)

set(CQL2CPP_SRC
  src/id_generator.cc
  src/ast_node.cc
  src/global_yylex.cc
  src/value.cc
  ${FLEX_OUTPUT}
  ${BISON_OUTPUT_CC}
)

# library
if (catkin_simple_FOUND)
  cs_add_library(cql2cpp ${CQL2CPP_SRC})
else()
  add_library(cql2cpp ${CQL2CPP_SRC})
endif()
target_compile_options(cql2cpp PRIVATE -Wno-register -Wno-write-strings)
target_link_libraries(cql2cpp GEOS::geos glog::glog)

# CLI tool
if (catkin_simple_FOUND)
  cs_add_executable(cql2 src/main.cc)
else()
  add_executable(cql2 src/main.cc)
endif()
target_compile_options(cql2 PRIVATE -Wno-register -Wno-write-strings)
target_link_libraries(cql2 cql2cpp GEOS::geos glog::glog)

# Test
enable_testing()
set(TEST_DIR ${CMAKE_SOURCE_DIR}/test)

add_executable(test_parse ${TEST_DIR}/test_parse.cc)
target_link_libraries(test_parse cql2cpp GTest::GTest GTest::Main glog::glog GEOS::geos)
add_test(NAME test_parse COMMAND test_parse WORKING_DIRECTORY ${TEST_DIR})

add_executable(test_bbox_reader ${TEST_DIR}/test_bbox_reader.cc)
target_link_libraries(test_bbox_reader GTest::GTest GTest::Main glog::glog GEOS::geos)
add_test(NAME test_bbox_reader COMMAND test_bbox_reader WORKING_DIRECTORY ${TEST_DIR})

add_executable(test_sql ${TEST_DIR}/test_sql.cc)
target_link_libraries(test_sql cql2cpp GTest::GTest GTest::Main glog::glog ${SQLITE_LIBS})
add_test(NAME test_sql COMMAND test_sql WORKING_DIRECTORY ${TEST_DIR})


if (catkin_simple_FOUND)
  cs_install()
  cs_export()
else()
  install(DIRECTORY
    ${CMAKE_SOURCE_DIR}/include/cql2cpp
    ${CMAKE_SOURCE_DIR}/include/argparse
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
  )
  install(TARGETS cql2cpp LIBRARY DESTINATION lib/)
  install(TARGETS cql2 RUNTIME DESTINATION lib/${PROJECT_NAME}/)
endif()
