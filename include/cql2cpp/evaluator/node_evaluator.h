/*
 * File Name: node_evaluator.h
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
#include "../value.h"

namespace cql2cpp {

class NodeEvaluator {
 public:
  virtual NodeType type() const = 0;
  virtual bool Evaluate(const AstNode& node,
                        const std::vector<ValueT> sub_value,
                        const DataSource& ds, ValueT* result) const = 0;
  virtual std::string error_msg() const = 0;
};
}  // namespace cql2cpp

