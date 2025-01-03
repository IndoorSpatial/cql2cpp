%code {
#include <iostream>
#include <cstdlib>
#include <geos/io/WKTReader.h>
#include <cql2cpp/ast_node.h>
#include <cql2cpp/bbox_reader.h>

using cql2cpp::AstNode;
using cql2cpp::AstNodePtr;

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
%parse-param {cql2cpp::AstNodePtr* root_}

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

%type <AstNodePtr> booleanExpression
%type <AstNodePtr> booleanTerm
%type <AstNodePtr> booleanFactor
%type <AstNodePtr> booleanPrimary
%type <AstNodePtr> booleanLiteral
%type <AstNodePtr> characterExpression
%type <AstNodePtr> characterClause
%type <AstNodePtr> propertyName
%type <AstNodePtr> predicate
%type <AstNodePtr> comparisonPredicate
%type <AstNodePtr> binaryComparisonPredicate
%type <AstNodePtr> scalarExpression
%type <AstNodePtr> numericLiteral
%type <AstNodePtr> spatialPredicate
%type <AstNodePtr> geomExpression
%type <AstNodePtr> spatialInstance
%type <AstNodePtr> isInListPredicate
%type <AstNodePtr> inList
%type <AstNodePtr> arrayPredicate
%type <AstNodePtr> arrayExpression
%type <AstNodePtr> array
%type <AstNodePtr> arrayElement
%type <AstNodePtr> arrayList
%type <AstNodePtr> function
%type <AstNodePtr> argumentList
%type <AstNodePtr> argument

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
  | booleanTerm OR booleanExpression { $$ = std::make_shared<AstNode>(BoolExpression, Or, std::vector({$1, $3})); }

booleanTerm:
  booleanFactor
  | booleanFactor AND booleanTerm  { $$ = std::make_shared<AstNode>(BoolExpression, And, std::vector({$1, $3})); }

booleanFactor:
  booleanPrimary
  | NOT booleanPrimary { $$ = std::make_shared<AstNode>(BoolExpression, Not, std::vector({ $2 })); }

booleanPrimary:
  function
  | predicate
  | booleanLiteral
  | LPT booleanExpression RPT { $$ = $2; }

booleanLiteral:
  TRUE { $$ = std::make_shared<AstNode>($1); }
  | FALSE { $$ = std::make_shared<AstNode>($1); }

predicate:
  comparisonPredicate
  | spatialPredicate
  | arrayPredicate

comparisonPredicate:
  binaryComparisonPredicate
  | isInListPredicate

isInListPredicate:
  scalarExpression IN LPT inList RPT { $$ = std::make_shared<AstNode>(IsInListPred, In, std::vector({$1, $4})); }
  | scalarExpression NOT IN LPT inList RPT { $$ = std::make_shared<AstNode>(IsInListPred, NotIn, std::vector({$1, $5})); }

inList:
  scalarExpression { $$ = std::make_shared<AstNode>(InList, NullOp, std::vector({$1})); }
  | inList COMMA scalarExpression { $1->append($3);  $$ = $1; }

binaryComparisonPredicate:
  scalarExpression EQ scalarExpression { $$ = std::make_shared<AstNode>(BinCompPred, cql2cpp::Equal, std::vector({$1, $3})); }
  | scalarExpression LT GT scalarExpression { $$ = std::make_shared<AstNode>(BinCompPred, cql2cpp::NotEqual, std::vector({$1, $4})); }
  | scalarExpression LT    scalarExpression { $$ = std::make_shared<AstNode>(BinCompPred, cql2cpp::Lesser,   std::vector({$1, $3})); }
  | scalarExpression GT    scalarExpression { $$ = std::make_shared<AstNode>(BinCompPred, cql2cpp::Greater,  std::vector({$1, $3})); }
  | scalarExpression LT EQ scalarExpression { $$ = std::make_shared<AstNode>(BinCompPred, cql2cpp::LesserEqual, std::vector({$1, $4})); }
  | scalarExpression GT EQ scalarExpression { $$ = std::make_shared<AstNode>(BinCompPred, cql2cpp::GreaterEqual, std::vector({$1, $4})); }

scalarExpression:
  numericLiteral
  | propertyName
  | function
  | booleanLiteral
  | characterClause

characterClause:
  CASEI LPT characterExpression RPT { $$ = $3; }  // TODO
  | ACCENTI LPT characterExpression RPT { $$ = $3; }  // TODO
  | CHAR_LIT { std::string s = std::string($1); $$ = std::make_shared<AstNode>(s.substr(1, s.size() - 2)); }

characterExpression:
  characterClause
  | propertyName { $$ = $1; }

numericLiteral:
  NUMBER_INT { $$ = std::make_shared<AstNode>($1); }
  | NUMBER_FLOAT { $$ = std::make_shared<AstNode>($1); }

propertyName:
  ID { $$ = std::make_shared<AstNode>(PropertyName, std::string($1)); }
  // | DQUOTE ID DQUOTE;

spatialPredicate:
  SPT_FUNC LPT geomExpression COMMA geomExpression RPT { $$ = std::make_shared<AstNode>(SpatialPred, NameOp.at($1), std::vector({$3, $5})); }


geomExpression:
  spatialInstance
  | propertyName
  | function


spatialInstance:
  geometryLiteral {
    auto p = geos::io::WKTReader().read($1);
    if (p) {
      $$ = std::make_shared<AstNode>(p.release());
    } else {
      error("Can not parse WKT");
      YYERROR;
    }
  }
  | bboxTaggedText {
    auto p = cql2cpp::BBoxReader().read($1);
    if (p) {
      $$ = std::make_shared<AstNode>(p.release());
    } else {
      error("Can not parse BBOX");
      YYERROR;
    }
  }

arrayPredicate:
  arrayFunction LPT arrayExpression COMMA arrayExpression RPT { $$ = std::make_shared<AstNode>(ArrayPred, NameOp.at($1), std::vector({$3, $5})); }

arrayExpression:
  array
  | propertyName
  | function

array:
  LPT RPT { $$ = std::make_shared<AstNode>(Array, NullOp, std::vector<AstNodePtr>()); }
  | LPT arrayList RPT { $$ = $2; }

arrayList:
  arrayElement  { $$ = std::make_shared<AstNode>(Array, NullOp, std::vector({$1})); }
  | arrayList COMMA arrayElement  { $1->append($3);  $$ = $1; }

arrayElement:
  characterClause
  | numericLiteral
  // | temporalInstance
  | spatialInstance
  // | array  // shift/reduce conflict
  // | arithmeticExpression
  | booleanExpression
  | propertyName
  // | function  // reduce/reduce conflict

function:
  ID LPT RPT {
    AstNodePtr function_name = std::make_shared<AstNode>($1);
    $$ = std::make_shared<AstNode>(Function, NullOp, std::vector({function_name}));
  }
  | ID LPT argumentList RPT {
    AstNodePtr function_name = std::make_shared<AstNode>($1);
    $$ = std::make_shared<AstNode>(Function, NullOp, std::vector({function_name, $3}));
  }


argumentList:
  argument { $$ = std::make_shared<AstNode>(ArgumentList, NullOp, std::vector({$1})); }
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
