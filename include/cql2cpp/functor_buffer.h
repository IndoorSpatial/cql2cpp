/*
 * File Name: functor_buffer.h
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

class FunctorBuffer : public Functor {
 public:
  std::string name() const override { return "Buffer"; }

  bool operator()(const std::vector<ValueT>& arguments, ValueT* result,
                  std::string* error_msg) const override {
    *error_msg = "Unsupported function Buffer";
    return false;
  }
};

}  // namespace cql2cpp

