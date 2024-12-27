%{
#include <cql2cpp/cql2_lexer-internal.h>
#include <cql2_parser_base.h>
%}

%option noyywrap

DIGIT    [0-9]
ID       [a-z][.a-z0-9_\-]*
CHAR_LIT \'.*\'

%%

{CHAR_LIT} {
  yyout << "TOKEN CHAR_LIT " << yytext << std::endl;
  token_texts_.emplace_back(yytext);
  lval_->str = token_texts_.back().c_str();
  return Cql2ParserBase::CHAR_LIT;
}

[+-]?{DIGIT}+    {
  yyout << "TOKEN NUMBER_INT " << yytext << std::endl;
  lval_->num_int = atoi(yytext);
  return Cql2ParserBase::NUMBER_INT;
}

[+-]?{DIGIT}+"."{DIGIT}* {
  yyout << "TOKEN NUMBER_FLOAT " << yytext << std::endl;
  lval_->num_float = atof(yytext);
  return Cql2ParserBase::NUMBER_FLOAT;
}

[+-]?"."{DIGIT}* {
  yyout << "TOKEN NUMBER_FLOAT " << yytext << std::endl;
  lval_->num_float = atof(yytext);
  return Cql2ParserBase::NUMBER_FLOAT;
}

"TRUE" {
  yyout << "TOKEN " << yytext << std::endl;
  lval_->boolean = true;
  return Cql2ParserBase::TRUE;
}
"FALSE" {
  yyout << "TOKEN " << yytext << std::endl;
  lval_->boolean = false;
  return Cql2ParserBase::FALSE;
}

"AND" {
  yyout << "TOKEN " << yytext << std::endl;
  return Cql2ParserBase::AND;
}

"OR" {
  yyout << "TOKEN " << yytext << std::endl;
  return Cql2ParserBase::OR;
}

"NOT" {
  yyout << "TOKEN " << yytext << std::endl;
  return Cql2ParserBase::NOT;
}

"CASEI" {
  yyout << "TOKEN " << yytext << std::endl;
  return Cql2ParserBase::CASEI;
}

"ACCENTI" {
  yyout << "TOKEN " << yytext << std::endl;
  return Cql2ParserBase::ACCENTI;
}

"(" {
  yyout << "TOKEN LPT " << yytext << std::endl;
  lval_->c = '(';
  return Cql2ParserBase::LPT;
}

")" {
  yyout << "TOKEN RPT " << yytext << std::endl;
  lval_->c = ')';
  return Cql2ParserBase::RPT;
}

"\"" {
  yyout << "TOKEN DQUOTE " << yytext << std::endl;
  return Cql2ParserBase::DQUOTE;
}
"'" {
  yyout << "TOKEN SQUOTE " << yytext << std::endl;
  return Cql2ParserBase::SQUOTE;
}

"," {
  yyout << "TOKEN COMMA " << yytext << std::endl;
  lval_->c = ',';
  return Cql2ParserBase::COMMA;
}


S_INTERSECTS|S_EQUALS|S_DISJOINT|S_TOUCHES|S_WITHIN|S_OVERLAPS|S_CROSSES|S_CONTAINS {
  yyout << "TOKEN " << yytext << std::endl;
  token_texts_.emplace_back(yytext);
  lval_->str = token_texts_.back().c_str();
  return Cql2ParserBase::SPT_FUNC;
}


POINT[Z]?"("[0-9. +\-\n]+")" {
  yyout << "TOKEN " << yytext << std::endl;
  token_texts_.emplace_back(yytext);
  lval_->str = token_texts_.back().c_str();
  return Cql2ParserBase::POINT_WKT;
}

LINESTRING[Z]?"("[0-9. +\-,\n]+")" {
  yyout << "TOKEN " << yytext << std::endl;
  token_texts_.emplace_back(yytext);
  lval_->str = token_texts_.back().c_str();
  return Cql2ParserBase::LINESTRING_WKT;
}

POLYGON[Z]?"(("[0-9. +\-,\n]+"))" {
  yyout << "TOKEN " << yytext << std::endl;
  token_texts_.emplace_back(yytext);
  lval_->str = token_texts_.back().c_str();
  return Cql2ParserBase::POLYGON_WKT;
}

BBOX"("[0-9. +\-,\n]+")" {
  yyout << "TOKEN " << yytext << std::endl;
  token_texts_.emplace_back(yytext);
  lval_->str = token_texts_.back().c_str();
  return Cql2ParserBase::BBOX_TEXT;
}

IN {
  yyout << "TOKEN " << yytext << std::endl;
  token_texts_.emplace_back(yytext);
  lval_->str = token_texts_.back().c_str();
  return Cql2ParserBase::IN;
}

{ID} {
  yyout << "TOKEN ID " << yytext << std::endl;
  token_texts_.emplace_back(yytext);
  lval_->str = token_texts_.back().c_str();
  return Cql2ParserBase::ID;
}

"+" { yyout << "TOKEN " << yytext << std::endl; return Cql2ParserBase::PLUS; }
"-" { yyout << "TOKEN " << yytext << std::endl; return Cql2ParserBase::MINUS;}
"*" { yyout << "TOKEN " << yytext << std::endl; return Cql2ParserBase::MULT; }
"/" { yyout << "TOKEN " << yytext << std::endl; return Cql2ParserBase::DIV;  }
"=" { yyout << "TOKEN " << yytext << std::endl; return Cql2ParserBase::EQ;  }
"<" { yyout << "TOKEN " << yytext << std::endl; return Cql2ParserBase::LT;  }
">" { yyout << "TOKEN " << yytext << std::endl; return Cql2ParserBase::GT;  }

"{"[^{}\n]*"}" {/* eat up one-line comments */}

[ \t\n]+ {/* eat up whitespace */}

. { yyout << "Unrecognized character: " << yytext << std::endl; }

%%
