%{
#include <cql2_parser_base.hh>
#include <cql2cpp/cql2_lexer-internal.h>

using cql2cpp::Cql2ParserBase;

void print(const std::string& prefix, const char* yytext) {
  LOG(INFO) << "TOKEN " << prefix << " \"" << yytext << "\"";
}

%}

%option noyywrap

DIGIT    [0-9]
ID       [a-z][.a-z0-9_\-]*
CHAR_LIT \'[^\']*\'

%%

{CHAR_LIT} {
  print("CHAR_LIT", yytext);
  yylval_->emplace<std::string>() = yytext;
  return Cql2ParserBase::token::CHAR_LIT;
}

[+-]?{DIGIT}+    {
  print("DIGIT_INT", yytext);
  yylval_->emplace<int>() = atoi(yytext);
  return Cql2ParserBase::token::NUMBER_INT;
}

[+-]?{DIGIT}+"."{DIGIT}* {
  print("DIGIT_FLOAT", yytext);
  yylval_->emplace<double>() = atof(yytext);
  return Cql2ParserBase::token::NUMBER_FLOAT;
}

[+-]?"."{DIGIT}* {
  print("DIGIT_FLOAT", yytext);
  yylval_->emplace<double>() = atof(yytext);
  return Cql2ParserBase::token::NUMBER_FLOAT;
}

"TRUE" {
  print("BOOL_LIT", yytext);
  yylval_->emplace<bool>() = true;
  return Cql2ParserBase::token::TRUE;
}
"FALSE" {
  print("BOOL_LIT", yytext);
  yylval_->emplace<bool>() = false;
  return Cql2ParserBase::token::FALSE;
}

"AND" {
  print("BOOL_EXPR", yytext);
  return Cql2ParserBase::token::AND;
}

"OR" {
  print("BOOL_EXPR", yytext);
  return Cql2ParserBase::token::OR;
}

"NOT" {
  print("BOOL_EXPR", yytext);
  return Cql2ParserBase::token::NOT;
}

"CASEI" {
  print("", yytext);
  return Cql2ParserBase::token::CASEI;
}

"ACCENTI" {
  print("", yytext);
  return Cql2ParserBase::token::ACCENTI;
}

"(" {
  print("LPT", yytext);
  return Cql2ParserBase::token::LPT;
}

")" {
  print("RPT", yytext);
  return Cql2ParserBase::token::RPT;
}

"\"" {
  print("DQUOTE", yytext);
  return Cql2ParserBase::token::DQUOTE;
}
"'" {
  print("SQUOTE", yytext);
  return Cql2ParserBase::token::SQUOTE;
}

"," {
  print("COMMA", yytext);
  return Cql2ParserBase::token::COMMA;
}


S_INTERSECTS|S_EQUALS|S_DISJOINT|S_TOUCHES|S_WITHIN|S_OVERLAPS|S_CROSSES|S_CONTAINS {
  print("SPATIAL", yytext);
  yylval_->emplace<std::string>(yytext);
  return Cql2ParserBase::token::SPT_FUNC;
}


POINT[Z]?"("[0-9. +\-\n]+")" {
  print("GEOMETRY", yytext);
  yylval_->emplace<std::string>() = yytext;
  return Cql2ParserBase::token::POINT_WKT;
}

LINESTRING[Z]?"("[0-9. +\-,\n]+")" {
  print("GEOMETRY", yytext);
  yylval_->emplace<std::string>() = yytext;
  return Cql2ParserBase::token::LINESTRING_WKT;
}

POLYGON[Z]?"(("[0-9. +\-,\n]+"))" {
  print("GEOMETRY", yytext);
  yylval_->emplace<std::string>() = yytext;
  return Cql2ParserBase::token::POLYGON_WKT;
}

BBOX"("[0-9. +\-,\n]+")" {
  print("BBOX", yytext);
  yylval_->emplace<std::string>() = yytext;
  return Cql2ParserBase::token::BBOX_TEXT;
}

IN {
  print("IN", yytext);
  return Cql2ParserBase::token::IN;
}

{ID} {
  print("ID", yytext);
  yylval_->emplace<std::string>(yytext);
  return Cql2ParserBase::token::ID;
}

"+" { print("OP", yytext); return Cql2ParserBase::token::PLUS; }
"-" { print("OP", yytext); return Cql2ParserBase::token::MINUS;}
"*" { print("OP", yytext); return Cql2ParserBase::token::MULT; }
"/" { print("OP", yytext); return Cql2ParserBase::token::DIV;  }
"=" { print("OP", yytext); return Cql2ParserBase::token::EQ;  }
"<" { print("OP", yytext); return Cql2ParserBase::token::LT;  }
">" { print("OP", yytext); return Cql2ParserBase::token::GT;  }

"{"[^{}\n]*"}" {/* eat up one-line comments */}

[ \t\n]+ {/* eat up whitespace */}

. { LOG(INFO) << "Unrecognized character: " << yytext << std::endl; }

%%
