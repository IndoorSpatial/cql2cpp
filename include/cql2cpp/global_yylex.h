/*
 * File Name: global_yylex.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/31
 *
 */

#pragma once

#include <cql2_parser_base.hh>

enum LexerInstance {
  CQL2_TEXT,
};

void set_current_lexer(LexerInstance lexer);

class Cql2Lexer;
void set_text_lexer(Cql2Lexer* lexer);

// bison expect a global yylex
// https://www.gnu.org/software/bison/manual/bison.html#C_002b_002b-Scanner-Interface
int yylex(cql2cpp::Cql2ParserBase::value_type* yylval);
