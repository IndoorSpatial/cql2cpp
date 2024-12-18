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

#include <variant>
#include <string>
#include <cctype>

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


typedef std::variant<bool, size_t, double, std::string> ValueT;

}
