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

  ValueT get_property(const geos_nlohmann::json& json) const {
    switch (json.type()) {
      case geos_nlohmann::json::value_t::null:
        return NullValue;
      case geos_nlohmann::json::value_t::object:
        return NullValue;
      case geos_nlohmann::json::value_t::array: {
        ArrayType array;
        for (const auto& child : json) {
          ValueT child_value = get_property(child);
          if (not std::holds_alternative<NullStruct>(child_value))
            array.insert(child_value);
        }
        return array;
      }
      case geos_nlohmann::json::value_t::string:
        return json.get<std::string>();
      case geos_nlohmann::json::value_t::boolean:
        return json.get<bool>();
      case geos_nlohmann::json::value_t::number_integer:
        return json.get<int64_t>();
      case geos_nlohmann::json::value_t::number_unsigned:
        return json.get<uint64_t>();
      case geos_nlohmann::json::value_t::number_float:
        return json.get<double>();
      case geos_nlohmann::json::value_t::binary:
        return json.get<std::string>();
      case geos_nlohmann::json::value_t::discarded:
        return NullValue;
      default:
        return NullValue;
    }
  }

  ValueT get_property(const std::string& property_path) const override {
    const geos_nlohmann::json* value =
        JsonHelper::get_property(property_path, json_);
    if (value == nullptr)
      return NullValue;
    else
      return get_property(*value);
  }
};

}  // namespace cql2cpp

