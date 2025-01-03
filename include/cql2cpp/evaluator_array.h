/*
 * File Name: evaluator_array.h
 *
 * Copyright (c) 2024-2025 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2025/01/02
 *
 */

#pragma once

#include "evaluator_ast_node.h"

namespace cql2cpp {

class EvaluatorArray : public EvaluatorAstNode {
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
  EvaluatorArray() {
    evaluators_[Array][NullOp] = [](auto n, auto vs, auto fs, auto value,
                                    auto errmsg) -> bool {
      ArrayType result;
      for (const auto& v : vs) result.emplace_back(Element(v));
      *value = result;
      return true;
    };

    evaluators_[ArrayPred][A_Equals] = [this](auto n, auto vs, auto fs,
                                              auto value, auto errmsg) -> bool {
      const auto& contains = evaluators_.at(ArrayPred).at(A_Contains);
      const auto& contained = evaluators_.at(ArrayPred).at(A_ContainedBy);

      bool ret1 = contains.operator()(n, vs, fs, value, errmsg);
      bool result_1 = std::get<bool>(*value);
      if (not ret1) return false;
      bool ret2 = contained.operator()(n, vs, fs, value, errmsg);
      bool result_2 = std::get<bool>(*value);
      if (not ret2) return false;

      *value = result_1 and result_2;
      return true;
    };

    evaluators_[ArrayPred][A_Contains] = [](auto n, auto vs, auto fs,
                                            auto value, auto errmsg) -> bool {
      if (not CheckValueNumberType<ArrayType>("Array Op", 2, vs, errmsg))
        return false;
      const auto& lhs_array = std::get<ArrayType>(vs.at(0));
      const auto& rhs_array = std::get<ArrayType>(vs.at(1));
      if (lhs_array.size() < rhs_array.size()) {
        *value = false;
        return true;
      }
      SetType lhs_set(lhs_array.begin(), lhs_array.end());
      SetType rhs_set(rhs_array.begin(), rhs_array.end());
      for (const auto& e : rhs_set)
        if (lhs_set.find(e) == lhs_set.end()) {
          *value = false;
          return true;
        }
      *value = true;
      return true;
    };

    evaluators_[ArrayPred][A_ContainedBy] =
        [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
      if (not CheckValueNumberType<ArrayType>("Array Op", 2, vs, errmsg))
        return false;

      const auto& lhs_array = std::get<ArrayType>(vs.at(0));
      const auto& rhs_array = std::get<ArrayType>(vs.at(1));
      if (lhs_array.size() > rhs_array.size()) {
        *value = false;
        return true;
      }
      SetType lhs_set(lhs_array.begin(), lhs_array.end());
      SetType rhs_set(rhs_array.begin(), rhs_array.end());
      for (const auto& e : lhs_set)
        if (rhs_set.find(e) == rhs_set.end()) {
          *value = false;
          return true;
        }
      *value = true;
      return true;
    };

    evaluators_[ArrayPred][A_Overlaps] = [](auto n, auto vs, auto fs,
                                            auto value, auto errmsg) -> bool {
      // TODO
      return false;
    };
  }
  const std::map<NodeType, std::map<Operator, NodeEval>>& GetEvaluators()
      const override {
    return evaluators_;
  }
};
}  // namespace cql2cpp

