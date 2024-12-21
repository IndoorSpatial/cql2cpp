%{
#include <iostream>
#include <cstdlib>
#include <cql2cpp/ast_node.h>

extern int yylex();  // Declare the lexer function
extern void yyerror(const char *s);  // Declare the error handler

#define YY_Cql2ParserBase_ERROR_BODY   {}
#define YY_Cql2ParserBase_LEX_BODY   { return 0; }

using cql2cpp::AstNode;
using cql2cpp::Not;
using cql2cpp::And;
using cql2cpp::Or;
using cql2cpp::BoolExpression;

%}

%header{

#include <cql2cpp/ast_node.h>
#define YY_Cql2ParserBase_MEMBERS cql2cpp::AstNode* root_;

%}

%name Cql2ParserBase

%union {
  bool boolean;
  int num_int;  // Union for holding integers (for number tokens)
  double num_float;
  char* str;
  char c;
  cql2cpp::AstNode* node;
}

%token <num_int> NUMBER_INT
%token <boolean> BOOLEAN
%token PLUS MINUS MULT DIV
%token EQ GT LT  // = > <
%token AND OR NOT
%token TRUE FALSE
%token LPT RPT  // ( )

%type <num_int> IntExpression
%type <node> booleanExpression
%type <node> booleanTerm
%type <node> booleanFactor
%type <node> booleanPrimary
%type <node> booleanLiteral
%type <boolean> TRUE
%type <boolean> FALSE

%left PLUS MINUS
%left MULT DIV
%left NOT AND OR

%%

// Grammar rules:

program:
       booleanExpression { root_ = $1; }

booleanExpression:
  booleanTerm { $$ = $1 }
  | booleanTerm OR booleanExpression { $$ = new AstNode(BoolExpression, Or, {$1, $3}); }

booleanTerm:
  booleanFactor { $$ = $1 }
  | booleanFactor AND booleanTerm  { $$ = new AstNode(BoolExpression, And, {$1, $3}); }

booleanFactor:
  booleanPrimary { $$ = $1; }
  | NOT booleanPrimary { $$ = new AstNode(BoolExpression, Not, { $2 }); }

booleanPrimary:
  booleanLiteral { $$ = $1; }
  | LPT booleanExpression RPT { $$ = $2; }

booleanLiteral:
  TRUE { $$ = new AstNode($1); }
  | FALSE { $$ = new AstNode($1); }


IntExpression:
  NUMBER_INT { $$ = $1; }
  | IntExpression PLUS  IntExpression { $$ = $1 + $3; std::cout << $$ << "=" << $1 << "+" << $3 << std::endl; }
  | IntExpression MINUS IntExpression { $$ = $1 - $3; std::cout << $$ << "=" << $1 << "-" << $3 << std::endl; }
  | IntExpression MULT  IntExpression { $$ = $1 * $3; std::cout << $$ << "=" << $1 << "*" << $3 << std::endl; }
  | IntExpression DIV   IntExpression { $$ = $1 / $3; std::cout << $$ << "=" << $1 << "/" << $3 << std::endl; }
;

%%
