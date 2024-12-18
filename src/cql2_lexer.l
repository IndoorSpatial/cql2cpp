%{
#include <math.h>
%}

%option noyywrap

DIGIT    [0-9]
ID       [a-z][a-z0-9]*

%%

{DIGIT}+    {
            printf( "An integer: %s (%d)\n", yytext,
                    atoi( yytext ) );
            }

{DIGIT}+"."{DIGIT}*        {
            printf( "A float: %s (%g)\n", yytext,
                    atof( yytext ) );
            }

if|then|begin|end|procedure|function        {
            printf( "A keyword: %s\n", yytext );
            }

{ID}        printf( "An identifier: %s\n", yytext );

"+"|"-"|"*"|"/"   printf( "An operator: %s\n", yytext );

"{"[^{}\n]*"}"     /* eat up one-line comments */

[ \t\n]+           /* eat up whitespace */

.           printf( "Unrecognized character: %s\n", yytext );

%%

void parse_all()
{
    FlexLexer* lexer = new yyFlexLexer();
    while(lexer->yylex() != 0);
}
