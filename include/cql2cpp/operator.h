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
  NotLike,
  Between,
  NotBetween,
  In,
  NotIn,
  IsNull,
  IsNotNull,

  // arithmetic operators
  PLUS,
  MINUS,
  MULT,
  DIV,
  DIVINT,
  MOD,
  POWER,

  // spatial operators
  S_Contains,
  S_Crosses,
  S_Disjoint,
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
  A_Equals,
  A_Contains,
  A_ContainedBy,
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
  OP_2_NAME(NotLike)
  OP_2_NAME(Between)
  OP_2_NAME(NotBetween)
  OP_2_NAME(In)
  OP_2_NAME(NotIn)
  OP_2_NAME(IsNull)
  OP_2_NAME(IsNotNull)

  OP_2_NAME(PLUS)
  OP_2_NAME(MINUS)
  OP_2_NAME(MULT)
  OP_2_NAME(DIV)
  OP_2_NAME(DIVINT)
  OP_2_NAME(MOD)
  OP_2_NAME(POWER)

  OP_2_NAME(S_Contains)
  OP_2_NAME(S_Crosses)
  OP_2_NAME(S_Disjoint)
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

const std::map<std::string, Operator> NameOp {
  {"S_CONTAINS", S_Contains},
  {"S_CROSSES", S_Crosses},
  {"S_DISJOINT", S_Disjoint},
  {"S_EQUALS", S_Equals},
  {"S_INTERSECTS", S_Intersects},
  {"S_OVERLAPS", S_Overlaps},
  {"S_TOUCHES", S_Touches},
  {"S_WITHIN", S_Within},

  {"T_AFTER", T_After},
  {"T_BEFORE", T_Before},
  {"T_CONTAINS", T_Contains},
  {"T_DISJOINT", T_Disjoint},
  {"T_DURING", T_During},
  {"T_EQUALS", T_Equals},
  {"T_FINISHEDBY", T_FinishedBy},
  {"T_FINISHES", T_Finishes},
  {"T_INTERSECTS", T_Intersects},
  {"T_MEETS", T_Meets},
  {"T_METBY", T_MetBy},
  {"T_OVERLAPPEDBY", T_OverlappedBy},
  {"T_OVERLAPS", T_Overlaps},
  {"T_STARTEDBY", T_StartedBy},
  {"T_STARTS", T_Starts},

  {"A_CONTAINEDBY", A_ContainedBy},
  {"A_CONTAINS", A_Contains},
  {"A_EQUALS", A_Equals},
  {"A_OVERLAPS", A_Overlaps}
};

}
