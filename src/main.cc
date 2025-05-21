/*
 * File Name: main.cc
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/18
 *
 */

#include <cql2cpp/cql2cpp.h>
#include <cql2cpp/feature_source.h>
#include <cql2cpp/feature_source_geojson.h>
#include <cql2cpp/global_yylex.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Point.h>
#include <geos/io/GeoJSON.h>
#include <geos/io/GeoJSONReader.h>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include <argparse/argparse.hpp>
#include <fstream>

#ifndef CQL2CPP_VERSION
#define CQL2CPP_VERSION "0.0.0"
#endif

int main(int argc, char** argv) {
  argparse::ArgumentParser program("cql2", CQL2CPP_VERSION);

  argparse::ArgumentParser parse_command("parse", "",
                                         argparse::default_arguments::help);
  parse_command.add_description("parse a cql2 query and print dot file");
  parse_command.add_argument("query").help("cql2 query string");
  parse_command.add_argument("-V", "--verbose")
      .help("print verbose debug log")
      .flag();
  parse_command.add_argument("-O", "--output").help("the output dot file");

  argparse::ArgumentParser filter_command("filter", "",
                                          argparse::default_arguments::help);
  filter_command.add_description(
      "filter features from geojson data collection");
  filter_command.add_argument("query").help("cql2 query string");
  filter_command.add_argument("--features")
      .help(
          "geojson file contains multiple features in one feature collection");
  filter_command.add_argument("-V", "--verbose")
      .help("print verbose debug log")
      .flag();

  argparse::ArgumentParser sql_command("sql", "",
                                       argparse::default_arguments::help);
  sql_command.add_argument("query").help("cql2 query string");
  sql_command.add_argument("-V", "--verbose")
      .help("print verbose debug log")
      .flag();

  argparse::ArgumentParser eval_command("evaluate", "",
                                        argparse::default_arguments::help);
  eval_command.add_argument("query").help("cql2 query string");
  eval_command.add_argument("--features")
      .help(
          "geojson file contains multiple features in one feature collection");
  eval_command.add_argument("--index")
      .help("index of feature in the geojson file to be evaluated")
      .scan<'i', int>();
  eval_command.add_argument("-O", "--output").help("the output dot file");
  eval_command.add_argument("-V", "--verbose")
      .help("print verbose debug log")
      .flag();

  program.add_subparser(parse_command);
  program.add_subparser(filter_command);
  program.add_subparser(sql_command);
  program.add_subparser(eval_command);

  if (argc == 1) {
    std::cout << program;
    return 0;
  }

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception& e) {
    LOG(ERROR) << e.what();
    return 1;
  }

  google::InitGoogleLogging(argv[0]);
  FLAGS_colorlogtostderr = true;
  google::InstallFailureSignalHandler();
  google::LogToStderr();

  if (program.is_subcommand_used("parse")) {
    if (parse_command.get<bool>("--verbose"))
      cql2cpp::AstNode::set_ostream(&std::cout);
    std::string dot;
    std::string error_msg;
    cql2cpp::Cql2Cpp cql2cpp;
    if (cql2cpp.ToDot(parse_command.get<std::string>("query"), &dot,
                      &error_msg)) {
      if (parse_command.is_used("--output")) {
        std::string dot_filename = parse_command.get<std::string>("--output");
        if (dot_filename.find(".dot") == std::string::npos)
          dot_filename += ".dot";

        std::ofstream fout(dot_filename);
        if (fout.is_open()) {
          fout << dot;
          fout.close();
          LOG(INFO) << "save dot file: " << dot_filename;
        } else {
          LOG(ERROR) << "Can not open file " << dot_filename;
        }
      } else {
        LOG(INFO) << dot;
      }
    } else {
      LOG(ERROR) << error_msg;
    }
  } else if (program.is_subcommand_used("filter")) {
    if (filter_command.get<bool>("--verbose"))
      cql2cpp::AstNode::set_ostream(&std::cout);
    std::string geojson_text;
    std::string features = filter_command.get<std::string>("--features");
    std::ifstream fin(features);

    if (not fin.good()) {
      LOG(ERROR) << features << " not exist";
      goto FAILED;
    }
    if (not fin.is_open()) {
      LOG(ERROR) << "can not open " << features;
      goto FAILED;
    }
    geojson_text.assign(std::istreambuf_iterator<char>(fin),
                        std::istreambuf_iterator<char>());
    fin.close();

    geos::io::GeoJSONFeatureCollection fc({});
    // read geojson features
    geos::io::GeoJSONReader reader;
    fc = reader.readFeatures(geojson_text);
    LOG(INFO) << "load " << fc.getFeatures().size() << " features from "
              << features;

    std::vector<cql2cpp::FeatureSourcePtr> fs_feature;
    std::map<cql2cpp::FeatureSourcePtr, const geos::io::GeoJSONFeature*>
        fs_to_geojson;
    for (const auto& feature : fc.getFeatures()) {
      cql2cpp::FeatureSourcePtr fp =
          std::make_shared<cql2cpp::FeatureSourceGeoJson>(feature);
      fs_feature.emplace_back(fp);
      fs_to_geojson[fp] = &feature;
    }

    std::string query = filter_command.get<std::string>("query");

    cql2cpp::Cql2Cpp cql2cpp;
    cql2cpp.set_feature_source(fs_feature);
    std::vector<cql2cpp::FeatureSourcePtr> result;
    if (cql2cpp.filter(query, &result)) {
      LOG(INFO) << result.size() << " features match the filter:";
      geos::io::GeoJSONWriter writer;
      for (const auto& feature : result)
        LOG(INFO) << writer.write(*fs_to_geojson[feature]);
    } else {
      LOG(ERROR) << "filter error: " << cql2cpp.error_msg();
    }
  } else if (program.is_subcommand_used("sql")) {
    if (sql_command.get<bool>("--verbose"))
      cql2cpp::AstNode::set_ostream(&std::cout);
    std::string query = sql_command.get<std::string>("query");
    std::string sql_where;
    std::string error_msg;
    if (cql2cpp::Cql2Cpp::ConvertToSQL(query, &sql_where, &error_msg)) {
      LOG(INFO) << sql_where;
    } else {
      LOG(ERROR) << error_msg;
      goto FAILED;
    }
  } else if (program.is_subcommand_used("evaluate")) {
    if (eval_command.get<bool>("--verbose")) {
      cql2cpp::AstNode::set_ostream(&std::cout);
    }
    std::string geojson_text;
    std::string features = eval_command.get<std::string>("--features");
    std::ifstream fin(features);
    if (not fin.good()) {
      LOG(ERROR) << features << " not exist";
      goto FAILED;
    }
    if (not fin.is_open()) {
      LOG(ERROR) << "can not open " << features;
      goto FAILED;
    }
    geojson_text.assign(std::istreambuf_iterator<char>(fin),
                        std::istreambuf_iterator<char>());
    fin.close();

    geos::io::GeoJSONFeatureCollection fc({});
    // read geojson features
    geos::io::GeoJSONReader reader;
    fc = reader.readFeatures(geojson_text);
    LOG(INFO) << "load " << fc.getFeatures().size() << " features from "
              << features;

    int index = eval_command.get<int>("index");
    if (index >= fc.getFeatures().size()) {
      LOG(ERROR) << "index(" << index << ") out of range [0.."
                 << fc.getFeatures().size() - 1 << "]";
      goto FAILED;
    }
    cql2cpp::FeatureSourceGeoJson fs(fc.getFeatures().at(index));
    std::string query = eval_command.get<std::string>("query");

    cql2cpp::Cql2Cpp cql2cpp;

    std::string dot;
    bool eval_result;
    std::string error_msg;
    if (cql2cpp.Evaluate(query, fs, &eval_result, &error_msg, &dot)) {
      if (eval_command.is_used("--output")) {
        std::string dot_filename = eval_command.get<std::string>("--output");
        if (dot_filename.find(".dot") == std::string::npos)
          dot_filename += ".dot";

        std::ofstream fout(dot_filename);
        if (fout.is_open()) {
          fout << dot;
          fout.close();
          LOG(INFO) << "dump dot file into " << dot_filename;
        } else {
          LOG(ERROR) << "Can not open file " << dot_filename;
        }
      } else {
        LOG(INFO) << dot;
      }
    } else {
      LOG(ERROR) << error_msg;
    }

  } else {
    LOG(ERROR) << "unknown sub-command";
  }

  gflags::ShutDownCommandLineFlags();
  google::ShutdownGoogleLogging();
  return 0;

FAILED:

  gflags::ShutDownCommandLineFlags();
  google::ShutdownGoogleLogging();
  return 1;
}
