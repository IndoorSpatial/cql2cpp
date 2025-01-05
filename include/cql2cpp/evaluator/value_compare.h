/*
 * File Name: value_compare.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/31
 *
 */

#pragma once

#include <cql2cpp/value.h>

namespace cql2cpp {

static const double kEpsilon = 1e-5;

template <typename T>
inline bool TypedEqual(const ValueT& a, const ValueT& b) {
  return std::get<T>(a) == std::get<T>(b);
}

inline bool isVariantEqual(const ValueT& a, const ValueT& b) {
  if (a.index() != b.index()) return false;

  if (std::holds_alternative<bool>(a)) return TypedEqual<bool>(a, b);

  if (std::holds_alternative<int64_t>(a)) return TypedEqual<int64_t>(a, b);

  if (std::holds_alternative<uint64_t>(a)) return TypedEqual<uint64_t>(a, b);

  if (std::holds_alternative<double>(a))
    return fabs(std::get<double>(a) - std::get<double>(b)) < kEpsilon;

  if (std::holds_alternative<std::string>(a))
    return TypedEqual<std::string>(a, b);

  return false;
}

}  // namespace cql2cpp
