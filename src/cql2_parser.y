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
  bool boolean;
  int num_int;  // Union for holding integers (for number tokens)
  double num_float;
  char* str;
  char c;
}

%token <num_int> NUMBER_INT
%token <boolean> BOOLEAN
%token PLUS MINUS MULT DIV
%token EQ GT LT  // = > <
%token AND OR NOT
%token TRUE FALSE
%token LPT RPT  // ( )

%type <num_int> IntExpression
%type <boolean> booleanExpression
%type <boolean> booleanTerm
%type <boolean> booleanFactor
%type <boolean> booleanPrimary
%type <boolean> booleanLiteral
%type <boolean> TRUE
%type <boolean> FALSE

%left PLUS MINUS
%left MULT DIV
%left NOT AND OR

%%

// Grammar rules:

program:
    booleanExpression { std::cout << "Result = " << ($1 ? "T" : "F") << std::endl; }
;

booleanExpression:
  booleanTerm { $$ = $1 }
  | booleanTerm OR booleanExpression { $$ = $1 || $3 }

booleanTerm:
  booleanFactor { $$ = $1 }
  | booleanFactor AND booleanTerm  { $$ = $1 && $3 }

booleanFactor:
  booleanPrimary { $$ = $1; }
  | NOT booleanPrimary { $$ = ! $2 }

booleanPrimary:
  booleanLiteral { $$ = $1; }
  | LPT booleanExpression RPT { $$ = $2; }

booleanLiteral:
  TRUE { $$ = $1; }
  | FALSE { $$ = $1; }


IntExpression:
  NUMBER_INT { $$ = $1; }
  | IntExpression PLUS  IntExpression { $$ = $1 + $3; std::cout << $$ << "=" << $1 << "+" << $3 << std::endl; }
  | IntExpression MINUS IntExpression { $$ = $1 - $3; std::cout << $$ << "=" << $1 << "-" << $3 << std::endl; }
  | IntExpression MULT  IntExpression { $$ = $1 * $3; std::cout << $$ << "=" << $1 << "*" << $3 << std::endl; }
  | IntExpression DIV   IntExpression { $$ = $1 / $3; std::cout << $$ << "=" << $1 << "/" << $3 << std::endl; }
;

%%
