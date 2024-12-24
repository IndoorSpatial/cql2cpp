/*
 * File Name: node_evaluator.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/19
 *
 */

#pragma once

#include <cmath>
#include <variant>

#include "evaluator.h"

namespace cql2cpp {

template <typename ValueType>
bool CheckValueNumberType(const std::string& op, size_t num,
                          const std::vector<ValueT>& vs, std::string* errmsg) {
  if (vs.size() != num) {
    *errmsg = op + " needs two values but we have " + std::to_string(vs.size());
    return false;
  }
  for (size_t i = 0; i < num; i++)
    if (not std::holds_alternative<ValueType>(vs.at(0))) {
      *errmsg = "value " + std::to_string(i) + "of " + op + " is incorrect";
      return false;
    }
  return true;
}

const std::map<NodeType, std::map<Operator, NodeEval>> node_evals = {
    {Literal,
     {{NullOp,
       [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
         *value = n->value();
         return true;
       }}}},
    {BoolExpression,
     {{And,
       [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
         if (not CheckValueNumberType<bool>("AND", 2, vs, errmsg)) return false;
         *value = std::get<bool>(vs.at(0)) and std::get<bool>(vs.at(1));
         return true;
       }},
      {Or,
       [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
         if (not CheckValueNumberType<bool>("OR", 2, vs, errmsg)) return false;
         *value = std::get<bool>(vs.at(0)) or std::get<bool>(vs.at(1));
         return true;
       }},
      {Not,
       [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
         if (not CheckValueNumberType<bool>("NOT", 1, vs, errmsg)) return false;
         *value = not std::get<bool>(vs.at(0));
         return true;
       }}}},
    {PropertyName,
     {{NullOp,
       [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
         if (not std::holds_alternative<std::string>(n->value())) {
           *errmsg = "value of property name is not string";
           return false;
         }
         *value = fs->get_property(std::get<std::string>(n->value()));
         return true;
       }}}},
    {BinCompPred,
     {{NotEqual,
       {[](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
         const NodeEval& equal_lambda = node_evals.at(BinCompPred).at(Equal);
         bool ret = equal_lambda.operator()(n, vs, fs, value, errmsg);
         if (ret) *value = not std::get<bool>(*value);
         return ret;
       }}},
      {Equal,
       [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
         if (vs.size() != 2) {
           *errmsg = "binary compare needs two values but we have " +
                     std::to_string(vs.size());
           return false;
         }
         if (std::holds_alternative<bool>(vs.at(0)) and
             std::holds_alternative<bool>(vs.at(1))) {
           *value = std::get<bool>(vs.at(0)) == std::get<bool>(vs.at(1));
           return true;
         }
         if (std::holds_alternative<std::string>(vs.at(0)) and
             std::holds_alternative<std::string>(vs.at(1))) {
           *value = std::get<std::string>(vs.at(0)) ==
                    std::get<std::string>(vs.at(1));
           return true;
         }

         double* lhs = nullptr;
         if (std::holds_alternative<int64_t>(vs.at(0))) {
           lhs = new double;
           *lhs = (double)std::get<int64_t>(vs.at(0));
         } else if (std::holds_alternative<uint64_t>(vs.at(0))) {
           lhs = new double;
           *lhs = (double)std::get<uint64_t>(vs.at(0));
         } else if (std::holds_alternative<double>(vs.at(0))) {
           lhs = new double;
           *lhs = std::get<double>(vs.at(0));
         }
         double* rhs = nullptr;
         if (std::holds_alternative<int64_t>(vs.at(1))) {
           rhs = new double;
           *rhs = (double)std::get<int64_t>(vs.at(1));
         } else if (std::holds_alternative<uint64_t>(vs.at(1))) {
           rhs = new double;
           *rhs = (double)std::get<uint64_t>(vs.at(1));
         } else if (std::holds_alternative<double>(vs.at(1))) {
           rhs = new double;
           *rhs = std::get<double>(vs.at(1));
         }

         if (lhs != nullptr and rhs != nullptr) {
           *value = fabs(*lhs - *rhs) < 1e-5;
           delete lhs;
           delete rhs;
           return true;
         }

         if (lhs != nullptr) delete lhs;
         if (rhs != nullptr) delete rhs;

         *errmsg = "type mismatch around equal";
         return false;
       }}}},
};

}  // namespace cql2cpp

