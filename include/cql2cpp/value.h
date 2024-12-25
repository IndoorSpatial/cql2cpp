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

#include <geos/geom/Envelope.h>
#include <geos/geom/Geometry.h>
#include <geos/io/WKTWriter.h>

#include <cctype>
#include <string>
#include <variant>

namespace cql2cpp {

enum NullStruct {
  NullValue
};

typedef std::variant<NullStruct, bool, int64_t, uint64_t, double, std::string,
                     const geos::geom::Geometry*, const geos::geom::Envelope*>
    ValueT;

static std::string value_str(ValueT value, bool with_type = false) {
  if (std::holds_alternative<NullStruct>(value)) return "?";

  if (std::holds_alternative<bool>(value))
    return std::get<bool>(value) ? "T"
                                 : "F" + std::string(with_type ? " bool" : "");

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

  if (std::holds_alternative<const geos::geom::Geometry*>(value)) {
    geos::io::WKTWriter writer;
    return writer.write(std::get<const geos::geom::Geometry*>(value));
  }

  return "unknown type";
}

}  // namespace cql2cpp
