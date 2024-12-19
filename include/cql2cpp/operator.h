/*
 * File Name: ast_node.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/19
 *
 */

#pragma once

namespace cql2cpp {

enum Operator {
  NullOp,

  // logical operators
  And,
  Or,
  Not,

  // comparison operators
  Equal,
  NotEqual,
  Greater,
  Lesser,
  GreaterEqual,
  LesserEqual,

  Like,
  Between,
  In,
  IsNull,

  // spatial operators
  S_Contains,
  S_Crosses,
  S_Equals,
  S_Intersects,
  S_Overlaps,
  S_Touches,
  S_Within,

  // temporal operators
  T_After,
  T_Before,
  T_Contains,
  T_Disjoint,
  T_During,
  T_Equals,
  T_FinishedBy,
  T_Finishes,
  T_Intersects,
  T_Meets,
  T_MetBy,
  T_OverlappedBy,
  T_Overlaps,
  T_StartedBy,
  T_Starts,

  // array operators
  A_ContainedBy,
  A_Contains,
  A_Equals,
  A_Overlaps,
};

}
