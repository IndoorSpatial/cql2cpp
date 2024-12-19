%{
#include <iostream>
#include <cstdlib>

using namespace std;

extern int yylex();  // Declare the lexer function
extern void yyerror(const char *s);  // Declare the error handler

#define YY_Cql2ParserBase_ERROR_BODY   {}
#define YY_Cql2ParserBase_LEX_BODY   {}

%}

%name Cql2ParserBase

%union {
    int num;  // Union for holding integers (for number tokens)
}

%token <num> NUMBER  // Declare token type as 'NUMBER'
%token PLUS MINUS MULT DIV  // Declare operator tokens

%type <num> expr  // Declare the type of 'expr' as 'int'

%left PLUS MINUS
%left MULT DIV

%%

// Grammar rules:

program:
    expr { cout << "Result = " << $1 << endl; }  // Print the result
;

expr:
    NUMBER { $$ = $1; }
    | expr PLUS expr { $$ = $1 + $3; }
    | expr MINUS expr { $$ = $1 - $3; }
    | expr MULT expr { $$ = $1 * $3; }
    | expr DIV expr { $$ = $1 / $3; }
;

%%

// Error handling function (C++ version)
void yyerror(const char *s) {
    cerr << "Error: " << s << endl;
}

