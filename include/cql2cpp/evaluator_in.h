/*
 * File Name: evaluator_in.h
 *
 * Copyright (c) 2024-2025 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2025/01/02
 *
 */

#pragma once

#include "evaluator_ast_node.h"
#include "value_compare.h"

namespace cql2cpp {

class EvaluatorIn : public EvaluatorAstNode {
 private:
  std::map<NodeType, std::map<Operator, NodeEval>> evaluators_;

 public:
  EvaluatorIn() {
    evaluators_[InList][NullOp] = [](auto n, auto vs, auto fs, auto value,
                                     auto errmsg) -> bool {
      // InList doesn't have value
      // the parent(IsInListPred) will access the value of their grand
      // children directly
      // WHICH IS NOT GOOD, WE SHOULD USE ARRAY
      *value = NullValue;
      return true;
    };
    evaluators_[IsInListPred][In] = [](auto n, auto vs, auto fs, auto value,
                                       auto errmsg) -> bool {
      if (vs.size() != 2) {
        *errmsg =
            "(NOT)IN needs two values but we have " + std::to_string(vs.size());
        return false;
      }
      if (not std::holds_alternative<NullStruct>(vs.at(1))) {
        *errmsg = "right hand side value of (NOT)IN should be null";
        return false;
      }
      if (n->children().size() != 2) {
        *errmsg =
            "(NOT)IN needs two children for scalar expression and "
            "list "
            "but we have " +
            std::to_string(n->children().size());
        return false;
      }
      if (std::holds_alternative<NullStruct>(vs.at(0))) {
        *value = NullValue;
        return true;
      }
      if (not std::holds_alternative<bool>(vs.at(0)) and
          not std::holds_alternative<int64_t>(vs.at(0)) and
          not std::holds_alternative<uint64_t>(vs.at(0)) and
          not std::holds_alternative<double>(vs.at(0)) and
          not std::holds_alternative<std::string>(vs.at(0))) {
        *errmsg = "left hand side is not scalar type";
        return false;
      }
      AstNodePtr in_list = n->children()[1];
      for (const AstNodePtr& grand_child : in_list->children()) {
        if (isVariantEqual(vs.at(0), grand_child->value())) {
          *value = true;
          return true;
        }
      }
      *value = false;
      return true;
    };
    evaluators_[IsInListPred][NotIn] = [this](auto n, auto vs, auto fs,
                                              auto value, auto errmsg) -> bool {
      const NodeEval& equal_lambda = evaluators_.at(IsInListPred).at(In);
      bool ret = equal_lambda.operator()(n, vs, fs, value, errmsg);
      if (ret) *value = not std::get<bool>(*value);
      return ret;
    };
  }
  const std::map<NodeType, std::map<Operator, NodeEval>>& GetEvaluators()
      const override {
    return evaluators_;
  }
};
}  // namespace cql2cpp

