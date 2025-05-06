%code {
#include <iostream>
#include <cstdlib>
#include <geos/io/WKTReader.h>
#include <cql2cpp/ast_node.h>
#include <cql2cpp/bbox_reader.h>

using cql2cpp::AstNode;
using cql2cpp::AstNodePtr;

using cql2cpp::BoolExpr;
using cql2cpp::BinCompPred;
using cql2cpp::IsLikePred;
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

// print line number to debug
#define PL LOG(INFO) << __LINE__
#define MakeAstNode std::make_shared<AstNode>

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

%token <int64_t> NUMBER_INT
%token <double> NUMBER_FLOAT
%token <bool> BOOLEAN
%token <bool> TRUE FALSE
%token <std::string> ID
%token <std::string> CHAR_LIT
%token <std::string> SPT_FUNC
%token <std::string> arrayFunction
%token PLUS MINUS MULT DIV MOD DIVINT POWER
%token EQ GT LT  // = > <
%token AND OR NOT
%token LIKE NOT_LIKE
%token IN NOT_IN
%token IS NIL // NILL=="NULL" to avoid conflict with C NULL
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
%type <AstNodePtr> arithmeticExpression
%type <AstNodePtr> arithmeticFactor
%type <AstNodePtr> arithmeticTerm
%type <AstNodePtr> arithmeticOperand
%type <AstNodePtr> powerTerm
%type <AstNodePtr> isLikePredicate
%type <AstNodePtr> patternExpression

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
       booleanExpression { PL; *root_ = $1; }

booleanExpression:
  booleanTerm
  | booleanTerm OR booleanExpression { PL; $$ = MakeAstNode(BoolExpr, Or, std::vector({$1, $3})); }

booleanTerm:
  booleanFactor
  | booleanFactor AND booleanTerm  { PL; $$ = MakeAstNode(BoolExpr, And, std::vector({$1, $3})); }

booleanFactor:
  booleanPrimary
  | NOT booleanPrimary { PL; $$ = MakeAstNode(BoolExpr, Not, std::vector({ $2 })); }

booleanPrimary:
  function
  | predicate
  | booleanLiteral
  | LPT booleanExpression RPT { PL; $$ = $2; }

booleanLiteral:
  TRUE { PL; $$ = MakeAstNode($1); }
  | FALSE { PL; $$ = MakeAstNode($1); }

predicate:
  comparisonPredicate
  | spatialPredicate
  | arrayPredicate

comparisonPredicate:
  binaryComparisonPredicate
  | isLikePredicate
  // | isBetweenPredicate
  | isInListPredicate
  // | isNullPredicate

isInListPredicate:
  scalarExpression IN LPT inList RPT { PL; $$ = MakeAstNode(IsInListPred, In, std::vector({$1, $4})); }
  | scalarExpression NOT_IN LPT inList RPT { PL; $$ = MakeAstNode(IsInListPred, NotIn, std::vector({$1, $4})); }

inList:
  scalarExpression { PL; $$ = MakeAstNode(InList, NullOp, std::vector({$1})); }
  | inList COMMA scalarExpression { PL; $1->append($3);  $$ = $1; }

// isNullPredicate:
//   isNullOperand IS NIL
//   | isNullOperand IS NOT NIL

// isNullOperand:
//   characterClause
//   | numericLiteral
//   // | temporalInstance
//   | spatialInstance
//   | arithmeticExpression
//   | booleanExpression
//   | propertyName
//   | function

binaryComparisonPredicate:
  scalarExpression EQ scalarExpression { PL; $$ = MakeAstNode(BinCompPred, cql2cpp::Equal, std::vector({$1, $3})); }
  | scalarExpression LT GT scalarExpression { PL; $$ = MakeAstNode(BinCompPred, cql2cpp::NotEqual, std::vector({$1, $4})); }
  | scalarExpression LT    scalarExpression { PL; $$ = MakeAstNode(BinCompPred, cql2cpp::Lesser,   std::vector({$1, $3})); }
  | scalarExpression GT    scalarExpression { PL; $$ = MakeAstNode(BinCompPred, cql2cpp::Greater,  std::vector({$1, $3})); }
  | scalarExpression LT EQ scalarExpression { PL; $$ = MakeAstNode(BinCompPred, cql2cpp::LesserEqual, std::vector({$1, $4})); }
  | scalarExpression GT EQ scalarExpression { PL; $$ = MakeAstNode(BinCompPred, cql2cpp::GreaterEqual, std::vector({$1, $4})); }

scalarExpression:
  characterClause
  | numericLiteral
  // | instantInstance
  | arithmeticExpression
  | booleanLiteral
  | propertyName
  | function

isLikePredicate:
  characterExpression LIKE patternExpression { PL; $$ = MakeAstNode(IsLikePred, cql2cpp::Like, std::vector({$1, $3})); }
  | characterExpression NOT_LIKE patternExpression { PL; $$ = MakeAstNode(IsLikePred, cql2cpp::NotLike, std::vector({$1, $3})); }

patternExpression:
  // CASEI LPT patternExpression RPT
  // | ACCENTI LPT patternExpression RPT
  | CHAR_LIT { PL; std::string s = std::string($1); $$ = MakeAstNode(s.substr(1, s.size() - 2)); }


characterClause:
  CASEI LPT characterExpression RPT { PL; $$ = $3; }  // TODO
  | ACCENTI LPT characterExpression RPT { PL; $$ = $3; }  // TODO
  | CHAR_LIT { PL; std::string s = std::string($1); $$ = MakeAstNode(s.substr(1, s.size() - 2)); }

characterExpression:
  characterClause
  | propertyName
  | function

numericLiteral:
  MINUS NUMBER_INT { PL; $$ = MakeAstNode($2 * -1); }
  | MINUS NUMBER_FLOAT { PL; $$ = MakeAstNode($2 * -1.0); }
  | NUMBER_INT { PL; $$ = MakeAstNode($1); }
  | NUMBER_FLOAT { PL; $$ = MakeAstNode($1); }
  | PLUS NUMBER_INT { PL; $$ = MakeAstNode($2); }
  | PLUS NUMBER_FLOAT { PL; $$ = MakeAstNode($2); }

propertyName:
  ID { PL; $$ = MakeAstNode(PropertyName, std::string($1)); }
  | DQUOTE ID DQUOTE  { PL; $$ = MakeAstNode(PropertyName, std::string($2)); }

spatialPredicate:
  SPT_FUNC LPT geomExpression COMMA geomExpression RPT { PL; $$ = MakeAstNode(SpatialPred, NameOp.at($1), std::vector({$3, $5})); }


geomExpression:
  spatialInstance
  | propertyName
  | function


spatialInstance:
  geometryLiteral {
    auto p = geos::io::WKTReader().read($1);
    if (p) {
      $$ = MakeAstNode(p.release());
    } else {
      error("Can not parse WKT");
      YYERROR;
    }
  }
  | bboxTaggedText {
    auto p = cql2cpp::BBoxReader().read($1);
    if (p) {
      $$ = MakeAstNode(p.release());
    } else {
      error("Can not parse BBOX");
      YYERROR;
    }
  }

arrayPredicate:
  arrayFunction LPT arrayExpression COMMA arrayExpression RPT { PL; $$ = MakeAstNode(ArrayPred, NameOp.at($1), std::vector({$3, $5})); }

arrayExpression:
  array
  | propertyName
  | function

array:
  LPT RPT { PL; $$ = MakeAstNode(Array, NullOp, std::vector<AstNodePtr>()); }
  | LPT arrayList RPT { PL; $$ = $2; }

arrayList:
  arrayElement  { PL; $$ = MakeAstNode(Array, NullOp, std::vector({$1})); }
  | arrayList COMMA arrayElement  { PL; $1->append($3);  $$ = $1; }

arrayElement:
  characterClause
  | numericLiteral
  // | temporalInstance
  | spatialInstance
  // | array  // shift/reduce conflict
  | arithmeticExpression
  | booleanExpression
  | propertyName
  // | function  // reduce/reduce conflict

arithmeticExpression:
  arithmeticTerm
  | arithmeticTerm PLUS arithmeticExpression { PL; $$ = MakeAstNode(ArithExpr, PLUS, std::vector({$1, $3})); }
  | arithmeticTerm MINUS arithmeticExpression { PL; $$ = MakeAstNode(ArithExpr, MINUS, std::vector({$1, $3})); }

arithmeticTerm:
  powerTerm
  | powerTerm MULT arithmeticTerm { PL; $$ = MakeAstNode(ArithExpr, MULT, std::vector({$1, $3})); }
  | powerTerm DIV arithmeticTerm { PL; $$ = MakeAstNode(ArithExpr, DIV, std::vector({$1, $3})); }
  | powerTerm MOD arithmeticTerm { PL; $$ = MakeAstNode(ArithExpr, MOD, std::vector({$1, $3})); }
  | powerTerm DIVINT arithmeticTerm { PL; $$ = MakeAstNode(ArithExpr, DIVINT, std::vector({$1, $3})); }

powerTerm:
  arithmeticFactor
  | arithmeticFactor POWER arithmeticFactor { PL; $$ = MakeAstNode(ArithExpr, POWER, std::vector({$1, $3})); }

arithmeticFactor:
  LPT arithmeticExpression RPT { PL; $$ = $2; }
  | arithmeticOperand
  | MINUS arithmeticOperand { PL; $$ = MakeAstNode(ArithExpr, MINUS, std::vector({$2})); }

arithmeticOperand:
  numericLiteral
  | propertyName
  | function

function:
  ID LPT RPT {
    AstNodePtr function_name = MakeAstNode($1);
    $$ = MakeAstNode(Function, NullOp, std::vector({function_name}));
  }
  | ID LPT argumentList RPT {
    AstNodePtr function_name = MakeAstNode($1);
    $$ = MakeAstNode(Function, NullOp, std::vector({function_name, $3}));
  }


argumentList:
  argument { PL; $$ = MakeAstNode(ArgumentList, NullOp, std::vector({$1})); }
  | argumentList COMMA argument { PL; $1->append($3); $$ = $1; }

argument:
  propertyName
  | function
  // | characterClause
  // | numericLiteral
  // | temporalInstance
  // | spatialInstance
  // | array
  | arithmeticExpression
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
