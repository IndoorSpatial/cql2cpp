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
#include <cql2cpp/node_evaluator.h>
#include <cql2cpp/evaluator.h>
#include <cql2cpp/tree_dot.h>
#include <gflags/gflags.h>

#include <fstream>
#include <sstream>

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
  std::cout << "query: " << FLAGS_cql2_query << std::endl;
  std::cout << "file: " << FLAGS_cql2_file << std::endl;
  std::cout << "geojson: " << FLAGS_geojson << std::endl;
  std::cout << "dot: " << FLAGS_dot << std::endl;

  // Parse query as AST
  std::istringstream ss(FLAGS_cql2_query);
  Cql2Lexer* lexer = new Cql2Lexer(ss, std::cout);
  Cql2Parser parser(lexer);
  lexer->RegisterLval(&parser.yylval);
  int ret = parser.yyparse();
  if (ret != 0) return ret;

  // Evaluate value of AST according to data source
  cql2cpp::TreeEvaluator tree_evaluator;
  tree_evaluator.RegisterNodeEvaluator(cql2cpp::node_evals);

  cql2cpp::ValueT result;
  cql2cpp::DataSource ds;
  if (tree_evaluator.Evaluate(parser.root(), ds, &result))
    std::cout << std::get<bool>(result) << std::endl;
  else
    std::cerr << "evaluate error: " << tree_evaluator.error_msg() << std::endl;

  // save AST to dot file
  if (not FLAGS_dot.empty()) {
    std::string dot_filename = FLAGS_dot;
    if (dot_filename.find(".dot") == std::string::npos) dot_filename += ".dot";
    std::ofstream fout(dot_filename);

    if (fout.is_open()) {
      cql2cpp::Tree2Dot::GenerateDot(fout, parser.root());
      fout.close();
    } else {
      std::cerr << "Can not open file " << dot_filename;
    }
  }

  gflags::ShutDownCommandLineFlags();
  return ret;
}
