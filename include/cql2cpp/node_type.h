/*
 * File Name: node_type.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/19
 *
 */

#pragma once

namespace cql2cpp {

enum NodeType {
  BoolExpr,
  BoolTerm,
  BoolFactor,
  BoolPrimary,

  // Comparison Predicate
  BinCompPred,
  IsLikePred,
  IsBetweenPred,
  IsInListPred,
  IsNullPred,

  SpatialPred,
  TemporalPred,
  ArrayPred,
};

}  // namespace cql2cpp

