/*
 * File Name: evaluator_function.h
 *
 * Copyright (c) 2024-2025 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2025/01/02
 *
 */

#pragma once

#include <cql2cpp/ast_node.h>
#include <cql2cpp/functor.h>
#include "ast_node.h"

namespace cql2cpp {

class EvaluatorFunction : public EvaluatorAstNode {
 private:
  std::map<NodeType, std::map<Operator, NodeEval>> evaluators_;
  std::map<std::string, FunctorPtr> functions_;

 public:
  void Register(const FunctorPtr& functor) {
    if (functions_.find(functor->name()) != functions_.end())
      LOG(WARNING) << "replace functor " << functor->name();
    functions_[functor->name()] = functor;
  }

  EvaluatorFunction() {
    evaluators_[Function][NullOp] = [this](auto n, auto vs, auto fs, auto value,
                                           auto errmsg) -> bool {
      if (vs.empty()) {
        *errmsg = "function needs a name and argument list";
        return false;
      }
      if (not std::holds_alternative<std::string>(vs.at(0))) {
        *errmsg = "function name should be a string";
        return false;
      }
      std::string function_name = std::get<std::string>(vs.at(0));
      if (functions_.find(function_name) == functions_.end()) {
        *errmsg = "can not find function " + function_name;
        return false;
      }
      if (vs.size() == 1)
        return functions_.at(function_name)->operator()({}, value, errmsg);
      else if (vs.size() == 2) {
        if (not std::holds_alternative<ArrayType>(vs.at(1))) {
          *errmsg = "the second value of a function should be argument list";
          return false;
        }
        std::vector<ValueT> vec;
        for (const auto& element : std::get<ArrayType>(vs.at(1)))
          vec.emplace_back(element.value);

        return functions_.at(function_name)->operator()(vec, value, errmsg);
      } else {
        *errmsg =
            "function needs only two child (name and argument list) but we "
            "get " +
            std::to_string(vs.size());
        return false;
      }
    };
    evaluators_[ArgumentList][NullOp] = [](auto n, auto vs, auto fs, auto value,
                                           auto errmsg) -> bool {
      ArrayType result;
      for (const auto& v : vs) result.emplace_back(Element(v));
      *value = result;
      return true;
    };
  }
  const std::map<NodeType, std::map<Operator, NodeEval>>& GetEvaluators()
      const override {
    return evaluators_;
  }
};
}  // namespace cql2cpp

