/*
 * File Name: feature_source_json.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/19
 *
 */

#pragma once

#include <geos/vend/include_nlohmann_json.hpp>

#include "feature_source.h"
#include "json_helper.h"

namespace cql2cpp {

class FeatureSourceJson : public FeatureSource {
 protected:
  geos_nlohmann::json json_;

 public:
  FeatureSourceJson(const geos_nlohmann::json& json) : json_(json) {}

  ValueT get_property(const std::string& property_path) const override {
    const geos_nlohmann::json* value =
        JsonHelper::get_property(property_path, json_);
    if (value == nullptr) return nullptr;

    switch (value->type()) {
      case geos_nlohmann::json::value_t::null:
        return nullptr;
      case geos_nlohmann::json::value_t::object:
        return nullptr;
      case geos_nlohmann::json::value_t::string:
        return value->get<std::string>();
      case geos_nlohmann::json::value_t::boolean:
        return value->get<bool>();
      case geos_nlohmann::json::value_t::number_integer:
        return value->get<int64_t>();
      case geos_nlohmann::json::value_t::number_unsigned:
        return value->get<uint64_t>();
      case geos_nlohmann::json::value_t::number_float:
        return value->get<double>();
      case geos_nlohmann::json::value_t::binary:
        return value->get<std::string>();
      case geos_nlohmann::json::value_t::discarded:
        return nullptr;
      default:
        return nullptr;
    }
  }
};

}  // namespace cql2cpp

