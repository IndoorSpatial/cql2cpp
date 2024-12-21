/*
 * File Name: value_type.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/19
 *
 */

#pragma once

#include <cctype>
#include <string>
#include <variant>

namespace cql2cpp {

enum ValueType {
  Bool,
  Integer,
  Float,
  String,
  Array,
  Geometry,
  BBox,
  Timestamp,
  TimeInterval,
};

// void* is just a placeholder
typedef std::variant<void*, bool, int64_t, double, std::string> ValueT;

static std::string value_str(ValueT value, bool with_type = false) {
  if (std::holds_alternative<void*>(value)) return "?";

  if (std::holds_alternative<bool>(value))
    return std::get<bool>(value) ? "T"
                                 : "F" + std::string(with_type ? " bool" : "");

  if (std::holds_alternative<int64_t>(value))
    return std::to_string(std::get<int64_t>(value)) +
           std::string(with_type ? " int64_t" : "");

  if (std::holds_alternative<double>(value))
    return std::to_string(std::get<double>(value)) +
           std::string(with_type ? " double" : "");

  if (std::holds_alternative<std::string>(value))
    return std::get<std::string>(value) +
           std::string(with_type ? " string" : "");

  return "unknown type";
}

}  // namespace cql2cpp
