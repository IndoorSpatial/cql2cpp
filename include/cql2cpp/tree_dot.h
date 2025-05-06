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
  static std::string node_name(const AstNodePtr node) {
    if (node->op() != NullOp)
      return OpName.at(node->op());
    else
      return TypeName.at(node->type());
  }

  static bool GenerateDot(std::ostream& ous, const AstNodePtr node) {
    ous << "digraph G {" << std::endl;
    GenerateDotNode(ous, node);
    ous << std::endl;
    GenerateDotEdge(ous, node);
    ous << "}" << std::endl;
    return true;
  }

  static bool GenerateDotNode(std::ostream& ous, const AstNodePtr node) {
    if (node == nullptr) return true;

    ous << "  \"" << node->id() << "\" [label=\"" << node->id() << ". "
        << node_name(node) << "("
        << value_str(node->origin_value(), node->value()) << ")"
        << "\"];" << std::endl;
    for (const auto& child : node->children()) GenerateDotNode(ous, child);

    return true;
  }

  static bool GenerateDotEdge(std::ostream& ous, const AstNodePtr node) {
    if (node == nullptr) return true;

    for (const auto& child : node->children()) {
      if (child == nullptr) {
        LOG(ERROR) << "corrupt AST";
        ous << "  \"" << node->id() << "\" -> \"null ptr\";\n";
      } else {
        ous << "  \"" << node->id() << "\" -> \"" << child->id() << "\";\n";
      }
      GenerateDotEdge(ous, child);
    }

    return true;
  }
};
}  // namespace cql2cpp
