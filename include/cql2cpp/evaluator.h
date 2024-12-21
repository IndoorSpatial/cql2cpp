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
#include "data_source/data_source.h"

namespace cql2cpp {

using NodeEval = std::function<ValueT(
    const AstNode*, const std::vector<ValueT>&, const DataSource&)>;

class TreeEvaluator {
 private:
  std::map<Operator, NodeEval> type_evaluator_;
  std::string error_msg_;

 public:
  void RegisterNodeEvaluator(Operator type, NodeEval evaluator) {
    type_evaluator_[type] = evaluator;
  }

  void RegisterNodeEvaluator(const std::map<Operator, NodeEval> evaluators) {
    type_evaluator_.insert(evaluators.begin(), evaluators.end());
  }

  bool Evaluate(const AstNode* root, const DataSource& ds, ValueT* result) {
    if (type_evaluator_.find(root->op()) == type_evaluator_.end()) {
      error_msg_ = "can not find evaluator for operator " +
                   OpName.at(root->op()) + " in node type " +
                   TypeName.at(root->type());
      return false;
    }

    std::vector<ValueT> child_values;
    for (AstNode* child : root->children()) {
      ValueT value;
      if (Evaluate(child, ds, &value))
        child_values.emplace_back(value);
      else
        return false;
    }

    *result = type_evaluator_[root->op()].operator()(root, child_values, ds);
    root->set_value(*result);
    std::cout << "Evaluate Node " << root->id()
              << " value: " << value_str(*result, true) << std::endl;

    return true;
  }

  const std::string& error_msg() { return error_msg_; }
};

}  // namespace cql2cpp
