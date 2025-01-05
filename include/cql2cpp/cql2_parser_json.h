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

#include "ast_node.h"

namespace cql2cpp {

class Cql2ParserJson {
 private:
  AstNodePtr root_;

 public:
  Cql2ParserJson() {}

  bool parse() { return true; }

  AstNodePtr root() const { return root_; }
};

}  // namespace cql2cpp
