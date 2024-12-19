/*
 * File Name: tree_evaluator.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/19
 *
 */

#pragma once

#include "../ast_node.h"
#include "../data_source/data_source.h"
#include "node_evaluator.h"

namespace cql2cpp {

class TreeEvaluator {
 public:
  void RegisterNodeEvaluator(const NodeEvaluator& node_eval) { }
  bool Evaluate(const AstNode& root, const DataSource& ds, bool* result) { return false; }
  std::string error_msg() { return ""; }
};

}  // namespace cql2cpp
