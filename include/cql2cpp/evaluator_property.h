/*
 * File Name: evaluator_property.h
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

class EvaluatorProperty : public NodeEvaluator {
 private:
  std::map<NodeType, std::map<Operator, NodeEval>> evaluators_;

 public:
  EvaluatorProperty() {
    evaluators_[PropertyName][NullOp] = [](auto n, auto vs, auto fs, auto value,
                                           auto errmsg) -> bool {
      if (not std::holds_alternative<std::string>(n->value())) {
        *errmsg = "value of property name is not string";
        return false;
      }
      *value = fs->get_property(std::get<std::string>(n->value()));
      return true;
    };
  }
  const std::map<NodeType, std::map<Operator, NodeEval>>& GetEvaluators()
      const override {
    return evaluators_;
  }
};
}  // namespace cql2cpp

