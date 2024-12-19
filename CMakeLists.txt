cmake_minimum_required(VERSION 3.5)
project(cql2cpp
  VERSION 0.1.0
  DESCRIPTION ""
  HOMEPAGE_URL "https://github.com/IndoorSpatial/cql2cpp"
  LANGUAGES CXX
)

find_package(bison REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_BINARY_DIR})

set(FLEX_FILE ${CMAKE_SOURCE_DIR}/src/cql2_lexer.l)
set(FLEX_OUTPUT ${CMAKE_BINARY_DIR}/cql2_lexer.cc)
add_custom_command(
	OUTPUT ${FLEX_OUTPUT}
  COMMAND flex++ --c++ --outfile=${FLEX_OUTPUT} --yyclass=Cql2Lexer ${FLEX_FILE}
	DEPENDS ${FLEX_FILE}
	COMMENT "Generating ${FLEX_OUTPUT} from ${FLEX_FILE} using flex++"
)

set(BISON_FILE ${CMAKE_SOURCE_DIR}/src/cql2_parser.y)
set(BISON_OUTPUT ${CMAKE_BINARY_DIR}/cql2_parser_base.cc)
add_custom_command(
	OUTPUT ${BISON_OUTPUT}
	COMMAND bison++ -d -o ${BISON_OUTPUT} ${BISON_FILE}
	DEPENDS ${BISON_FILE}
	COMMENT "Generating ${BISON_OUTPUT} from ${BISON_FILE} using bison++"
)

add_executable(cql2 src/main.cc ${FLEX_OUTPUT} ${BISON_OUTPUT})
target_compile_options(cql2 PRIVATE -Wno-register)
