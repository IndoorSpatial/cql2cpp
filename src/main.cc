/*
 * File Name: main.cc
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/18
 *
 */

#include <cql2_lexer.h>

int main(int argc, char** argv) {
  FlexLexer* lexer = new Cql2Lexer();
  while (lexer->yylex() != 0)
    ;
  return 0;
}
