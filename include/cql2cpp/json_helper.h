/*
 * File Name: json_helper.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/19
 *
 */

#pragma once
#include <geos/vend/include_nlohmann_json.hpp>
#include <sstream>

namespace cql2cpp {

class JsonHelper {
 public:
  static const geos_nlohmann::json* get_property(const std::string& property_path,
                                            const geos_nlohmann::json& json) {
    std::vector<std::string> segments = split(property_path, '.');
    const geos_nlohmann::json* current = &json;
    for (size_t i = 0; i < segments.size() - 1; i++) {
      const std::string& field_name = segments[i];
      if (current->contains(field_name) && current->at(field_name).is_object())
        current = &current->at(field_name);
      else
        return nullptr;
    }

    if (current->contains(segments.back()))
      return &current->at(segments.back());
    else
      return nullptr;
  }

 private:
  static std::vector<std::string> split(const std::string& str,
                                        char delimiter) {
    std::vector<std::string> components;
    std::istringstream iss(str);
    std::string component;

    while (std::getline(iss, component, delimiter)) {
      components.push_back(component);
    }
    return components;
  }
};

}  // namespace cql2cpp
