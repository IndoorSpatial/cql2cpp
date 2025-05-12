/*
 * File Name: bool.h
 *
 * Copyright (c) 2024-2025 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2025/01/02
 *
 */

#pragma once

#include "ast_node.h"

namespace cql2cpp {

class EvaluatorBool : public EvaluatorAstNode {
 private:
  std::map<NodeType, std::map<Operator, NodeEval>> evaluators_;

  template <typename ValueType>
  static bool CheckValueNumberType(const std::string& op, size_t num,
                                   const std::vector<ValueT>& vs,
                                   std::string* errmsg) {
    if (vs.size() != num) {
      *errmsg =
          op + " needs two values but we have " + std::to_string(vs.size());
      return false;
    }
    for (size_t i = 0; i < num; i++)
      if (not std::holds_alternative<ValueType>(vs.at(i))) {
        *errmsg = "value " + std::to_string(i) + " of " + op + " is incorrect";
        return false;
      }
    return true;
  }

 public:
  EvaluatorBool() {
    evaluators_[BoolExpr][And] = [](auto n, auto vs, auto fs, auto value,
                                          auto errmsg) -> bool {
      if (not CheckValueNumberType<bool>("AND", 2, vs, errmsg)) return false;
      *value = std::get<bool>(vs.at(0)) and std::get<bool>(vs.at(1));
      return true;
    };
    evaluators_[BoolExpr][Or] = [](auto n, auto vs, auto fs, auto value,
                                         auto errmsg) -> bool {
      if (not CheckValueNumberType<bool>("OR", 2, vs, errmsg)) return false;
      *value = std::get<bool>(vs.at(0)) or std::get<bool>(vs.at(1));
      return true;
    };
    evaluators_[BoolExpr][Not] = [](auto n, auto vs, auto fs, auto value,
                                          auto errmsg) -> bool {
      if (not CheckValueNumberType<bool>("NOT", 1, vs, errmsg)) return false;
      *value = not std::get<bool>(vs.at(0));
      return true;
    };
  }
  const std::map<NodeType, std::map<Operator, NodeEval>>& GetEvaluators()
      const override {
    return evaluators_;
  }
};
}  // namespace cql2cpp

