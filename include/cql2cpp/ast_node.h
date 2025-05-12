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

#include <glog/logging.h>
#include <unistd.h>

#include <memory>
#include <queue>
#include <vector>

#include "id_generator.h"
#include "node_type.h"
#include "operator.h"
#include "value.h"

namespace cql2cpp {

class AstNode;
using AstNodePtr = std::shared_ptr<AstNode>;

// Abstract Syntax Tree
class AstNode : public std::enable_shared_from_this<AstNode> {
 private:
  std::string id_;
  NodeType type_;
  Operator op_;
  std::vector<AstNodePtr> children_;
  ValueT origin_value_;
  mutable ValueT value_;
  static std::ostream* ous_;

 public:
  static void set_ostream(std::ostream* ous) { ous_ = ous; }

  AstNode(NodeType type, Operator op, const std::vector<AstNodePtr> children)
      : type_(type), op_(op), children_(children) {
    id_ = idg.Gen();
#ifdef DEBUG
    LOG(INFO) << "AstNode " << ToString() << std::endl;
#endif
  }

  AstNode(NodeType type, const ValueT& value)
      : type_(type), op_(NullOp), origin_value_(value), value_(NullValue) {
    id_ = idg.Gen();
#ifdef DEBUG
    LOG(INFO) << "AstNode " << ToString() << std::endl;
#endif
  }

  AstNode(const ValueT& value)
      : type_(Literal), op_(NullOp), origin_value_(value), value_(NullValue) {
    id_ = idg.Gen();
#ifdef DEBUG
    LOG(INFO) << "AstNode " << ToString() << std::endl;
#endif
  }

  const std::string& id() const { return id_; }

  NodeType type() const { return type_; }

  Operator op() const { return op_; }

  const std::vector<AstNodePtr>& children() const { return children_; }
  void append(AstNodePtr node) { children_.emplace_back(node); }

  const ValueT& origin_value() const { return origin_value_; }
  ValueT value() const { return value_; }
  void set_value(const ValueT& value) const { value_ = value; }

  std::string ToString() {
    if (op_ == NullOp)
      return id_ + " " + TypeName.at(type()) + " " +
             value_str(origin_value_, value_, true);
    else
      return id_ + " " + TypeName.at(type()) + " " + OpName.at(op());
  }

  class Iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = AstNodePtr;
    using difference_type = std::ptrdiff_t;
    using pointer = AstNodePtr*;
    using reference = AstNodePtr&;

    Iterator() = default;
    explicit Iterator(AstNodePtr root) {
      if (root) {
        queue_.push(root);
        current_ = root;
      }
    }

    Iterator& operator++() {
      if (queue_.empty()) {
        current_ = nullptr;
        return *this;
      }

      queue_.pop();

      if (current_) {
        const auto& children = current_->children();
        for (const auto& child : children) {
          if (child) queue_.push(child);
        }
      }

      if (queue_.empty()) {
        current_ = nullptr;
      } else {
        current_ = queue_.front();
      }

      return *this;
    }

    Iterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const Iterator& other) const {
      return current_ == other.current_;
    }

    bool operator!=(const Iterator& other) const { return !(*this == other); }

    AstNodePtr operator*() const { return current_; }

   private:
    AstNodePtr current_ = nullptr;
    std::queue<AstNodePtr> queue_;
  };

  Iterator begin() const {
    return Iterator(std::const_pointer_cast<AstNode>(shared_from_this()));
  }

  Iterator end() const { return Iterator(nullptr); }
};

}  // namespace cql2cpp
