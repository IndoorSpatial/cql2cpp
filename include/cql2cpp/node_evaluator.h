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
#include "geos/geom/Envelope.h"
#include "geos/geom/Geometry.h"
#include "geos/geom/GeometryFactory.h"

namespace cql2cpp {

constexpr double kEpsilon = 1e-5;

template <typename ValueType>
bool CheckValueNumberType(const std::string& op, size_t num,
                          const std::vector<ValueT>& vs, std::string* errmsg) {
  if (vs.size() != num) {
    *errmsg = op + " needs two values but we have " + std::to_string(vs.size());
    return false;
  }
  for (size_t i = 0; i < num; i++)
    if (not std::holds_alternative<ValueType>(vs.at(i))) {
      *errmsg = "value " + std::to_string(i) + "of " + op + " is incorrect";
      return false;
    }
  return true;
}

template <typename T>
inline bool TypedEqual(const ValueT& a, const ValueT& b) {
  return std::get<T>(a) == std::get<T>(b);
}

inline bool isVariantEqual(const ValueT& a, const ValueT& b) {
  if (a.index() != b.index()) return false;

  if (std::holds_alternative<bool>(a)) return TypedEqual<bool>(a, b);

  if (std::holds_alternative<int64_t>(a)) return TypedEqual<int64_t>(a, b);

  if (std::holds_alternative<uint64_t>(a)) return TypedEqual<uint64_t>(a, b);

  if (std::holds_alternative<double>(a))
    return fabs(std::get<double>(a) - std::get<double>(b)) < kEpsilon;

  if (std::holds_alternative<std::string>(a))
    return TypedEqual<std::string>(a, b);

  return false;
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
           *value = fabs(*lhs - *rhs) < kEpsilon;
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

