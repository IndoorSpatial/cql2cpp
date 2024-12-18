cmake_minimum_required(VERSION 3.5)
project(cql2cpp
  VERSION 0.1.0
  DESCRIPTION ""
  HOMEPAGE_URL "https://github.com/IndoorSpatial/cql2cpp"
  LANGUAGES CXX
)

find_package(bison REQUIRED)

set(FLEX_FILE ${CMAKE_SOURCE_DIR}/src/cql2_lexer.l)
set(FLEX_OUTPUT ${CMAKE_BINARY_DIR}/cql2_lexer.cc)
add_custom_command(
	OUTPUT ${FLEX_OUTPUT}
  COMMAND flex++ --c++ --outfile=${FLEX_OUTPUT} ${FLEX_FILE}
	DEPENDS ${FLEX_FILE}
	COMMENT "Generating ${FLEX_OUTPUT} from ${FLEX_FILE} using flex"
)

add_executable(cql2 src/main.cc ${FLEX_OUTPUT})
