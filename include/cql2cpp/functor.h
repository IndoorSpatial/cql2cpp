/*
 * File Name: functor.h
 *
 * Copyright (c) 2024-2025 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2025/01/02
 *
 */

#pragma once

#include <string>
#include "value.h"

namespace cql2cpp {

class Functor {
 public:
  virtual std::string name() const = 0;
  virtual bool operator()(const std::vector<ValueT>&, ValueT*,
                          std::string* error_msg) const = 0;
};
using FunctorPtr = std::shared_ptr<Functor>;

}  // namespace cql2cpp

