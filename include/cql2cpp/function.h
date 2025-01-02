/*
 * File Name: function.h
 *
 * Copyright (c) 2024-2025 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2025/01/02
 *
 */

#pragma once

#include <functional>
#include <map>
#include <string>
#include <variant>

#include "value.h"

namespace cql2cpp {

using FunctionType = std::function<bool(const std::vector<ValueT>&, ValueT*,
                                        std::string* error_msg)>;

template <typename T>
double avg(const ArrayType& array) {
  double sum = 0.0;
  for (size_t i = 0; i < array.size(); i++) sum += std::get<T>(array.at(i).value);
  return sum / array.size();
}

const std::map<std::string, FunctionType> functions = {
    {"avg",
     [](auto arguments, auto result, auto errmsg) -> bool {
       if (arguments.size() != 1) {
         *errmsg = "avg expect 1 argument with type array";
         return false;
       }
       if (not std::holds_alternative<ArrayType>(arguments.at(0))) {
         *errmsg = "avg expect 1 argument with type array";
         return false;
       }

       const ArrayType& array = std::get<ArrayType>(arguments.at(0));

       for (size_t i = 0; i < array.size(); i++)
         if (array.at(i).value.index() != array.at(0).value.index()) {
           *errmsg = "type mismatched between arguments of avg";
           return false;
         }

       if (std::holds_alternative<int64_t>(array.at(0).value))
         *result = avg<int64_t>(array);
       else if (std::holds_alternative<uint64_t>(array.at(0).value))
         *result = avg<uint64_t>(array);
       else if (std::holds_alternative<double>(array.at(0).value))
         *result = avg<double>(array);
       else {
         *errmsg = "can only calculate average for double and int values";
         return false;
       }

       return true;
     }},

    {"Buffer",
     [](auto array, auto result, auto errmsg) -> bool {
       *errmsg = "unsupported function Buffer";
       return true;
     }},
    {"related_bins",
     [](auto array, auto result, auto errmsg) -> bool {
       // ha
       return true;
     }},
};
}  // namespace cql2cpp

