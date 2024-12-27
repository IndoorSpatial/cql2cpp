%{
#include <iostream>
#include <cstdlib>
#include <geos/io/WKTReader.h>
#include <cql2cpp/ast_node.h>
#include <cql2cpp/bbox_reader.h>

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
using cql2cpp::SpatialPred;
using cql2cpp::PropertyName;
using cql2cpp::NameOp;

%}

%header{

#include <geos/geom/Geometry.h>
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
  const char* str;
  char c;
  cql2cpp::AstNode* node;
  geos::geom::Geometry* geom;
  geos::geom::Envelope* env;
}

%token <num_int> NUMBER_INT
%token <num_float> NUMBER_FLOAT
%token <boolean> BOOLEAN
%token <boolean> TRUE FALSE
%token <str> ID
%token <str> CHAR_LIT
%token <str> SPT_FUNC
%token PLUS MINUS MULT DIV
%token EQ GT LT  // = > <
%token AND OR NOT
%token <c> LPT RPT COMMA  // ( ) ,
%token CASEI ACCENTI
%token SQUOTE DQUOTE

%token <str> POINT_WKT
%token <str> LINESTRING_WKT
%token <str> POLYGON_WKT
%token <str> BBOX_TEXT

%type <node> booleanExpression
%type <node> booleanTerm
%type <node> booleanFactor
%type <node> booleanPrimary
%type <node> booleanLiteral
// %type <node> characterExpression
// %type <node> characterClause
%type <node> propertyName
%type <node> predicate
%type <node> comparisonPredicate
%type <node> binaryComparisonPredicate
%type <node> scalarExpression
%type <node> numericLiteral
%type <node> spatialPredicate
%type <node> geomExpression
%type <node> spatialInstance
%type <str> geometryLiteral
%type <str> pointTaggedText
%type <str> linestringTaggedText
%type <str> polygonTaggedText
%type <str> bboxTaggedText

%left PLUS MINUS
%left MULT DIV
%left NOT AND OR

%define parse.trace
%%

// Grammar rules:

program:
       booleanExpression { root_ = $1 }

booleanExpression:
  booleanTerm
  | booleanTerm OR booleanExpression { $$ = new AstNode(BoolExpression, Or, {$1, $3}); }

booleanTerm:
  booleanFactor
  | booleanFactor AND booleanTerm  { $$ = new AstNode(BoolExpression, And, {$1, $3}); }

booleanFactor:
  booleanPrimary
  | NOT booleanPrimary { $$ = new AstNode(BoolExpression, Not, { $2 }); }

booleanPrimary:
  predicate
  | booleanLiteral
  | LPT booleanExpression RPT { $$ = $2 }

booleanLiteral:
  TRUE { $$ = new AstNode($1); }
  | FALSE { $$ = new AstNode($1); }

predicate:
  comparisonPredicate
  | spatialPredicate

comparisonPredicate:
  binaryComparisonPredicate

binaryComparisonPredicate:
  scalarExpression EQ scalarExpression { $$ = new AstNode(BinCompPred, cql2cpp::Equal, {$1, $3}); }
  | scalarExpression LT GT scalarExpression { $$ = new AstNode(BinCompPred, cql2cpp::NotEqual, {$1, $4}); }
  | scalarExpression LT    scalarExpression { $$ = new AstNode(BinCompPred, cql2cpp::Lesser,   {$1, $3}); }
  | scalarExpression GT    scalarExpression { $$ = new AstNode(BinCompPred, cql2cpp::Greater,  {$1, $3}); }
  | scalarExpression LT EQ scalarExpression { $$ = new AstNode(BinCompPred, cql2cpp::LesserEqual, {$1, $4}); }
  | scalarExpression GT EQ scalarExpression { $$ = new AstNode(BinCompPred, cql2cpp::GreaterEqual, {$1, $4}) }

scalarExpression:
  numericLiteral
  | propertyName
//| booleanLiteral
//| characterClause

// characterClause:
//   CASEI LPT characterExpression RPT
//   | ACCENTI LPT characterExpression RPT
//   | CHAR_LIT { std::string s = std::string($1); $$ = new AstNode(s.substr(1, s.size() - 2)); }

// characterExpression:
//   characterClause
//   | propertyName { $$ = $1; }

numericLiteral:
  NUMBER_INT { $$ = new AstNode($1); }
  | NUMBER_FLOAT { $$ = new AstNode($1); }

propertyName:
  ID { $$ = new AstNode(PropertyName, std::string($1)); }
  // | DQUOTE ID DQUOTE;

spatialPredicate:
  SPT_FUNC LPT geomExpression COMMA geomExpression RPT { $$ = new AstNode(SpatialPred, NameOp.at($1), {$3, $5}); }


geomExpression:
  spatialInstance
  | propertyName
//| function


spatialInstance:
  geometryLiteral {
    auto p = geos::io::WKTReader().read($1);
    if (p) {
      $$ = new AstNode(p.release());
    } else {
      yyerror("Can not parse WKT");
      YYERROR;
    }
  }
  | bboxTaggedText {
    auto p = cql2cpp::BBoxReader().read($1);
    if (p) {
      $$ = new AstNode(p.release());
    } else {
      yyerror("Can not parse BBOX");
      YYERROR;
    }
  }


geometryLiteral:
  pointTaggedText
  | linestringTaggedText
  | polygonTaggedText

pointTaggedText: POINT_WKT

linestringTaggedText: LINESTRING_WKT

polygonTaggedText: POLYGON_WKT

bboxTaggedText: BBOX_TEXT

%%
