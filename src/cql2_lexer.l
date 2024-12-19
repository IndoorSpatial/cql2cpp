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
  return Cql2ParserBase::NUMBER_INT;
}

TRUE|FALSE {
  printf("bool %s\n", yytext);
  return 0;
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

"{"[^{}\n]*"}" {/* eat up one-line comments */}

[ \t\n]+ {/* eat up whitespace */}

. { printf( "Unrecognized character: %s\n", yytext ); }

%%
