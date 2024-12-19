%{
#include <cql2cpp/cql2_lexer-internal.h>
%}

%option noyywrap

DIGIT    [0-9]
ID       [a-z][a-z0-9]*

%%

{DIGIT}+    {
  printf( "int: %s (%d)\n", yytext, atoi( yytext ) );
}

{DIGIT}+"."{DIGIT}* {
  printf( "float: %s (%g)\n", yytext, atof( yytext ) );
}

TRUE|FALSE {
  printf("bool %s\n", yytext);
}

POINT|LINESTRING|POLYGON|MULTIPOINT|MULTILINESTRING|MULTIPOLYGON|GEOMETRYCOLLECTION {
  printf( "geom: %s\n", yytext );
}

{ID} {
  printf( "identifier: %s\n", yytext );
}

"+"|"-"|"*"|"/" {
  printf( "An operator: %s\n", yytext );
}

"{"[^{}\n]*"}" {/* eat up one-line comments */}

[ \t\n]+ {/* eat up whitespace */}

. { printf( "Unrecognized character: %s\n", yytext ); }

%%
