/*
 * File Name: value.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/19
 *
 */

#pragma once

#include <geos/geom/Envelope.h>
#include <geos/geom/Geometry.h>
#include <geos/io/WKTWriter.h>

#include <cctype>
#include <set>
#include <sstream>
#include <string>
#include <variant>

namespace cql2cpp {

enum NullStruct { NullValue };

struct Element;

class ArrayElementComp {
 public:
  static constexpr double tolerance = 1e-9;

 public:
  bool operator()(const Element& a, const Element& b) const;

 private:
  template <typename T, typename U>
  bool less(const U& lhs, const U& rhs) const {
    return std::less()(std::get<T>(lhs), std::get<T>(rhs));
  }
};

// https://docs.ogc.org/is/21-065r2/21-065r2.html#_array_functions
// /req/array-functions/array-predicates Requirement 16 B
// Both array expressions SHALL be evaluated as sets.
// No inherent order SHALL be implied in an array of values.
using SetType = std::set<Element, ArrayElementComp>;

using ArrayType = std::vector<Element>;

using ValueT = std::variant<NullStruct, bool, int64_t, uint64_t, double,
                            std::string, ArrayType, const geos::geom::Geometry*,
                            const geos::geom::Envelope*>;

struct Element {
  ValueT value;
  Element(const ValueT& value) : value(value) {}
};

static std::string value_str(ValueT value, bool with_type = false) {
  if (std::holds_alternative<NullStruct>(value)) return "null";

  if (std::holds_alternative<bool>(value))
    return (std::get<bool>(value) ? "T" : "F") +
           std::string(with_type ? " bool" : "");

  if (std::holds_alternative<int64_t>(value))
    return std::to_string(std::get<int64_t>(value)) +
           std::string(with_type ? " int64_t" : "");

  if (std::holds_alternative<uint64_t>(value))
    return std::to_string(std::get<uint64_t>(value)) +
           std::string(with_type ? " uint64_t" : "");

  if (std::holds_alternative<double>(value))
    return std::to_string(std::get<double>(value)) +
           std::string(with_type ? " double" : "");

  if (std::holds_alternative<std::string>(value))
    return std::get<std::string>(value) +
           std::string(with_type ? " string" : "");

  if (std::holds_alternative<ArrayType>(value)) {
    std::stringstream ss;
    ss << "[";
    for (const auto& element : std::get<ArrayType>(value))
      ss << value_str(element.value) << ",";
    ss.seekp(-1, std::ios_base::end);
    ss << "]";
    return ss.str() + std::string(with_type ? " array" : "");
  }

  if (std::holds_alternative<const geos::geom::Geometry*>(value)) {
    geos::io::WKTWriter writer;
    return writer.write(std::get<const geos::geom::Geometry*>(value));
  }

  if (std::holds_alternative<const geos::geom::Envelope*>(value)) {
    return std::get<const geos::geom::Envelope*>(value)->toString();
  }

  return "unknown type";
}

static std::string value_str(ValueT origin_value, ValueT value,
                             bool with_type = false) {
  auto s = value_str(value, with_type);
  if (s != "null")
    return s;
  else
    return value_str(origin_value, with_type);
}

}  // namespace cql2cpp
