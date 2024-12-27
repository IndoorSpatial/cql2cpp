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
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Point.h>
#include <geos/io/GeoJSON.h>
#include <geos/io/GeoJSONReader.h>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include <fstream>

DEFINE_string(cql2_query, "", "cql2 query string");
DEFINE_string(geojson, "", "data set to be queried");
DEFINE_string(dot, "", "generate dot file");
DEFINE_bool(verbose, false, "Enable verbose output");

int main(int argc, char** argv) {
  gflags::SetUsageMessage(
      "cql2 usage: cql2 [-cql2_query=\"city='Toronto'\"] "
      "-geojson=\"path/to/geo.json\"");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();
  google::LogToStderr();

  // print flags
  LOG(INFO) << "==== flags ====";
  LOG(INFO) << "cql2_query: " << FLAGS_cql2_query;
  LOG(INFO) << "geojson: " << FLAGS_geojson;
  LOG(INFO) << "dot: " << FLAGS_dot;

  if (FLAGS_verbose) cql2cpp::AstNode::set_ostream(&std::cout);

  if (FLAGS_cql2_query.empty()) {
    LOG(ERROR) << "you should provide cql2_query";
    return 0;
  }

  std::string error_msg;

  // just parse the query and dump a dot file
  if (FLAGS_geojson.empty()) {
    std::string dot;
    cql2cpp::Cql2Cpp<geos::io::GeoJSONFeature> cql2cpp;
    if (cql2cpp.ToDot(FLAGS_cql2_query, &dot, &error_msg)) {
      LOG(INFO) << error_msg;
      if (not FLAGS_dot.empty()) {
        std::string dot_filename = FLAGS_dot;
        if (dot_filename.find(".dot") == std::string::npos)
          dot_filename += ".dot";

        std::ofstream fout(dot_filename);
        if (fout.is_open()) {
          fout << dot;
          fout.close();
        } else {
          LOG(ERROR) << "Can not open file " << dot_filename;
        }
      }
    } else {
      LOG(ERROR) << error_msg;
    }
  } else {
    // read geojson text
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
    if (cql2cpp.filter(FLAGS_cql2_query, &result)) {
      LOG(INFO) << "get feature count: " << result.size();
      for (const auto& feature : result) {
        LOG(INFO) << "get feature geometry: "
                  << feature->getGeometry()->toText();
      }
    } else {
      LOG(ERROR) << "filter error: " << cql2cpp.error_msg();
    }
  }
FAILED:

  gflags::ShutDownCommandLineFlags();
  return 0;
}
