/*
 * File Name: id_generator.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/20
 *
 */

#pragma once

#include <string>

namespace cql2cpp {

class IdGen {
 private:
  std::string prefix_;
  size_t index_;

 public:
  IdGen(const std::string& prefix) : prefix_(prefix), index_(0) {}

  std::string Gen() { return prefix_ + std::to_string(index_++); }

  void reset() { index_ = 0; }
};

extern IdGen idg;

}  // namespace cql2cpp
