/*
 * File Name: sql_converter.h
 *
 * Copyright (c) 2024-2025 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2025/05/12
 *
 */

#pragma once
#include <cql2cpp/ast_node.h>

#include <variant>

#include "geos/io/WKTWriter.h"

namespace cql2cpp {

using NodeConv = std::function<std::string(const AstNodePtr,
                                           const std::vector<std::string>&)>;

class SqlConverter {
 private:
  std::map<NodeType, std::map<Operator, NodeConv>> converters;
  mutable std::string error_msg_;

 public:
  SqlConverter() {
    converters[BoolExpr][And] = [](auto n, auto c) -> std::string {
      return c.at(0) + " AND " + c.at(1);
    };
    converters[BoolExpr][Or] = [](auto n, auto c) -> std::string {
      return c.at(0) + " OR " + c.at(1);
    };
    converters[BoolExpr][Not] = [](auto n, auto c) -> std::string {
      return "NOT " + c.at(1);
    };
    converters[BinCompPred][Greater] = [](auto n, auto c) -> std::string {
      return c.at(0) + " > " + c.at(1);
    };
    converters[BinCompPred][GreaterEqual] = [](auto n, auto c) -> std::string {
      return c.at(0) + " >= " + c.at(1);
    };
    converters[BinCompPred][Lesser] = [](auto n, auto c) -> std::string {
      return c.at(0) + " < " + c.at(1);
    };
    converters[BinCompPred][LesserEqual] = [](auto n, auto c) -> std::string {
      return c.at(0) + " <= " + c.at(1);
    };
    converters[BinCompPred][NotEqual] = [](auto n, auto c) -> std::string {
      return c.at(0) + " <> " + c.at(1);
    };
    converters[BinCompPred][Equal] = [](auto n, auto c) -> std::string {
      return c.at(0) + " = " + c.at(1);
    };
    converters[Function][NullOp] = [](const AstNodePtr n, auto c) -> std::string {
      return std::get<std::string>(n->children().front()->origin_value()) + "(" + c.at(1) + ")";
    };
    converters[ArgumentList][NullOp] = [](auto n, auto c) -> std::string {
      std::stringstream ss;
      for (size_t i = 0; i < c.size(); i++) {
        if (i > 0) ss << ",";
        ss << c[i];
      }
      return ss.str();
    };
    converters[IsInListPred][In] = [](auto n, auto c) -> std::string {
      return c.at(0) + " IN (" + c.at(1) + ")";
    };
    converters[IsInListPred][NotIn] = [](auto n, auto c) -> std::string {
      return c.at(0) + " NOT IN (" + c.at(1) + ")";
    };
    converters[InList][NullOp] = [](auto n, auto c) -> std::string {
      std::stringstream ss;
      for (size_t i = 0; i < c.size(); i++) {
        if (i > 0) ss << ",";
        ss << c[i];
      }
      return ss.str();
    };
    converters[Literal][NullOp] = [](auto n, auto c) -> std::string {
      if (std::holds_alternative<bool>(n->origin_value()))
        return std::get<bool>(n->origin_value()) ? "TRUE" : "FALSE";
      else if (std::holds_alternative<int64_t>(n->origin_value()))
        return std::to_string(std::get<int64_t>(n->origin_value()));
      else if (std::holds_alternative<uint64_t>(n->origin_value()))
        return std::to_string(std::get<uint64_t>(n->origin_value()));
      else if (std::holds_alternative<double>(n->origin_value()))
        return std::to_string(std::get<double>(n->origin_value()));
      else if (std::holds_alternative<std::string>(n->origin_value()))
        return "'" + std::get<std::string>(n->origin_value()) + "'";
      else if (std::holds_alternative<const geos::geom::Geometry*>(
                   n->origin_value())) {
        geos::io::WKTWriter writer;
        auto geom = std::get<const geos::geom::Geometry*>(n->origin_value());
        auto wkt = writer.write(geom);
        return "ST_GeomFromText('" + wkt + "')";
      } else if (std::holds_alternative<const geos::geom::Envelope*>(
                     n->origin_value())) {
        const geos::geom::Envelope* env =
            std::get<const geos::geom::Envelope*>(n->origin_value());
        std::stringstream ss;
        ss << "ST_MakeEnvelope(";
        ss << env->getMinX() << "," << env->getMinY() << ",";
        ss << env->getMaxX() << "," << env->getMaxY() << ")";
        return ss.str();
      } else {
        // TODO: support array literal
        LOG(ERROR) << "unsupported array literal";
        return "";
      }
    };

    converters[PropertyName][NullOp] = [](auto n, auto c) -> std::string {
      // TODO: this is database related. We should add an option for dialects
      // sqlite: "" / []
      // Oracle: ""
      // PostgreSQL: ""
      // MySQL: ``
      // SQL Server: []
      return "\"" + std::get<std::string>(n->origin_value()) + "\"";
    };
    converters[SpatialPred][S_Contains] = [](auto n, auto c) -> std::string {
      return "ST_Contains(" + c.at(0) + "," + c.at(1) + ")";
    };
    converters[SpatialPred][S_Crosses] = [](auto n, auto c) -> std::string {
      return "ST_Crosses(" + c.at(0) + "," + c.at(1) + ")";
    };
    converters[SpatialPred][S_Disjoint] = [](auto n, auto c) -> std::string {
      return "ST_Disjoint(" + c.at(0) + "," + c.at(1) + ")";
    };
    converters[SpatialPred][S_Equals] = [](auto n, auto c) -> std::string {
      return "ST_Equals(" + c.at(0) + "," + c.at(1) + ")";
    };
    converters[SpatialPred][S_Intersects] = [](auto n, auto c) -> std::string {
      return "ST_Intersects(" + c.at(0) + "," + c.at(1) + ")";
    };
    converters[SpatialPred][S_Overlaps] = [](auto n, auto c) -> std::string {
      return "ST_Overlaps(" + c.at(0) + "," + c.at(1) + ")";
    };
    converters[SpatialPred][S_Touches] = [](auto n, auto c) -> std::string {
      return "ST_Touches(" + c.at(0) + "," + c.at(1) + ")";
    };
    converters[SpatialPred][S_Within] = [](auto n, auto c) -> std::string {
      return "ST_Within(" + c.at(0) + "," + c.at(1) + ")";
    };
    converters[ArithExpr][PLUS] = [](auto n, auto c) -> std::string {
      return c.at(0) + " + " + c.at(1);
    };
    converters[ArithExpr][MINUS] = [](auto n, auto c) -> std::string {
      return c.at(0) + " - " + c.at(1);
    };
    converters[ArithExpr][MULT] = [](auto n, auto c) -> std::string {
      return c.at(0) + " * " + c.at(1);
    };
    converters[ArithExpr][DIV] = [](auto n, auto c) -> std::string {
      return c.at(0) + " / " + c.at(1);
    };
    converters[ArithExpr][DIVINT] = [](auto n, auto c) -> std::string {
      return c.at(0) + " DIV " + c.at(1);
    };
    converters[ArithExpr][MOD] = [](auto n, auto c) -> std::string {
      return c.at(0) + " % " + c.at(1);
    };
    converters[ArithExpr][POWER] = [](auto n, auto c) -> std::string {
      return c.at(0) + " ^ " + c.at(1);
    };
    converters[IsLikePred][Like] = [](auto n, auto c) -> std::string {
      return c.at(0) + " LIKE " + c.at(1);
    };
    converters[IsLikePred][NotLike] = [](auto n, auto c) -> std::string {
      return c.at(0) + " LIKE " + c.at(1);
    };
    converters[IsNullPred][IsNull] = [](auto n, auto c) -> std::string {
      return c.at(0) + " IS NULL";
    };
    converters[IsNullPred][IsNotNull] = [](auto n, auto c) -> std::string {
      return c.at(0) + " IS NOT NULL";
    };
    converters[IsBetweenPred][Between] = [](auto n, auto c) -> std::string {
      return c.at(0) + " BETWEEN " + c.at(1) + " AND " + c.at(2);
    };
    converters[IsBetweenPred][NotBetween] = [](auto n, auto c) -> std::string {
      return c.at(0) + " NOT BETWEEN " + c.at(1) + " AND " + c.at(2);
    };
  }

  void Register(
      const std::map<NodeType, std::map<Operator, NodeConv>> evaluators) {
    converters.insert(evaluators.begin(), evaluators.end());
  }

  bool Convert(const AstNodePtr& root, std::string* sql_where) {
    if (converters.find(root->type()) == converters.end() ||
        converters.at(root->type()).find(root->op()) ==
            converters.at(root->type()).end()) {
      error_msg_ = "can not find sql converter for operator \"" +
                   OpName.at(root->op()) + "\" in node type \"" +
                   TypeName.at(root->type()) + "\"";
      return false;
    }

    std::vector<std::string> children_sql;
    for (AstNodePtr child : root->children()) {
      std::string sub_sql;
      if (Convert(child, &sub_sql))
        children_sql.emplace_back(sub_sql);
      else
        return false;
    }

    *sql_where = converters.at(root->type())
                     .at(root->op())
                     .
                     operator()(root, children_sql);

    return true;
  }
};

}  // namespace cql2cpp
