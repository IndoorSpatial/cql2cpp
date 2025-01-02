/*
 * File Name: evaluator_literal.h
 *
 * Copyright (c) 2024-2025 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2025/01/02
 *
 */

#pragma once

#include "evaluator.h"

namespace cql2cpp {

class EvaluatorLiteral : public NodeEvaluator {
 private:
  std::map<NodeType, std::map<Operator, NodeEval>> evaluators_;

 public:
  EvaluatorLiteral() {
    evaluators_[PropertyName][NullOp] = [](auto n, auto vs, auto fs, auto value,
                                           auto errmsg) -> bool {
      *value = n->value();
      return true;
    };
  }
  const std::map<NodeType, std::map<Operator, NodeEval>>& GetEvaluators()
      const override {
    return evaluators_;
  }
};
}  // namespace cql2cpp

