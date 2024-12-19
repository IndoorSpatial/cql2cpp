/*
 * File Name: main.cc
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/18
 *
 */

#include <cql2cpp/cql2_lexer.h>
#include <cql2cpp/cql2_parser.h>

int main(int argc, char** argv) {
  Cql2Lexer* lexer = new Cql2Lexer();
  Cql2Parser parser(lexer);
  lexer->RegisterLval(&parser.yylval);

  parser.yyparse();

  return 0;
}
