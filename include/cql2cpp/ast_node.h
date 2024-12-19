/*
 * File Name: ast_node.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/19
 *
 */

#pragma once

#include <vector>

#include "node_type.h"
#include "operator.h"

namespace cql2cpp {

class AstNode {  // Abstract Syntax Tree
 private:
  NodeType ntype_;
  Operator op_;
  std::vector<AstNode*> children_;

 public:
  AstNode(NodeType type, Operator op) : ntype_(type), op_(op) {}
  NodeType type() { return ntype_; }
  Operator op() { return op_; }
};

}  // namespace cql2cpp
