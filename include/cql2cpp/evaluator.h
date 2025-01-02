/*
 * File Name: evaluator.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/19
 *
 */

#pragma once

#include <functional>
#include <map>

#include "ast_node.h"
#include "feature_source.h"

namespace cql2cpp {

using NodeEval =
    std::function<bool(const AstNode*, const std::vector<ValueT>&,
                       const FeatureSource*, ValueT*, std::string* error_msg)>;

class TreeEvaluator {
 private:
  std::map<NodeType, std::map<Operator, NodeEval>> type_evaluator_;
  std::string error_msg_;

 public:
  void RegisterNodeEvaluator(Operator op, NodeType type, NodeEval evaluator) {
    type_evaluator_[type][op] = evaluator;
  }

  void RegisterNodeEvaluator(
      const std::map<NodeType, std::map<Operator, NodeEval>> evaluators) {
    type_evaluator_.insert(evaluators.begin(), evaluators.end());
  }

  bool Evaluate(const AstNode* root, const FeatureSource* fs, ValueT* result) {
    if (type_evaluator_.find(root->type()) == type_evaluator_.end() ||
        type_evaluator_[root->type()].find(root->op()) ==
            type_evaluator_[root->type()].end()) {
      error_msg_ = "can not find evaluator for operator \"" +
                   OpName.at(root->op()) + "\" in node type " +
                   TypeName.at(root->type());
      return false;
    }

    std::vector<ValueT> child_values;
    for (AstNode* child : root->children()) {
      ValueT value;
      if (Evaluate(child, fs, &value))
        child_values.emplace_back(value);
      else
        return false;
    }

    bool ret = type_evaluator_[root->type()][root->op()].operator()(
        root, child_values, fs, result, &error_msg_);
    if (ret) {
      root->set_value(*result);
      LOG(INFO) << "Evaluate Node " << root->id() << " "
                << TypeName.at(root->type())
                << " value: " << value_str(*result, true);
    } else {
      LOG(ERROR) << "Evaluate Node " << root->id() << " error: " << error_msg_;
    }

    return ret;
  }

  const std::string& error_msg() { return error_msg_; }
};

}  // namespace cql2cpp
