/*
 * File Name: evaluator_ast_node.h
 *
 * Copyright (c) 2024-2025 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2025/01/03
 *
 */

#pragma once

#include <memory>
#include <functional>
#include <cql2cpp/ast_node.h>
#include <cql2cpp/node_type.h>
#include <cql2cpp/operator.h>
#include <cql2cpp/feature_source.h>

namespace cql2cpp {

using NodeEval =
    std::function<bool(const AstNodePtr, const std::vector<ValueT>&,
                       const FeatureSource*, ValueT*, std::string* error_msg)>;

class EvaluatorAstNode {
 public:
   virtual const std::map<NodeType, std::map<Operator, NodeEval>>& GetEvaluators() const = 0;
};

using EvaluatorAstNodePtr = std::shared_ptr<EvaluatorAstNode>;

}  // namespace cql2cpp

