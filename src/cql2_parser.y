%{
#include <iostream>
#include <cstdlib>

extern int yylex();  // Declare the lexer function
extern void yyerror(const char *s);  // Declare the error handler

#define YY_Cql2ParserBase_ERROR_BODY   {}
#define YY_Cql2ParserBase_LEX_BODY   {}

%}

%name Cql2ParserBase

%union {
  int num_int;  // Union for holding integers (for number tokens)
  double num_double;
  char* str;
}

%token <num_int> NUMBER_INT
%token PLUS MINUS MULT DIV

%type <num_int> expr  // Declare the type of 'expr' as 'int'

%left PLUS MINUS
%left MULT DIV

%%

// Grammar rules:

program:
    expr { std::cout << "Result = " << $1 << std::endl; }  // Print the result
;

expr:
    NUMBER_INT { $$ = $1; }
    | expr PLUS expr { $$ = $1 + $3; std::cout << $$ << "=" << $1 << "+" << $3 << std::endl; }
    | expr MINUS expr { $$ = $1 - $3; }
    | expr MULT expr { $$ = $1 * $3; }
    | expr DIV expr { $$ = $1 / $3; }
;

%%

// Error handling function (C++ version)
void yyerror(const char *s) {
    std::cerr << "Error: " << s << std::endl;
}

