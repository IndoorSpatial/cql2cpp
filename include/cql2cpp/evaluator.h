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

#include <map>

#include "ast_node.h"
#include "evaluator/array.h"
#include "evaluator/ast_node.h"
#include "evaluator/bool.h"
#include "evaluator/compare.h"
#include "evaluator/function.h"
#include "evaluator/in.h"
#include "evaluator/literal.h"
#include "evaluator/property.h"
#include "evaluator/spatial.h"
#include "feature_source.h"
#include "functor.h"
#include "functor_avg.h"
#include "functor_buffer.h"
#include "functor_related_bins.h"

namespace cql2cpp {

class Evaluator {
 private:
  std::map<NodeType, std::map<Operator, NodeEval>> type_evaluator_;
  EvaluatorFunction eval_func;
  mutable std::string error_msg_;

 public:
  Evaluator() {
    Register(EvaluatorBool().GetEvaluators());
    Register(EvaluatorCompare().GetEvaluators());
    Register(EvaluatorSpatial().GetEvaluators());
    Register(EvaluatorArray().GetEvaluators());
    Register(EvaluatorIn().GetEvaluators());
    Register(EvaluatorLiteral().GetEvaluators());
    Register(EvaluatorProperty().GetEvaluators());

    Register(eval_func.GetEvaluators());
    eval_func.Register(std::make_shared<FunctorAvg>());
    eval_func.Register(std::make_shared<FunctorBuffer>());
    eval_func.Register(std::make_shared<FunctorRelatedBins>());
  }

  void Register(
      const std::map<NodeType, std::map<Operator, NodeEval>> evaluators) {
    type_evaluator_.insert(evaluators.begin(), evaluators.end());
  }

  void RegisterFunctor(const FunctorPtr functor) {
    eval_func.Register(functor);
  }

  bool Evaluate(const AstNodePtr root, const FeatureSource* fs,
                ValueT* result) const {
    if (type_evaluator_.find(root->type()) == type_evaluator_.end() ||
        type_evaluator_.at(root->type()).find(root->op()) ==
            type_evaluator_.at(root->type()).end()) {
      error_msg_ = "can not find evaluator for operator \"" +
                   OpName.at(root->op()) + "\" in node type \"" +
                   TypeName.at(root->type()) + "\"";
      return false;
    }

    std::vector<ValueT> child_values;
    for (AstNodePtr child : root->children()) {
      ValueT value;
      if (Evaluate(child, fs, &value))
        child_values.emplace_back(value);
      else
        return false;
    }

    bool ret = type_evaluator_.at(root->type())
                   .at(root->op())
                   .
                   operator()(root, child_values, fs, result, &error_msg_);
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

  const std::string& error_msg() const { return error_msg_; }
};

}  // namespace cql2cpp
