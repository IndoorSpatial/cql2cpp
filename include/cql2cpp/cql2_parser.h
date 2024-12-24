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

#include <cql2_parser_base.h>

class Cql2Parser : public Cql2ParserBase {
 private:
  FlexLexer* lexer_;

 public:
  Cql2Parser(FlexLexer* lexer) : lexer_(lexer) {
    yydebug = false;
  }

  void yyerror(char* s) override {
    std::cerr << "Cql2Parser Error: " << s << std::endl;
  }

  int yylex() override { return lexer_->yylex(); }

  cql2cpp::AstNode* root() const { return root_; }
};
