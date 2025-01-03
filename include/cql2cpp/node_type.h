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

#include <map>
#include <string>

namespace cql2cpp {


enum NodeType {
  Literal,

  BoolExpression,

  // Comparison Predicate
  BinCompPred,
  IsLikePred,
  IsBetweenPred,

  IsInListPred,
  InList,

  IsNullPred,

  SpatialPred,

  TemporalPred,

  ArrayPred,
  Array,

  PropertyName,

  Function,
  ArgumentList,
};

#define TYPE_2_NAME(op) {op, #op },

const std::map<NodeType, std::string> TypeName {
  TYPE_2_NAME(Literal)
  TYPE_2_NAME(BoolExpression)
  TYPE_2_NAME(BinCompPred)
  TYPE_2_NAME(IsLikePred)
  TYPE_2_NAME(IsBetweenPred)
  TYPE_2_NAME(IsInListPred)
  TYPE_2_NAME(InList)
  TYPE_2_NAME(IsNullPred)
  TYPE_2_NAME(SpatialPred)
  TYPE_2_NAME(TemporalPred)
  TYPE_2_NAME(ArrayPred)
  TYPE_2_NAME(Array)
  TYPE_2_NAME(PropertyName)
  TYPE_2_NAME(Function)
  TYPE_2_NAME(ArgumentList)
};

}  // namespace cql2cpp

