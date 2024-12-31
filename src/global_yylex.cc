/*
 * File Name: global_yylex.cc
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/31
 *
 */
#include <cql2cpp/cql2_lexer.h>
#include <cql2cpp/global_yylex.h>

LexerInstance global_lexer_instance = CQL2_TEXT;

void set_current_lexer(LexerInstance lexer) { global_lexer_instance = lexer; }

static Cql2Lexer* lexer_text = nullptr;
void set_text_lexer(Cql2Lexer* lexer) {
  if (lexer_text) delete lexer_text;
  lexer_text = lexer;
}

int yylex(cql2cpp::Cql2ParserBase::value_type* yylval) {
  if (global_lexer_instance == CQL2_TEXT) {
    lexer_text->set_yylval(yylval);
    return lexer_text->yylex();
  }

  return -1;
}

