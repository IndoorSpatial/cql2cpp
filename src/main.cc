/*
 * File Name: main.cc
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/18
 *
 */

#include <sstream>

#include <cql2cpp/cql2_lexer.h>
#include <cql2cpp/cql2_parser.h>
#include <gflags/gflags.h>

DEFINE_string(cql2_query, "", "cql2 query string");
DEFINE_string(cql2_file, "", "a file contains cql2");
DEFINE_string(geojson, "", "data set to be queried");

int main(int argc, char** argv) {
  gflags::SetUsageMessage(
      "cql2 usage: cql2 [-cql2_query=\"city='Toronto'\"] "
      "[-cql2_file=\"path/to/query.cql2\"] "
      "-geojson=\"path/to/geo.json\"");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  std::cout << "query: " << FLAGS_cql2_query << std::endl;
  std::cout << "file: " << FLAGS_cql2_file << std::endl;
  std::cout << "geojson: " << FLAGS_geojson << std::endl;

  std::istringstream ss(FLAGS_cql2_query);
  Cql2Lexer* lexer = new Cql2Lexer(ss, std::cout);
  Cql2Parser parser(lexer);
  lexer->RegisterLval(&parser.yylval);

  int ret = parser.yyparse();

  gflags::ShutDownCommandLineFlags();
  return ret;
}
