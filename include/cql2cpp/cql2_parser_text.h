/*
 * File Name: cql2_parser_text.h
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

namespace cql2cpp {

class Cql2ParserText : public Cql2ParserBase {
 private:
  AstNodePtr root_;

 public:
  Cql2ParserText() : Cql2ParserBase(&root_) {}

  void error(const std::string& msg) override {
    LOG(ERROR) << "Cql2ParserText Error: " << msg << std::endl;
  }

  int parse() override { return Cql2ParserBase::parse(); }

  AstNodePtr root() const { return root_; }
};

}  // namespace cql2cpp
