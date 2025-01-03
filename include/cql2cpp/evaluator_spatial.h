/*
 * File Name: evaluator_spatial.h
 *
 * Copyright (c) 2024-2025 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2025/01/02
 *
 */

#pragma once

#include <geos/geom/GeometryFactory.h>

#include "evaluator_ast_node.h"

namespace cql2cpp {

class EvaluatorSpatial : public EvaluatorAstNode {
 private:
  std::map<NodeType, std::map<Operator, NodeEval>> evaluators_;

 public:
  EvaluatorSpatial() {
    evaluators_[SpatialPred][S_Intersects] =
        [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
      if (vs.size() != 2) {
        *errmsg = "S_INTERSECTS needs two values but we have " +
                  std::to_string(vs.size());
        return false;
      }
      geos::geom::GeometryFactory::Ptr factory =
          geos::geom::GeometryFactory::create();
      const geos::geom::Geometry* lhs = nullptr;
      std::shared_ptr<geos::geom::Geometry> lhs_shared;
      if (std::holds_alternative<const geos::geom::Geometry*>(vs.at(0)))
        lhs = std::get<const geos::geom::Geometry*>(vs.at(0));
      else if (std::holds_alternative<const geos::geom::Envelope*>(vs.at(0))) {
        lhs_shared = factory->toGeometry(
            std::get<const geos::geom::Envelope*>(vs.at(0)));
        lhs = lhs_shared.get();
      } else {
        *errmsg =
            "left hand side value type of S_INTERSECTS should be geometry or "
            "bbox";
        return false;
      }
      const geos::geom::Geometry* rhs = nullptr;
      std::shared_ptr<geos::geom::Geometry> rhs_shared;
      if (std::holds_alternative<const geos::geom::Geometry*>(vs.at(1)))
        rhs = std::get<const geos::geom::Geometry*>(vs.at(1));
      else if (std::holds_alternative<const geos::geom::Envelope*>(vs.at(1))) {
        rhs_shared = factory->toGeometry(
            std::get<const geos::geom::Envelope*>(vs.at(1)));
        rhs = rhs_shared.get();
      } else {
        *errmsg =
            "right hand side value type of S_INTERSECTS should be geometry or "
            "bbox";
        return false;
      }
      *value = lhs->intersects(rhs);
      return true;
    };
  }
  const std::map<NodeType, std::map<Operator, NodeEval>>& GetEvaluators()
      const override {
    return evaluators_;
  }
};
}  // namespace cql2cpp

