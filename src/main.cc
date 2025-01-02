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
#include <cql2cpp/feature_source_geojson.h>
#include <cql2cpp/global_yylex.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Point.h>
#include <geos/io/GeoJSON.h>
#include <geos/io/GeoJSONReader.h>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include <fstream>

#include "cql2cpp/feature_source.h"

DEFINE_string(query, "", "cql2 query string");
DEFINE_string(geojson, "",
              "geojson data set with multiple features in one feature "
              "collection to be queried");
DEFINE_uint32(index, 0, "index of the feature in geojson");
DEFINE_string(dot, "", "generate dot file");
DEFINE_bool(verbose, false, "Enable verbose output");

int main(int argc, char** argv) {
  gflags::SetUsageMessage(
      "Usage: cql2 -query=\"city='Toronto'\" "
      "-geojson=\"path/to/feature_collection.geojson\" -index=0");
  gflags::SetVersionString("0.1.0");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();
  google::LogToStderr();

  if (FLAGS_query.empty()) {
    gflags::ShowUsageWithFlagsRestrict(argv[0], "main.cc");
    LOG(ERROR) << "you should provide query";
    return -1;
  }

  // print flags
  LOG(INFO) << "==== flags ====";
  LOG(INFO) << "query: " << FLAGS_query;
  LOG(INFO) << "geojson: " << FLAGS_geojson;
  LOG(INFO) << "dot: " << FLAGS_dot;

  if (FLAGS_verbose) cql2cpp::AstNode::set_ostream(&std::cout);

  std::string error_msg;

  // case 1:
  // we have only query without any feature
  // just parse the query and may dump a dot file
  if (FLAGS_geojson.empty()) {
    std::string dot;
    cql2cpp::Cql2Cpp<geos::io::GeoJSONFeature> cql2cpp;
    if (cql2cpp.ToDot(FLAGS_query, &dot, &error_msg)) {
      LOG(INFO) << error_msg;
      if (not FLAGS_dot.empty()) {
        std::string dot_filename = FLAGS_dot;
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
      }
    } else {
      LOG(ERROR) << error_msg;
    }
  } else {
    // case 2:
    // we have query and a feature collection
    // filter matched features from the feature collection
    std::string geojson_text;
    std::ifstream fin(FLAGS_geojson);
    if (fin.good()) {
      if (fin.is_open()) {
        geojson_text.assign(std::istreambuf_iterator<char>(fin),
                            std::istreambuf_iterator<char>());
        fin.close();
      } else {
        LOG(ERROR) << "can not open " << FLAGS_geojson;
        goto FAILED;
      }
    } else {
      LOG(ERROR) << FLAGS_geojson << " not exist";
      goto FAILED;
    }

    geos::io::GeoJSONFeatureCollection fc({});
    // read geojson features
    geos::io::GeoJSONReader reader;
    fc = reader.readFeatures(geojson_text);
    LOG(INFO) << "load " << fc.getFeatures().size() << " features from "
              << FLAGS_geojson;

    std::map<cql2cpp::FeatureSourcePtr, const geos::io::GeoJSONFeature*>
        fs_feature;
    for (const auto& feature : fc.getFeatures()) {
      cql2cpp::FeatureSourcePtr fp =
          std::make_shared<cql2cpp::FeatureSourceGeoJson>(feature);
      fs_feature[fp] = &feature;
    }

    cql2cpp::Cql2Cpp<const geos::io::GeoJSONFeature*> cql2cpp;
    cql2cpp.set_feature_source(fs_feature);
    std::vector<const geos::io::GeoJSONFeature*> result;
    if (cql2cpp.filter(FLAGS_query, &result)) {
      LOG(INFO) << "get feature count: " << result.size();
      for (const auto& feature : result) {
        LOG(INFO) << "get feature geometry: "
                  << feature->getGeometry()->toText();
      }
    } else {
      LOG(ERROR) << "filter error: " << cql2cpp.error_msg();
    }

    if (FLAGS_index >= fc.getFeatures().size()) {
      LOG(ERROR) << "index(" << FLAGS_index << ") out of range [0.."
                 << fc.getFeatures().size() - 1 << "]";
      goto FAILED;
    }
    cql2cpp::FeatureSourceGeoJson fs(fc.getFeatures().at(FLAGS_index));

    std::string dot;
    bool eval_result;
    if (cql2cpp::Cql2Cpp<const geos::io::GeoJSONFeature*>::Evaluate(
            FLAGS_query, fs, &eval_result, &error_msg, &dot)) {
      if (not FLAGS_dot.empty()) {
        std::string dot_filename = FLAGS_dot;
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
      }
    } else {
      LOG(ERROR) << error_msg;
    }
  }
FAILED:

  gflags::ShutDownCommandLineFlags();
  return 0;
}
