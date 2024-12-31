%code {
#include <iostream>
#include <cstdlib>
#include <geos/io/WKTReader.h>
#include <cql2cpp/ast_node.h>
#include <cql2cpp/bbox_reader.h>

using cql2cpp::AstNode;

using cql2cpp::BoolExpression;
using cql2cpp::BinCompPred;
using cql2cpp::SpatialPred;
using cql2cpp::PropertyName;
using cql2cpp::IsInListPred;
using cql2cpp::InList;
using cql2cpp::ArrayPred;
using cql2cpp::Array;
using cql2cpp::Function;
using cql2cpp::ArgumentList;

using cql2cpp::NullOp;
using cql2cpp::Not;
using cql2cpp::And;
using cql2cpp::Or;
using cql2cpp::In;
using cql2cpp::NotIn;

using cql2cpp::NameOp;

void cql2cpp::Cql2ParserBase::error(const std::string& msg) {
  LOG(ERROR) << msg;
}

}

%code requires {

#include <geos/geom/Geometry.h>
#include <cql2cpp/ast_node.h>

}

%code provides {

#include <cql2cpp/global_yylex.h>

}

%language "C++"
%define api.namespace {cql2cpp}
%define api.parser.class {Cql2ParserBase}
%define api.value.type variant
%define parse.error verbose
%parse-param {cql2cpp::AstNode **root_}

%token <int> NUMBER_INT
%token <double> NUMBER_FLOAT
%token <bool> BOOLEAN
%token <bool> TRUE FALSE
%token <std::string> ID
%token <std::string> CHAR_LIT
%token <std::string> SPT_FUNC
%token <std::string> arrayFunction
%token PLUS MINUS MULT DIV
%token EQ GT LT  // = > <
%token AND OR NOT
%token IN
%token <char> LPT RPT COMMA  // ( ) ,
%token CASEI ACCENTI
%token SQUOTE DQUOTE

%token <std::string> POINT_WKT
%token <std::string> LINESTRING_WKT
%token <std::string> POLYGON_WKT
%token <std::string> BBOX_TEXT

%type <AstNode*> booleanExpression
%type <AstNode*> booleanTerm
%type <AstNode*> booleanFactor
%type <AstNode*> booleanPrimary
%type <AstNode*> booleanLiteral
%type <AstNode*> characterExpression
%type <AstNode*> characterClause
%type <AstNode*> propertyName
%type <AstNode*> predicate
%type <AstNode*> comparisonPredicate
%type <AstNode*> binaryComparisonPredicate
%type <AstNode*> scalarExpression
%type <AstNode*> numericLiteral
%type <AstNode*> spatialPredicate
%type <AstNode*> geomExpression
%type <AstNode*> spatialInstance
%type <AstNode*> isInListPredicate
%type <AstNode*> inList
%type <AstNode*> arrayPredicate
%type <AstNode*> arrayExpression
%type <AstNode*> array
%type <AstNode*> arrayElement
%type <AstNode*> arrayList
%type <AstNode*> function
%type <AstNode*> argumentList
%type <AstNode*> argument

%type <std::string> geometryLiteral
%type <std::string> pointTaggedText
%type <std::string> linestringTaggedText
%type <std::string> polygonTaggedText
%type <std::string> bboxTaggedText

%left PLUS MINUS
%left MULT DIV

%define parse.trace
%%

// Grammar rules:

program:
       booleanExpression { *root_ = $1; }

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
  function
  | predicate
  | booleanLiteral
  | LPT booleanExpression RPT { $$ = $2; }

booleanLiteral:
  TRUE { $$ = new AstNode($1); }
  | FALSE { $$ = new AstNode($1); }

predicate:
  comparisonPredicate
  | spatialPredicate
  | arrayPredicate

comparisonPredicate:
  binaryComparisonPredicate
  | isInListPredicate

isInListPredicate:
  scalarExpression IN LPT inList RPT { $$ = new AstNode(IsInListPred, In, {$1, $4}); }
  | scalarExpression NOT IN LPT inList RPT { $$ = new AstNode(IsInListPred, NotIn, {$1, $5}); }

inList:
  scalarExpression { $$ = new AstNode(InList, NullOp, {$1}); }
  | inList COMMA scalarExpression { $1->append($3);  $$ = $1; }

binaryComparisonPredicate:
  scalarExpression EQ scalarExpression { $$ = new AstNode(BinCompPred, cql2cpp::Equal, {$1, $3}); }
  | scalarExpression LT GT scalarExpression { $$ = new AstNode(BinCompPred, cql2cpp::NotEqual, {$1, $4}); }
  | scalarExpression LT    scalarExpression { $$ = new AstNode(BinCompPred, cql2cpp::Lesser,   {$1, $3}); }
  | scalarExpression GT    scalarExpression { $$ = new AstNode(BinCompPred, cql2cpp::Greater,  {$1, $3}); }
  | scalarExpression LT EQ scalarExpression { $$ = new AstNode(BinCompPred, cql2cpp::LesserEqual, {$1, $4}); }
  | scalarExpression GT EQ scalarExpression { $$ = new AstNode(BinCompPred, cql2cpp::GreaterEqual, {$1, $4}); }

scalarExpression:
  numericLiteral
  | propertyName
  | function
  | booleanLiteral
  | characterClause

characterClause:
  CASEI LPT characterExpression RPT { $$ = $3; }  // TODO
  | ACCENTI LPT characterExpression RPT { $$ = $3; }  // TODO
  | CHAR_LIT { std::string s = std::string($1); $$ = new AstNode(s.substr(1, s.size() - 2)); }

characterExpression:
  characterClause
  | propertyName { $$ = $1; }

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
  | function


spatialInstance:
  geometryLiteral {
    auto p = geos::io::WKTReader().read($1);
    if (p) {
      $$ = new AstNode(p.release());
    } else {
      error("Can not parse WKT");
      YYERROR;
    }
  }
  | bboxTaggedText {
    auto p = cql2cpp::BBoxReader().read($1);
    if (p) {
      $$ = new AstNode(p.release());
    } else {
      error("Can not parse BBOX");
      YYERROR;
    }
  }

arrayPredicate:
  arrayFunction LPT arrayExpression COMMA arrayExpression RPT { $$ = new AstNode(ArrayPred, NameOp.at($1), {$3, $5}); }

arrayExpression:
  array
  | propertyName
  | function

array:
  LPT RPT { $$ = new AstNode(Array, NullOp, {}); }
  | LPT arrayList RPT { $$ = $2; }

arrayList:
  arrayElement  { $$ = new AstNode(Array, NullOp, {$1}); }
  | arrayList COMMA arrayElement  { $1->append($3);  $$ = $1; }

arrayElement:
  characterClause
  | numericLiteral
  // | temporalInstance
  | spatialInstance
  | array  // shift/reduce conflict
  // | arithmeticExpression
  | booleanExpression
  | propertyName
  // | function  // reduce/reduce conflict

function:
  ID LPT RPT {
    AstNode * function_name = new AstNode($1);
    $$ = new AstNode(Function, NullOp, {function_name});
  }
  | ID LPT argumentList RPT {
    AstNode * function_name = new AstNode($1);
    $$ = new AstNode(Function, NullOp, {function_name, $3});
  }


argumentList:
  argument { $$ = new AstNode(ArgumentList, NullOp, {$1}); }
  | argumentList COMMA argument { $1->append($3); $$ = $1; }

argument:
  propertyName
  | function
  // | characterClause
  // | numericLiteral
  // | temporalInstance
  // | spatialInstance
  // | array
  // | arithmeticExpression
  // | booleanExpression

geometryLiteral:
  pointTaggedText
  | linestringTaggedText
  | polygonTaggedText

pointTaggedText: POINT_WKT

linestringTaggedText: LINESTRING_WKT

polygonTaggedText: POLYGON_WKT

bboxTaggedText: BBOX_TEXT

%%
