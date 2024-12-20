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

#include <iostream>
#include <memory>
#include <vector>

#include "node_type.h"
#include "operator.h"
#include "value.h"
#include "id_generator.h"

namespace cql2cpp {

class AstNode;
using AstNodePtr = std::shared_ptr<AstNode>;

// Abstract Syntax Tree
class AstNode {
 private:
  std::string id_;
  NodeType type_;
  Operator op_;
  std::vector<AstNode*> children_;
  ValueT value_;

 public:
  AstNode(NodeType type, Operator op, const std::vector<AstNode*> children)
      : type_(type), op_(op), children_(children) {
    std::cout << "Construct AstNode " << std::endl;
    id_ = idg.Gen();
  }

  AstNode(const ValueT& value) : type_(Literal), op_(NullOp), value_(value) {
    std::cout << "Construct AstNode literal" << std::endl;
    id_ = idg.Gen();
  }

  const std::string& id() const { return id_; }

  NodeType type() const { return type_; }

  Operator op() const { return op_; }

  const std::vector<AstNode*>& children() const { return children_; }

  ValueT value() const { return value_; }
};

}  // namespace cql2cpp
