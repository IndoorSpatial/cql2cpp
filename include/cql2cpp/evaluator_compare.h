/*
 * File Name: evaluator_compare.h
 *
 * Copyright (c) 2024-2025 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2025/01/02
 *
 */

#pragma once

#include "evaluator.h"
#include "value_compare.h"

namespace cql2cpp {

class EvaluatorCompare : public NodeEvaluator {
 private:
  std::map<NodeType, std::map<Operator, NodeEval>> evaluators_;

 public:
  EvaluatorCompare() {
    evaluators_[BinCompPred][Greater] = [](auto n, auto vs, auto fs, auto value,
                                           auto errmsg) -> bool {
      double left, right;
      if (not ComparisonCheck(vs, &left, &right, errmsg)) return false;
      *value = (left > right);
      return true;
    };
    evaluators_[BinCompPred][GreaterEqual] =
        [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
      double left, right;
      if (not ComparisonCheck(vs, &left, &right, errmsg)) return false;
      *value = (left >= right);
      return true;
    };
    evaluators_[BinCompPred][Lesser] = [](auto n, auto vs, auto fs, auto value,
                                          auto errmsg) -> bool {
      double left, right;
      if (not ComparisonCheck(vs, &left, &right, errmsg)) return false;
      *value = (left < right);
      return true;
    };
    evaluators_[BinCompPred][LesserEqual] =
        [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
      double left, right;
      if (not ComparisonCheck(vs, &left, &right, errmsg)) return false;
      *value = (left <= right);
      return true;
    };
    evaluators_[BinCompPred][NotEqual] =
        [this](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
      const NodeEval& equal_lambda = evaluators_.at(BinCompPred).at(Equal);
      bool ret = equal_lambda.operator()(n, vs, fs, value, errmsg);
      if (ret) *value = not std::get<bool>(*value);
      return ret;
    };
    evaluators_[BinCompPred][Equal] = [](auto n, auto vs, auto fs, auto value,
                                         auto errmsg) -> bool {
      if (vs.size() != 2) {
        *errmsg = "binary compare needs two values but we have " +
                  std::to_string(vs.size());
        return false;
      }
      if (std::holds_alternative<bool>(vs.at(0)) and
          std::holds_alternative<bool>(vs.at(1))) {
        *value = std::get<bool>(vs.at(0)) == std::get<bool>(vs.at(1));
        return true;
      }
      if (std::holds_alternative<std::string>(vs.at(0)) and
          std::holds_alternative<std::string>(vs.at(1))) {
        *value =
            std::get<std::string>(vs.at(0)) == std::get<std::string>(vs.at(1));
        return true;
      }

      double left, right;
      if (not ComparisonCheck(vs, &left, &right, errmsg)) return false;
      *value = (fabs(left - right) < kEpsilon);
      return true;
    };
  }
  const std::map<NodeType, std::map<Operator, NodeEval>>& GetEvaluators()
      const override {
    return evaluators_;
  }
};
}  // namespace cql2cpp

