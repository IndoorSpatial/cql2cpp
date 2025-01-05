/*
 * File Name: cql2_parser_json.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/19
 *
 */

#pragma once

#include <geos/vend/include_nlohmann_json.hpp>

#include "ast_node.h"

namespace cql2cpp {

class Cql2ParserJson {
 private:
  AstNodePtr root_;

 public:
  Cql2ParserJson() {}

  AstNodePtr parse(const geos_nlohmann::json& j) {
    AstNodePtr node;
    std::vector<geos_nlohmann::json> children;
    if (parse(j, &node, &children))
      for (const auto& child_j : children) {
        AstNodePtr child_node = parse(child_j);
        if (child_node != nullptr)
          node->append(child_node);
        else
          return nullptr;
      }

    return node;
  }

  AstNodePtr root() const { return root_; }

 private:
  bool parse(const geos_nlohmann::json& j, AstNodePtr* node,
             std::vector<geos_nlohmann::json>* children);
};

class JsonParser {
 public:
  virtual bool detect(const geos_nlohmann::json& j) = 0;
  virtual bool parse(const geos_nlohmann::json& j, AstNodePtr* node,
                     std::vector<geos_nlohmann::json>* children) = 0;
};

class JsonParserOperator : public JsonParser {
 public:
  JsonParserOperator() {}

  bool detect(const geos_nlohmann::json& j) override {
    return j.contains("op");
  }

  bool parse(const geos_nlohmann::json& j, AstNodePtr* node,
             std::vector<geos_nlohmann::json>* children) override {
    return false;
  }
};

}  // namespace cql2cpp
