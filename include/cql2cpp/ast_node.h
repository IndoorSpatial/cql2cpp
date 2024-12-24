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

#include <memory>
#include <vector>

#include <glog/logging.h>

#include "id_generator.h"
#include "node_type.h"
#include "operator.h"
#include "value.h"

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
  mutable ValueT value_;
  static std::ostream* ous_;

 public:
  static void set_ostream(std::ostream* ous) { ous_ = ous; }

  AstNode(NodeType type, Operator op, const std::vector<AstNode*> children)
      : type_(type), op_(op), children_(children) {
    id_ = idg.Gen();
    *ous_ << "AstNode " << ToString() << std::endl;
  }

  AstNode(NodeType type, const ValueT& value)
      : type_(type), op_(NullOp), value_(value) {
    id_ = idg.Gen();
    *ous_ << "AstNode " << ToString() << std::endl;
  }

  AstNode(const ValueT& value) : type_(Literal), op_(NullOp), value_(value) {
    id_ = idg.Gen();
    *ous_ << "AstNode " << ToString() << std::endl;
  }


  const std::string& id() const { return id_; }

  NodeType type() const { return type_; }

  Operator op() const { return op_; }

  const std::vector<AstNode*>& children() const { return children_; }

  ValueT value() const { return value_; }
  void set_value(const ValueT& value) const { value_ = value; }

  std::string ToString() {
    if (op_ == NullOp)
      return id_ + " " + TypeName.at(type()) + " " + value_str(value_, true);
    else
      return id_ + " " + TypeName.at(type()) + " " + OpName.at(op());
  }
};

}  // namespace cql2cpp
