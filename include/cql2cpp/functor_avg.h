/*
 * File Name: functor_avg.h
 *
 * Copyright (c) 2024-2025 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2025/01/03
 *
 */

#pragma once

#include "functor.h"

namespace cql2cpp {

class FunctorAvg : public Functor {
 public:
  std::string name() const override { return "avg"; }

  bool operator()(const std::vector<ValueT>& arguments, ValueT* result,
                  std::string* error_msg) const override {
    if (arguments.size() != 1) {
      *error_msg = "avg expect 1 argument with type array";
      return false;
    }
    if (not std::holds_alternative<ArrayType>(arguments.at(0))) {
      *error_msg = "avg expect 1 argument with type array but we get: " +
                   value_str(arguments.at(0));
      return false;
    }

    const ArrayType& array = std::get<ArrayType>(arguments.at(0));

    for (size_t i = 0; i < array.size(); i++)
      if (array.at(i).value.index() != array.at(0).value.index()) {
        *error_msg = "type mismatched between arguments of avg";
        return false;
      }

    if (std::holds_alternative<int64_t>(array.at(0).value))
      *result = avg<int64_t>(array);
    else if (std::holds_alternative<uint64_t>(array.at(0).value))
      *result = avg<uint64_t>(array);
    else if (std::holds_alternative<double>(array.at(0).value))
      *result = avg<double>(array);
    else {
      *error_msg = "can only calculate average for double and int values";
      return false;
    }

    return true;
  }

 private:
  template <typename T>
  static double avg(const ArrayType& array) {
    double sum = 0.0;
    for (size_t i = 0; i < array.size(); i++)
      sum += std::get<T>(array.at(i).value);
    return sum / array.size();
  }
};

}  // namespace cql2cpp

