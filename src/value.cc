/*
 * File Name: value_type.cc
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/31
 *
 */

#include <cql2cpp/value.h>

namespace cql2cpp {

bool ArrayElementComp::operator()(const ArrayElement& lhs,
                                  const ArrayElement& rhs) const {
  const auto& a = lhs.value;
  const auto& b = rhs.value;
  if (a.index() != b.index()) return std::less()(a.index(), b.index());

  if (std::holds_alternative<NullStruct>(a)) return less<NullStruct>(a, b);
  if (std::holds_alternative<bool>(a)) return less<bool>(a, b);
  if (std::holds_alternative<int64_t>(a)) return less<int64_t>(a, b);
  if (std::holds_alternative<uint64_t>(a)) return less<uint64_t>(a, b);
  if (std::holds_alternative<std::string>(a)) return less<std::string>(a, b);

  if (std::holds_alternative<double>(a)) {
    double d_a = std::get<double>(a);
    double d_b = std::get<double>(b);
    if (std::fabs(d_a - d_b) < tolerance) return false;
    return std::less()(d_a, d_b);
  }

  return true;
}

}  // namespace cql2cpp
