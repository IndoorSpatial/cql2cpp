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

#include <cql2_parser_base.h>

class Cql2Parser : public Cql2ParserBase {
 public:
  Cql2Parser() {}
  void yyerror(char*) override {}
  int yylex() override { return 0; }
};
