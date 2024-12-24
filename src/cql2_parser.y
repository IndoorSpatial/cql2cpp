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
using cql2cpp::BinCompPred;
using cql2cpp::PropertyName;

%}

%header{

#include <cql2cpp/ast_node.h>
#define YY_Cql2ParserBase_MEMBERS cql2cpp::AstNode* root_;
#define YY_Cql2ParserBase_DEBUG 1
#define YYDEBUG

%}

%name Cql2ParserBase

%union {
  bool boolean;
  int num_int;
  double num_float;
  char* str;
  char c;
  cql2cpp::AstNode* node;
}

%token <num_int> NUMBER_INT
%token <num_float> NUMBER_FLOAT
%token <boolean> BOOLEAN
%token <boolean> TRUE FALSE
%token <str> ID
%token <str> CHAR_LIT
%token PLUS MINUS MULT DIV
%token EQ GT LT  // = > <
%token AND OR NOT
%token LPT RPT  // ( )
%token CASEI ACCENTI
%token SQUOTE DQUOTE

%type <node> booleanExpression
%type <node> booleanTerm
%type <node> booleanFactor
%type <node> booleanPrimary
%type <node> booleanLiteral
%type <node> characterExpression
%type <node> characterClause
%type <node> propertyName
%type <node> predicate
%type <node> comparisonPredicate
%type <node> binaryComparisonPredicate
%type <node> scalarExpression
%type <node> numericLiteral

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
  predicate
  | booleanLiteral { $$ = $1; }
  | LPT booleanExpression RPT { $$ = $2; }

booleanLiteral:
  TRUE { $$ = new AstNode($1); }
  | FALSE { $$ = new AstNode($1); }

predicate:
  comparisonPredicate { $$ = $1; }

comparisonPredicate:
  binaryComparisonPredicate { $$ = $1; }

binaryComparisonPredicate:
  scalarExpression EQ scalarExpression { $$ = new AstNode(BinCompPred, cql2cpp::Equal, {$1, $3}); }
  | scalarExpression LT GT scalarExpression { $$ = new AstNode(BinCompPred, cql2cpp::NotEqual, {$1, $4}); }
  | scalarExpression LT    scalarExpression { $$ = new AstNode(BinCompPred, cql2cpp::Lesser,   {$1, $3}); }
  | scalarExpression GT    scalarExpression { $$ = new AstNode(BinCompPred, cql2cpp::Greater,  {$1, $3}); }
  | scalarExpression LT EQ scalarExpression { $$ = new AstNode(BinCompPred, cql2cpp::LesserEqual, {$1, $4}); }
  | scalarExpression GT EQ scalarExpression { $$ = new AstNode(BinCompPred, cql2cpp::GreaterEqual, {$1, $4}); }

scalarExpression:
  characterClause
  | numericLiteral { $$ = $1; }
  | booleanLiteral
  | propertyName { $$ = $1; }

characterClause:
  CASEI LPT characterExpression RPT
  | ACCENTI LPT characterExpression RPT
  | CHAR_LIT { std::string s = std::string($1); $$ = new AstNode(s.substr(1, s.size() - 2)); }

characterExpression:
  characterClause
  | propertyName { $$ = $1; }

numericLiteral:
  NUMBER_INT { $$ = new AstNode($1); }
  | NUMBER_FLOAT { $$ = new AstNode($1); }

propertyName:
  ID { $$ = new AstNode(PropertyName, { std::string($1) }); }
  | DQUOTE ID DQUOTE;

%%
