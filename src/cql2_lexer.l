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
  yyout << "CHAR_LIT " << yytext << std::endl;
  lval_->str = yytext;
  return Cql2ParserBase::CHAR_LIT;
}

[+-]?{DIGIT}+    {
  yyout << "NUMBER_INT " << yytext << std::endl;
  lval_->num_int = atoi(yytext);
  return Cql2ParserBase::NUMBER_INT;
}

[+-]?{DIGIT}+"."{DIGIT}* {
  yyout << "NUMBER_FLOAT " << yytext << std::endl;
  lval_->num_float = atof(yytext);
  return Cql2ParserBase::NUMBER_FLOAT;
}

[+-]?"."{DIGIT}* {
  yyout << "NUMBER_FLOAT " << yytext << std::endl;
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
  yyout << "LPT " << yytext << std::endl;
  return Cql2ParserBase::LPT;
}

")" {
  yyout << "RPT " << yytext << std::endl;
  return Cql2ParserBase::RPT;
}

"\"" {
  yyout << "DQUOTE " << yytext << std::endl;
  return Cql2ParserBase::DQUOTE;
}
"'" {
  yyout << "SQUOTE " << yytext << std::endl;
  return Cql2ParserBase::SQUOTE;
}

POINT|LINESTRING|POLYGON|MULTIPOINT|MULTILINESTRING|MULTIPOLYGON|GEOMETRYCOLLECTION {
  yyout << "TOKEN " << yytext << std::endl;
  lval_->str = yytext;
  return 0;
}

{ID} {
  yyout << "ID " << yytext << std::endl;
  lval_->str = yytext;
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
