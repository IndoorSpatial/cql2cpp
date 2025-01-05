/*
 * File Name: cql2cpp.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/26
 *
 */

#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>

#include "ast_node.h"
#include "cql2_lexer.h"
#include "cql2_parser_text.h"
#include "evaluator.h"
#include "feature_source.h"
#include "global_yylex.h"
#include "tree_dot.h"

#ifndef VERSION
#define VERSION "0.0.0"
#endif

namespace cql2cpp {

template <typename FeatureType>
class Cql2Cpp {
 private:
  std::map<FeatureSourcePtr, FeatureType> feature_source_2_type_;
  std::ostream& ostr_;
  Evaluator evaluator_;

  mutable std::string error_msg_;

 public:
  Cql2Cpp() : ostr_(std::cout) {}

  Cql2Cpp(std::ostream& ostr) : ostr_(ostr) {}

  void set_feature_source(const std::map<FeatureSourcePtr, FeatureType> fs2t) {
    feature_source_2_type_ = fs2t;
  }

  void clear() { feature_source_2_type_.clear(); }

  void RegisterFunctor(const FunctorPtr functor) {
    evaluator_.RegisterFunctor(functor);
  }

  bool filter(const std::string& cql2_query,
              std::vector<FeatureType>* result) const {
    // Parse
    AstNodePtr root;
    if (not Parse(cql2_query, &root, &error_msg_)) return false;

    // Prepare evaluator
    ValueT value;

    // Loop over all features
    for (const auto& [fs, f] : feature_source_2_type_) {
      if (evaluator_.Evaluate(root, fs.get(), &value)) {
        if (std::holds_alternative<bool>(value)) {
          if (std::get<bool>(value)) result->emplace_back(f);
        } else {
          LOG(ERROR) << "evaluation result type error";
        }
      } else {
        LOG(ERROR) << "evaluation error: " << evaluator_.error_msg();
      }
    }

    return result;
  }

  const std::string error_msg() const { return error_msg_; }

  bool Evaluate(const std::string& cql2_query, const FeatureSource& fs,
                bool* result, std::string* error_msg, std::string* dot) {
    // Parse
    AstNodePtr root;
    if (not Parse(cql2_query, &root, error_msg)) return false;

    // Evaluate
    ValueT value;
    if (evaluator_.Evaluate(root, &fs, &value) &&
        std::holds_alternative<bool>(value)) {
      *result = std::get<bool>(value);
      if (dot != nullptr) {
        std::stringstream ss;
        Tree2Dot::GenerateDot(ss, root);
        *dot = ss.str();
      }
      return true;
    } else {
      if (error_msg != nullptr) *error_msg = evaluator_.error_msg();
      return false;
    }
  }

  static bool ToDot(const std::string& cql2_query, std::string* dot,
                    std::string* error_msg) {
    // Parse
    AstNodePtr root;
    if (not Parse(cql2_query, &root, error_msg)) return false;

    // to dot
    std::stringstream ss;
    Tree2Dot::GenerateDot(ss, root);
    *dot = ss.str();

    return true;
  }

  static std::string version() { return VERSION; }

 private:
  static bool Parse(const std::string& cql2_query, AstNodePtr* root,
                    std::string* error_msg) {
    std::istringstream iss(cql2_query);
    std::ostringstream oss;

    set_text_lexer(new Cql2Lexer(iss, oss));

    cql2cpp::AstNode::set_ostream(&oss);

    Cql2ParserText parser;
    int ret = parser.parse();
    if (error_msg != nullptr) *error_msg = oss.str();
    if (ret == 0) {
      *root = parser.root();
      return true;
    } else {
      return false;
    }
  }
};

}  // namespace cql2cpp

