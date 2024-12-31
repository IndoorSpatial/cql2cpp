/*
 * File Name: cql2_parser.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/19
 *
 */

#pragma once

#include <FlexLexer.h>
#include <glog/logging.h>

#include <cql2_parser_base.hh>

class Cql2Parser : public cql2cpp::Cql2ParserBase {
 private:
  cql2cpp::AstNode* root_;

 public:
  Cql2Parser() : cql2cpp::Cql2ParserBase(&root_) {}

  void error(const std::string& msg) override {
    LOG(ERROR) << "Cql2Parser Error: " << msg << std::endl;
  }

  cql2cpp::AstNode* root() const { return root_; }

  void DeConstructRoot() {
    // TODO
  }

  static void DeConstructAST(cql2cpp::AstNode*) {
    // TODO
  }
};
