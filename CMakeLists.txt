cmake_minimum_required(VERSION 3.5)
project(cql2cpp
  VERSION 0.1.0
  DESCRIPTION ""
  HOMEPAGE_URL "https://github.com/IndoorSpatial/cql2cpp"
  LANGUAGES CXX
)

find_package(bison REQUIRED)

add_executable(cql2 src/main.cc)
