%{
#include <cql2cpp/cql2_lexer-internal.h>
#include <cql2_parser_base.h>
%}

%option noyywrap

DIGIT    [0-9]
ID       [a-z][a-z0-9]*

%%

{DIGIT}+    {
  printf( "int: \"%s\" (%d)\n", yytext, atoi( yytext ) );
  lval_->num_int = atoi(yytext);
  return Cql2ParserBase::NUMBER_INT;
}

{DIGIT}+"."{DIGIT}* {
  printf( "float: \"%s\" (%g)\n", yytext, atof( yytext ) );
  lval_->num_float = atof(yytext);
  return Cql2ParserBase::NUMBER_INT;
}

"TRUE" {
  printf("bool %s\n", yytext);
  lval_->boolean = true;
  return Cql2ParserBase::TRUE;
}
"FALSE" {
  printf("bool %s\n", yytext);
  lval_->boolean = false;
  return Cql2ParserBase::FALSE;
}

"AND" {
  printf("logical %s\n", yytext);
  lval_->str = yytext;
  return Cql2ParserBase::AND;
}

"OR" {
  printf("logical %s\n", yytext);
  lval_->str = yytext;
  return Cql2ParserBase::OR;
}

"(" {
  printf("parenthesis %s\n", yytext);
  lval_->c = '(';
  return Cql2ParserBase::LPT;
}

")" {
  printf("parenthesis %s\n", yytext);
  lval_->c = ')';
  return Cql2ParserBase::RPT;
}

POINT|LINESTRING|POLYGON|MULTIPOINT|MULTILINESTRING|MULTIPOLYGON|GEOMETRYCOLLECTION {
  printf( "geom: %s\n", yytext );
  return 0;
}

{ID} {
  printf( "identifier: %s\n", yytext );
  return 0;
}

"+" { printf( "op: %s\n", yytext ); return Cql2ParserBase::PLUS; }
"-" { printf( "op: %s\n", yytext ); return Cql2ParserBase::MINUS;}
"*" { printf( "op: %s\n", yytext ); return Cql2ParserBase::MULT; }
"/" { printf( "op: %s\n", yytext ); return Cql2ParserBase::DIV;  }

"=" { printf( "comp: %s\n", yytext ); return Cql2ParserBase::EQ;  }
"<" { printf( "comp: %s\n", yytext ); return Cql2ParserBase::LT;  }
">" { printf( "comp: %s\n", yytext ); return Cql2ParserBase::GT;  }

"{"[^{}\n]*"}" {/* eat up one-line comments */}

[ \t\n]+ {/* eat up whitespace */}

. { printf( "Unrecognized character: %s\n", yytext ); }

%%
