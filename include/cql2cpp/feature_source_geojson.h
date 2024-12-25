/*
 * File Name: feature_source_geojson.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/22
 *
 */

#pragma once

#include <geos/io/GeoJSON.h>
#include <geos/io/GeoJSONWriter.h>

#include "feature_source_json.h"

namespace cql2cpp {

class FeatureSourceGeoJson : public FeatureSourceJson {
 private:
  const geos::io::GeoJSONFeature& feature_;

 public:
  FeatureSourceGeoJson(const geos::io::GeoJSONFeature& feature)
      : FeatureSourceJson(geos_nlohmann::json::object()), feature_(feature) {
    geos::io::GeoJSONWriter writer;
    std::string serialized = writer.write(feature_);
    geos_nlohmann::json j = geos_nlohmann::json::parse(serialized);
    if (j.contains("properties"))
      json_ = j.at("properties");
    else
      json_ = geos_nlohmann::json::object();
  }
  ValueT get_property(const std::string& property_path) const override {
    // Annex A: Abstract Test Suite (Normative)
    // "the queryable for the feature geometry is geom"
    if (property_path == "geom")
      return feature_.getGeometry();
    else
      return FeatureSourceJson::get_property(property_path);
  }
};

}  // namespace cql2cpp

