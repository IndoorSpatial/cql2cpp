%{
#include <cql2_parser_base.hh>
#include <cql2cpp/cql2_lexer-internal.h>

using cql2cpp::Cql2ParserBase;

void print(const std::string& prefix, const char* yytext) {
  LOG(INFO) << "TOKEN " << prefix << " \"" << yytext << "\"";
}

%}

%option noyywrap

DIGIT    [0-9]
ID       [:_a-zA-Z][:_a-zA-Z.0-9]*
CHAR_LIT \'[!"#$%&()*+,-.:;<=>?@a-zA-Z[\\\]^_`0-9 \t\r\n]*\'
%%

"div" { print("OP", yytext); return Cql2ParserBase::token::DIVINT;  }
"IS" { print("OP", yytext); return Cql2ParserBase::token::IS;  }
"NULL" { print("OP", yytext); return Cql2ParserBase::token::NIL;  }

{DIGIT}+    {
  print("DIGIT_INT", yytext);
  yylval_->emplace<int64_t>() = atoi(yytext);
  return Cql2ParserBase::token::NUMBER_INT;
}

{DIGIT}+"."{DIGIT}* {
  print("DIGIT_FLOAT", yytext);
  yylval_->emplace<double>() = atof(yytext);
  return Cql2ParserBase::token::NUMBER_FLOAT;
}

"."{DIGIT}* {
  print("DIGIT_FLOAT", yytext);
  yylval_->emplace<double>() = atof(yytext);
  return Cql2ParserBase::token::NUMBER_FLOAT;
}

"TRUE" {
  print("BOOL_LIT", yytext);
  yylval_->emplace<bool>() = true;
  return Cql2ParserBase::token::TRUE;
}
"FALSE" {
  print("BOOL_LIT", yytext);
  yylval_->emplace<bool>() = false;
  return Cql2ParserBase::token::FALSE;
}

"AND" {
  print("BOOL_OP", yytext);
  return Cql2ParserBase::token::AND;
}

"OR" {
  print("BOOL_OP", yytext);
  return Cql2ParserBase::token::OR;
}

"NOT" {
  print("BOOL_OP", yytext);
  return Cql2ParserBase::token::NOT;
}

"CASEI" {
  print("", yytext);
  return Cql2ParserBase::token::CASEI;
}

"ACCENTI" {
  print("", yytext);
  return Cql2ParserBase::token::ACCENTI;
}

TIMESTAMP|timestamp {
  print("", yytext);
  return Cql2ParserBase::token::TIMESTAMP;
}

DATE|date {
  print("", yytext);
  return Cql2ParserBase::token::DATE;
}

INTERVAL|interval {
  print("", yytext);
  return Cql2ParserBase::token::INTERVAL;
}

"(" {
  print("LPT", yytext);
  return Cql2ParserBase::token::LPT;
}

")" {
  print("RPT", yytext);
  return Cql2ParserBase::token::RPT;
}

"\"" {
  print("DQUOTE", yytext);
  return Cql2ParserBase::token::DQUOTE;
}

"," {
  print("COMMA", yytext);
  return Cql2ParserBase::token::COMMA;
}


S_INTERSECTS|S_EQUALS|S_DISJOINT|S_TOUCHES|S_WITHIN|S_OVERLAPS|S_CROSSES|S_CONTAINS {
  print("SPATIAL", yytext);
  yylval_->emplace<std::string>(yytext);
  return Cql2ParserBase::token::SPT_FUNC;
}

T_AFTER|T_BEFORE|T_CONTAINS|T_DISJOINT|T_DURING|T_EQUALS|T_FINISHEDBY|T_FINISHES|T_INTERSECTS|T_MEETS|T_METBY|T_OVERLAPPEDBY|T_OVERLAPS|T_STARTEDBY|T_STARTS {
  print("TIME", yytext);
  yylval_->emplace<std::string>(yytext);
  return Cql2ParserBase::token::TIME_FUNC;
}

(POINT|LINESTRING|POLYGON|GEOMETRYCOLLECTION|MULTIPOINT|MULTILINESTRING|MULTIPOLYGON)[ ]*[Z]?[ ]*"(" {
  std::stringstream ss;
  ss << yytext;

  size_t parenthesis = 1;
  do {
    char c = yyinput();;
    ss << c;
    if (c == '(') parenthesis ++;
    if (c == ')') parenthesis --;
  } while (parenthesis > 0);
  std::string text = ss.str();
  print("GEOMETRY", text.c_str());
  yylval_->emplace<std::string>() = text;
  return Cql2ParserBase::token::ANY_WKT;
}

BBOX"("[0-9. +\-,\n]+")" {
  print("BBOX", yytext);
  yylval_->emplace<std::string>() = yytext;
  return Cql2ParserBase::token::BBOX_TEXT;
}

IN {
  print("IN_OP", yytext);
  return Cql2ParserBase::token::IN;
}
"NOT"[ \t\n]+"IN" {
  print("IN_OP", yytext);
  return Cql2ParserBase::token::NOT_IN;
}

"LIKE" {
	print("LIKE_OP", yytext);
	return Cql2ParserBase::token::LIKE;
}
"NOT"[ \t\n]+"LIKE" {
	print("LIKE_OP", yytext);
	return Cql2ParserBase::token::NOT_LIKE;
}

A_EQUALS|A_CONTAINS|A_CONTAINEDBY|A_OVERLAPS {
  print("ARRAY", yytext);
  yylval_->emplace<std::string>(yytext);
  return Cql2ParserBase::token::ARR_FUNC;
}

{ID} {
  print("ID", yytext);
  yylval_->emplace<std::string>(yytext);
  return Cql2ParserBase::token::ID;
}

"+" { print("OP", yytext); return Cql2ParserBase::token::PLUS; }
"-" { print("OP", yytext); return Cql2ParserBase::token::MINUS;}
"*" { print("OP", yytext); return Cql2ParserBase::token::MULT; }
"/" { print("OP", yytext); return Cql2ParserBase::token::DIV;  }
"%" { print("OP", yytext); return Cql2ParserBase::token::MOD;  }
"^" { print("OP", yytext); return Cql2ParserBase::token::POWER;  }
"=" { print("OP", yytext); return Cql2ParserBase::token::EQ;  }
"<" { print("OP", yytext); return Cql2ParserBase::token::LT;  }
">" { print("OP", yytext); return Cql2ParserBase::token::GT;  }

"{"[^{}\n]*"}" {/* eat up one-line comments */}

[ \t\n]+ {/* eat up whitespace */}

{CHAR_LIT} {
  print("CHAR_LIT", yytext);
  yylval_->emplace<std::string>() = yytext;
  return Cql2ParserBase::token::CHAR_LIT;
}

"'" {
  print("SQUOTE", yytext);
  return Cql2ParserBase::token::SQUOTE;
}

. { LOG(INFO) << "Unrecognized character: " << yytext << std::endl; }

%%
