/*
 * File Name: tree_dot.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/20
 *
 */

#pragma once

#include <ostream>

#include "ast_node.h"

namespace cql2cpp {

class Tree2Dot {
 public:
  static std::string node_name(const AstNode* node) {
    if (node->op() != NullOp)
      return OpName.at(node->op()) + "(" + value_str(node->value()) + ")";
    else if (node->type() == PropertyName)
      return std::string("property") + "(" + value_str(node->value()) + ")";
    else
      return value_str(node->value());
  }

  static std::string node_value_str(const AstNode* node) {
    return value_str(node->value());
  }

  static bool GenerateDot(std::ostream& ous, const AstNode* node) {
    ous << "digraph G {" << std::endl;
    GenerateDotNode(ous, node);
    ous << std::endl;
    GenerateDotEdge(ous, node);
    ous << "}" << std::endl;
    return true;
  }

  static bool GenerateDotNode(std::ostream& ous, const AstNode* node) {
    if (node == nullptr) return true;

    ous << "  \"" << node->id() << "\" [label=\"" << node->id() << " "
        << node_name(node) << "\"];" << std::endl;
    for (const auto child : node->children()) GenerateDotNode(ous, child);

    return true;
  }

  static bool GenerateDotEdge(std::ostream& ous, const AstNode* node) {
    if (node == nullptr) return true;

    for (const auto child : node->children()) {
      ous << "  \"" << node->id() << "\" -> \"" << child->id() << "\";\n";
      GenerateDotEdge(ous, child);
    }

    return true;
  }
};
}  // namespace cql2cpp
