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

#include <geos/geom/Envelope.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>

#include <cmath>
#include <variant>

#include "evaluator.h"
#include "function.h"
#include "value_compare.h"

namespace cql2cpp {

template <typename ValueType>
bool CheckValueNumberType(const std::string& op, size_t num,
                          const std::vector<ValueT>& vs, std::string* errmsg) {
  if (vs.size() != num) {
    *errmsg = op + " needs two values but we have " + std::to_string(vs.size());
    return false;
  }
  for (size_t i = 0; i < num; i++)
    if (not std::holds_alternative<ValueType>(vs.at(i))) {
      *errmsg = "value " + std::to_string(i) + " of " + op + " is incorrect";
      return false;
    }
  return true;
}

inline bool ComparisonCheck(const std::vector<ValueT>& vs, double* left,
                            double* right, std::string* errmsg) {
  if (vs.size() != 2) {
    *errmsg = "binary compare needs two values but we have " +
              std::to_string(vs.size());
    return false;
  }

  if (std::holds_alternative<int64_t>(vs.at(0)))
    *left = std::get<int64_t>(vs.at(0));
  else if (std::holds_alternative<uint64_t>(vs.at(0)))
    *left = std::get<uint64_t>(vs.at(0));
  else if (std::holds_alternative<double>(vs.at(0)))
    *left = std::get<double>(vs.at(0));
  else {
    *errmsg = "left hand size of compare is not int or double";
    return false;
  }

  if (std::holds_alternative<int64_t>(vs.at(1)))
    *right = std::get<int64_t>(vs.at(1));
  else if (std::holds_alternative<uint64_t>(vs.at(1)))
    *right = std::get<uint64_t>(vs.at(1));
  else if (std::holds_alternative<double>(vs.at(1)))
    *right = std::get<double>(vs.at(1));
  else {
    *errmsg = "right hand size of compare is not int or double";
    return false;
  }

  if (std::isnan(*left)) {
    *errmsg = "left hand side is nan";
    return false;
  }
  if (std::isnan(*right)) {
    *errmsg = "right hand side is nan";
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
    {InList,
     {{NullOp,
       [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
         // InList doesn't have value
         // the parent(IsInListPred) will access the value of their grand
         // children directly
         *value = NullValue;
         return true;
       }}}},
    {IsInListPred,
     {
         {In,
          [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
            if (vs.size() != 2) {
              *errmsg = "(NOT)IN needs two values but we have " +
                        std::to_string(vs.size());
              return false;
            }
            if (not std::holds_alternative<NullStruct>(vs.at(1))) {
              *errmsg = "right hand side value of (NOT)IN should be null";
              return false;
            }
            if (n->children().size() != 2) {
              *errmsg =
                  "(NOT)IN needs two children for scalar expression and list "
                  "but we have " +
                  std::to_string(n->children().size());
              return false;
            }
            if (std::holds_alternative<NullStruct>(vs.at(0))) {
              *value = NullValue;
              return true;
            }
            if (not std::holds_alternative<bool>(vs.at(0)) and
                not std::holds_alternative<int64_t>(vs.at(0)) and
                not std::holds_alternative<uint64_t>(vs.at(0)) and
                not std::holds_alternative<double>(vs.at(0)) and
                not std::holds_alternative<std::string>(vs.at(0))) {
              *errmsg = "left hand side is not scalar type";
              return false;
            }
            AstNode* in_list = n->children()[1];
            for (const AstNode* grand_child : in_list->children()) {
              if (isVariantEqual(vs.at(0), grand_child->value())) {
                *value = true;
                return true;
              }
            }
            *value = false;
            return true;
          }},
         {NotIn,
          [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
            const NodeEval& equal_lambda = node_evals.at(IsInListPred).at(In);
            bool ret = equal_lambda.operator()(n, vs, fs, value, errmsg);
            if (ret) *value = not std::get<bool>(*value);
            return ret;
          }},
     }},
    {Array,
     {{NullOp,
       [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
         ArrayType result;
         for (const auto& v : vs) result.emplace_back(Element(v));
         *value = result;
         return true;
       }}}},
    {
        ArrayPred,
        {{A_Equals,
          [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
            const auto& contains = node_evals.at(ArrayPred).at(A_Contains);
            const auto& contained = node_evals.at(ArrayPred).at(A_ContainedBy);

            bool ret1 = contains.operator()(n, vs, fs, value, errmsg);
            bool result_1 = std::get<bool>(*value);
            if (not ret1) return false;
            bool ret2 = contained.operator()(n, vs, fs, value, errmsg);
            bool result_2 = std::get<bool>(*value);
            if (not ret2) return false;

            *value = result_1 and result_2;
            return true;
          }},
         {A_Contains,
          [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
            if (not CheckValueNumberType<ArrayType>("Array Op", 2, vs, errmsg))
              return false;
            const auto& lhs_array = std::get<ArrayType>(vs.at(0));
            const auto& rhs_array = std::get<ArrayType>(vs.at(1));
            if (lhs_array.size() < rhs_array.size()) {
              *value = false;
              return true;
            }
            SetType lhs_set(lhs_array.begin(), lhs_array.end());
            SetType rhs_set(rhs_array.begin(), rhs_array.end());
            for (const auto& e : rhs_set)
              if (lhs_set.find(e) == lhs_set.end()) {
                *value = false;
                return true;
              }
            *value = true;
            return true;
          }},
         {A_ContainedBy,
          [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
            if (not CheckValueNumberType<ArrayType>("Array Op", 2, vs, errmsg))
              return false;

            const auto& lhs_array = std::get<ArrayType>(vs.at(0));
            const auto& rhs_array = std::get<ArrayType>(vs.at(1));
            if (lhs_array.size() > rhs_array.size()) {
              *value = false;
              return true;
            }
            SetType lhs_set(lhs_array.begin(), lhs_array.end());
            SetType rhs_set(rhs_array.begin(), rhs_array.end());
            for (const auto& e : lhs_set)
              if (rhs_set.find(e) == rhs_set.end()) {
                *value = false;
                return true;
              }
            *value = true;
            return true;
          }},
         {A_Overlaps,
          [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
            return true;
          }}},
    },
    {SpatialPred,
     {{S_Intersects,
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
         else if (std::holds_alternative<const geos::geom::Envelope*>(
                      vs.at(0))) {
           lhs_shared = factory->toGeometry(
               std::get<const geos::geom::Envelope*>(vs.at(0)));
           lhs = lhs_shared.get();
         } else {
           *errmsg =
               "left hand side value type of S_INTERSECTS should be geometry "
               "or bbox";
           return false;
         }
         const geos::geom::Geometry* rhs = nullptr;
         std::shared_ptr<geos::geom::Geometry> rhs_shared;
         if (std::holds_alternative<const geos::geom::Geometry*>(vs.at(1)))
           rhs = std::get<const geos::geom::Geometry*>(vs.at(1));
         else if (std::holds_alternative<const geos::geom::Envelope*>(
                      vs.at(1))) {
           rhs_shared = factory->toGeometry(
               std::get<const geos::geom::Envelope*>(vs.at(1)));
           rhs = rhs_shared.get();
         } else {
           *errmsg =
               "right hand side value type of S_INTERSECTS should be geometry "
               "or bbox";
           return false;
         }
         *value = lhs->intersects(rhs);
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
    {Function,
     {{NullOp,
       [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
         if (vs.empty()) {
           *errmsg = "function needs a name and argument list";
           return false;
         }
         if (not std::holds_alternative<std::string>(vs.at(0))) {
           *errmsg = "function name should be a string";
           return false;
         }
         std::string function_name = std::get<std::string>(vs.at(0));
         if (functions.find(function_name) == functions.end()) {
           *errmsg = "can not find function " + function_name;
           return false;
         }
         if (vs.size() == 1)
           return functions.at(function_name).operator()({}, value, errmsg);
         else if (vs.size() == 2) {
           if (not std::holds_alternative<ArrayType>(vs.at(1))) {
             *errmsg = "the second value of a function should be argument list";
             return false;
           }
           std::vector<ValueT> vec;
           for (const auto& element : std::get<ArrayType>(vs.at(1)))
             vec.emplace_back(element.value);

           return functions.at(function_name)
               .
               operator()(vec, value, errmsg);
         } else {
           *errmsg =
               "function needs only two child (name and argument list) but we "
               "get " +
               std::to_string(vs.size());
           return false;
         }
       }}}},
    {ArgumentList,
     {{NullOp,
       [](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
         ArrayType result;
         for (const auto& v : vs) result.emplace_back(Element(v));
         *value = result;
         return true;
       }}}},
    {BinCompPred,
     {{Greater, {[](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
         double left, right;
         if (not ComparisonCheck(vs, &left, &right, errmsg)) return false;
         *value = (left > right);
         return true;
       }}},
      {GreaterEqual,
       {[](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
         double left, right;
         if (not ComparisonCheck(vs, &left, &right, errmsg)) return false;
         *value = (left >= right);
         return true;
       }}},
      {Lesser, {[](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
         double left, right;
         if (not ComparisonCheck(vs, &left, &right, errmsg)) return false;
         *value = (left < right);
         return true;
       }}},
      {LesserEqual,
       {[](auto n, auto vs, auto fs, auto value, auto errmsg) -> bool {
         double left, right;
         if (not ComparisonCheck(vs, &left, &right, errmsg)) return false;
         *value = (left <= right);
         return true;
       }}},
      {NotEqual,
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

         double left, right;
         if (not ComparisonCheck(vs, &left, &right, errmsg)) return false;
         *value = (fabs(left - right) < kEpsilon);
         return true;
       }}}},
};

}  // namespace cql2cpp

