/*
 * File Name: main.cc
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/18
 *
 */

#include <cql2cpp/ast_node.h>
#include <cql2cpp/cql2_lexer.h>
#include <cql2cpp/cql2_parser.h>
#include <cql2cpp/evaluator.h>
#include <cql2cpp/feature_source_geojson.h>
#include <cql2cpp/node_evaluator.h>
#include <cql2cpp/tree_dot.h>
#include <geos/geom/GeometryFactory.h>
#include <gflags/gflags.h>

#include <fstream>
#include <sstream>

#include "geos/io/GeoJSON.h"

DEFINE_string(cql2_query, "", "cql2 query string");
DEFINE_string(cql2_file, "", "a file contains cql2");
DEFINE_string(geojson, "", "data set to be queried");
DEFINE_string(dot, "", "generate dot file");

int main(int argc, char** argv) {
  // handle gflags
  gflags::SetUsageMessage(
      "cql2 usage: cql2 [-cql2_query=\"city='Toronto'\"] "
      "[-cql2_file=\"path/to/query.cql2\"] "
      "-geojson=\"path/to/geo.json\"");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  // print flags
  std::cout << "==== flags ====" << std::endl;
  std::cout << "query: " << FLAGS_cql2_query << std::endl;
  std::cout << "file: " << FLAGS_cql2_file << std::endl;
  std::cout << "geojson: " << FLAGS_geojson << std::endl;
  std::cout << "dot: " << FLAGS_dot << std::endl;

  // Parse query as AST
  std::cout << std::endl << "==== lexer/parser ====" << std::endl;
  std::istringstream ss(FLAGS_cql2_query);
  Cql2Lexer* lexer = new Cql2Lexer(ss, std::cout);

  Cql2Parser parser(lexer);
  lexer->RegisterLval(&parser.yylval);
  int ret = parser.yyparse();
  if (ret != 0) return ret;

  // Evaluate value of AST according to data source
  std::cout << std::endl << "==== evaluation ====" << std::endl;
  cql2cpp::TreeEvaluator tree_evaluator;
  tree_evaluator.RegisterNodeEvaluator(cql2cpp::node_evals);

  geos::geom::GeometryFactory::Ptr gf = geos::geom::GeometryFactory::create();
  geos::io::GeoJSONFeature feature(gf->createEmpty(2), {});
  cql2cpp::FeatureSourceGeoJson fgs(feature);

  cql2cpp::ValueT result;
  if (not tree_evaluator.Evaluate(parser.root(), &fgs, &result))
    std::cerr << "evaluate error: " << tree_evaluator.error_msg() << std::endl;

  // save AST to dot file
  if (not FLAGS_dot.empty()) {
    std::cout << std::endl << "==== dot ====" << std::endl;
    std::stringstream ss;
    cql2cpp::Tree2Dot::GenerateDot(ss, parser.root());
    std::cout << ss.str() << std::endl;

    std::string dot_filename = FLAGS_dot;
    if (dot_filename.find(".dot") == std::string::npos) dot_filename += ".dot";

    std::ofstream fout(dot_filename);
    if (fout.is_open()) {
      fout << ss.str();
      fout.close();
    } else {
      std::cerr << "Can not open file " << dot_filename;
    }
  }

  gflags::ShutDownCommandLineFlags();
  return ret;
}
