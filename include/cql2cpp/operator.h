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

#include <map>
#include <string>

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

#define OP_2_NAME(op) {op, #op},

const std::map<Operator, std::string> OpName {
  OP_2_NAME(NullOp)

  OP_2_NAME(And)
  OP_2_NAME(Or)
  OP_2_NAME(Not)

  OP_2_NAME(Equal)
  OP_2_NAME(NotEqual)
  OP_2_NAME(Greater)
  OP_2_NAME(Lesser)
  OP_2_NAME(GreaterEqual)
  OP_2_NAME(LesserEqual)

  OP_2_NAME(Like)
  OP_2_NAME(Between)
  OP_2_NAME(In)
  OP_2_NAME(IsNull)

  OP_2_NAME(S_Contains)
  OP_2_NAME(S_Crosses)
  OP_2_NAME(S_Equals)
  OP_2_NAME(S_Intersects)
  OP_2_NAME(S_Overlaps)
  OP_2_NAME(S_Touches)
  OP_2_NAME(S_Within)

  OP_2_NAME(T_After)
  OP_2_NAME(T_Before)
  OP_2_NAME(T_Contains)
  OP_2_NAME(T_Disjoint)
  OP_2_NAME(T_During)
  OP_2_NAME(T_Equals)
  OP_2_NAME(T_FinishedBy)
  OP_2_NAME(T_Finishes)
  OP_2_NAME(T_Intersects)
  OP_2_NAME(T_Meets)
  OP_2_NAME(T_MetBy)
  OP_2_NAME(T_OverlappedBy)
  OP_2_NAME(T_Overlaps)
  OP_2_NAME(T_StartedBy)
  OP_2_NAME(T_Starts)

  OP_2_NAME(A_ContainedBy)
  OP_2_NAME(A_Contains)
  OP_2_NAME(A_Equals)
  OP_2_NAME(A_Overlaps)
};

#define NAME_2_OP(op) {#op, op},

const std::map<std::string, Operator> NameOp {
  NAME_2_OP(NullOp)
  NAME_2_OP(And)
  NAME_2_OP(Or)
  NAME_2_OP(Not)
  NAME_2_OP(Equal)
  NAME_2_OP(NotEqual)
  NAME_2_OP(Greater)
  NAME_2_OP(Lesser)
  NAME_2_OP(GreaterEqual)
  NAME_2_OP(LesserEqual)
  NAME_2_OP(Like)
  NAME_2_OP(Between)
  NAME_2_OP(In)
  NAME_2_OP(IsNull)
  NAME_2_OP(S_Contains)
  NAME_2_OP(S_Crosses)
  NAME_2_OP(S_Equals)
  NAME_2_OP(S_Intersects)
  NAME_2_OP(S_Overlaps)
  NAME_2_OP(S_Touches)
  NAME_2_OP(S_Within)
  NAME_2_OP(T_After)
  NAME_2_OP(T_Before)
  NAME_2_OP(T_Contains)
  NAME_2_OP(T_Disjoint)
  NAME_2_OP(T_During)
  NAME_2_OP(T_Equals)
  NAME_2_OP(T_FinishedBy)
  NAME_2_OP(T_Finishes)
  NAME_2_OP(T_Intersects)
  NAME_2_OP(T_Meets)
  NAME_2_OP(T_MetBy)
  NAME_2_OP(T_OverlappedBy)
  NAME_2_OP(T_Overlaps)
  NAME_2_OP(T_StartedBy)
  NAME_2_OP(T_Starts)
  NAME_2_OP(A_ContainedBy)
  NAME_2_OP(A_Contains)
  NAME_2_OP(A_Equals)
  NAME_2_OP(A_Overlaps)
};

}
