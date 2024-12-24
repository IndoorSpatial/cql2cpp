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
#include <geos/io/GeoJSONReader.h>
#include <gflags/gflags.h>
#include <glog/export.h>
#include <glog/logging.h>

#include <fstream>
#include <sstream>

#include "geos/io/GeoJSON.h"

DEFINE_string(cql2_query, "", "cql2 query string");
DEFINE_string(cql2_file, "", "a file contains cql2");
DEFINE_string(geojson, "", "data set to be queried");
DEFINE_string(dot, "", "generate dot file");

int main(int argc, char** argv) {
  gflags::SetUsageMessage(
      "cql2 usage: cql2 [-cql2_query=\"city='Toronto'\"] "
      "[-cql2_file=\"path/to/query.cql2\"] "
      "-geojson=\"path/to/geo.json\"");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();
  google::LogToStderr();

  // print flags
  LOG(INFO) << "==== flags ====";
  LOG(INFO) << "query: " << FLAGS_cql2_query;
  LOG(INFO) << "file: " << FLAGS_cql2_file;
  LOG(INFO) << "geojson: " << FLAGS_geojson;
  LOG(INFO) << "dot: " << FLAGS_dot;

  // Parse query as AST
  LOG(INFO) << "==== lexer/parser ====";
  std::istringstream iss(FLAGS_cql2_query);
  std::ostringstream oss;
  Cql2Lexer* lexer = new Cql2Lexer(iss, oss);

  cql2cpp::AstNode::set_ostream(&oss);
  Cql2Parser parser(lexer);
  lexer->RegisterLval(&parser.yylval);
  int ret = parser.yyparse();
  if (ret == 0) {
    LOG(INFO) << "log of generated code of flex and bison:\n" << oss.str();
  } else {
    LOG(ERROR) << "log of generated code of flex and bison:\n" << oss.str();
    return ret;
  }

  // Evaluate value of AST according to data source
  if (not FLAGS_geojson.empty()) {
    LOG(INFO) << "==== evaluation ====";

    // read geojson text
    std::string geojson_text;
    std::ifstream fin(FLAGS_geojson);
    if (fin.good()) {
      if (fin.is_open()) {
        geojson_text.assign(std::istreambuf_iterator<char>(fin),
                            std::istreambuf_iterator<char>());
        fin.close();
      } else {
        std::cerr << "can not open " << FLAGS_geojson;
        goto GEOJSON_FAILED;
      }
    } else {
      std::cerr << FLAGS_geojson << " not exist";
      goto GEOJSON_FAILED;
    }

    // read geojson features
    geos::io::GeoJSONReader reader;
    geos::io::GeoJSONFeatureCollection features =
        reader.readFeatures(geojson_text);

    // evaluate
    cql2cpp::TreeEvaluator tree_evaluator;
    tree_evaluator.RegisterNodeEvaluator(cql2cpp::node_evals);
    for (const auto& feature : features.getFeatures()) {
      cql2cpp::FeatureSourceGeoJson fgs(feature);
      cql2cpp::ValueT result;
      if (tree_evaluator.Evaluate(parser.root(), &fgs, &result)) {
        LOG(INFO) << "evaluation result of feature " << feature.getId() << ": "
                  << (std::get<bool>(result) ? "true" : "false");
      } else {
        LOG(ERROR) << "evaluate error: " << tree_evaluator.error_msg()
                   << std::endl;
      }
    }
  }
GEOJSON_FAILED:

  // save AST to dot file
  if (not FLAGS_dot.empty()) {
    LOG(INFO) << "==== dot ====";
    std::stringstream ss;
    cql2cpp::Tree2Dot::GenerateDot(ss, parser.root());
    LOG(INFO) << ss.str();

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
