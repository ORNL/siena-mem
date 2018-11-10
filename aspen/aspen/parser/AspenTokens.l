%{
// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.

///\todo: get rid of option yylineno; i have to do some things differently with
///its state stack because it's handled internally by flex.  at this point, probably
///more consistent to just do it myself.
#include <string>
#include <iostream>
#include "parser/AspenParser.hpp"
#include "common/AST.h"
#define SAVE_TOKEN yylval.str = string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)

std::vector<int> lineno;
std::vector<int> column;
std::vector<int> filepos;
std::vector<std::string> filename;
std::string parsedir;

#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.filename = filename.back();                                  \
    yylloc.first_column = column.back();                                \
    yylloc.last_column = column.back() + yyleng - 1;                    \
    yylloc.first_filepos = filepos.back();                              \
    yylloc.last_filepos = filepos.back() + yyleng - 1;                  \
    column.back() += yyleng;                                            \
    filepos.back() += yyleng;                                           \
    yylval.str = yytext;
%}

%x incl
%option yylineno
%option noyywrap
%option nounput


%%

[/][/].*[\r]?[\n]                                      column.back() = 1;
[ \t]                                                  ;
[\r]?[\n]                                              column.back() = 1;

include                                                BEGIN(incl);
<incl>[ \t]*                                           ;
<incl>[^ \t\r\n]+                                      {
                                                          std::string fname = parsedir + "/" + yytext;
                                                          yyin = fopen(fname.c_str(), "r");
                                                          if (!yyin)
                                                          {
                                                             cerr << "Error: couldn't find include file '"<<fname<<"'\n";
                                                             cerr << "Note: paths are currently relative to CWD.\n";
                                                             yyterminate();
                                                          }
                                                          column.push_back(1);
                                                          filepos.push_back(0);
                                                          filename.push_back(fname);
                                                          lineno.push_back(yylineno);
                                                          yypush_buffer_state(yy_new_buffer(yyin, YY_BUF_SIZE));
                                                          BEGIN(INITIAL);
                                                          yylineno = 1;
                                                       }
<<EOF>>                                                {
                                                           yylineno = lineno.back();
                                                           column.pop_back();
                                                           filepos.pop_back();
                                                           filename.pop_back();
                                                           lineno.pop_back();
                                                           yypop_buffer_state();
                                                           if (!YY_CURRENT_BUFFER)
                                                               yyterminate();
                                                       }

%{ /* common keywords */ %}
param                                                  return TOKEN(TKW_PARAM);
enum                                                   return TOKEN(TKW_ENUM);
sample                                                 return TOKEN(TKW_SAMPLE);
in                                                     return TOKEN(TKW_IN);
with                                                   return TOKEN(TKW_WITH);
as                                                     return TOKEN(TKW_AS);
of                                                     return TOKEN(TKW_OF);
size                                                   return TOKEN(TKW_SIZE);

%{ /* application keywords */ %}
model                                                  return TOKEN(TKW_MODEL);
kernel                                                 return TOKEN(TKW_KERNEL);
data                                                   return TOKEN(TKW_DATA);
import                                                 return TOKEN(TKW_IMPORT);
to                                                     return TOKEN(TKW_TO);
from                                                   return TOKEN(TKW_FROM);
call                                                   return TOKEN(TKW_CALL);
iterate                                                return TOKEN(TKW_ITERATE);
map                                                    return TOKEN(TKW_MAP);
par                                                    return TOKEN(TKW_PAR);
seq                                                    return TOKEN(TKW_SEQ);
execute                                                return TOKEN(TKW_EXECUTE);
if                                                     return TOKEN(TKW_IF);
else                                                   return TOKEN(TKW_ELSE);
switch                                                 return TOKEN(TKW_SWITCH);
probability                                            return TOKEN(TKW_PROBABILITY);

%{ /* machine keywords */ %}
%{ // done differently now:
//include                                              return TOKEN(TKW_INCLUDE);
%}
resource                                               return TOKEN(TKW_RESOURCE);
conflict                                               return TOKEN(TKW_CONFLICT);
property                                               return TOKEN(TKW_PROPERTY);
connect                                                return TOKEN(TKW_CONNECT);
%{ // we're using identifiers for private|shared at the moment....
//private                                                return TOKEN(TKW_PRIVATE);
//shared                                                 return TOKEN(TKW_SHARED);
%}
power                                                  return TOKEN(TKW_POWER);
static                                                 return TOKEN(TKW_STATIC);
dynamic                                                return TOKEN(TKW_DYNAMIC);

%{ /* built-in constants */ %}
nano|micro|milli|kilo|mega|giga|tera|peta|exa          SAVE_TOKEN; return TKW_10POWER;

%{ /* symbols */ %}
".."                                                   return TOKEN(TDOTDOT);
"=="                                                   return TOKEN(TCOMPEQ);
"<="                                                   return TOKEN(TCOMPLE);
">="                                                   return TOKEN(TCOMPGE);
"!="                                                   return TOKEN(TCOMPNE);
"<"                                                    return TOKEN(TCOMPLT);
">"                                                    return TOKEN(TCOMPGT);
"or"                                                   return TOKEN(TOR);
"and"                                                  return TOKEN(TAND);
"="                                                    return TOKEN(TEQUAL);
"["                                                    return TOKEN(TLBRACKET);
"]"                                                    return TOKEN(TRBRACKET);
"("                                                    return TOKEN(TLPAREN);
")"                                                    return TOKEN(TRPAREN);
"{"                                                    return TOKEN(TLBRACE);
"}"                                                    return TOKEN(TRBRACE);
"."                                                    return TOKEN(TDOT);
","                                                    return TOKEN(TCOMMA);
"+"                                                    return TOKEN(TPLUS);
"-"                                                    return TOKEN(TMINUS);
"*"                                                    return TOKEN(TMUL);
"/"                                                    return TOKEN(TDIV);
"^"                                                    return TOKEN(TEXP);

%{ /* srings, identifiers, and numbers */ %}
\"([^\\\"]|\\.)*\"                                     yylval.str = string(yytext+1, yyleng-2); return TSTRING;
[a-zA-Z_][a-zA-Z0-9_]*                                 SAVE_TOKEN; return TIDENT;
[0-9]+                                                 SAVE_TOKEN; return TINT;
[0-9]+([eE][\+\-]?[0-9]+)?                             SAVE_TOKEN; return TREAL;
[0-9]+\.[0-9]*([eE][\+\-]?[0-9]+)?                     SAVE_TOKEN; return TREAL;
[0-9]*\.[0-9]+([eE][\+\-]?[0-9]+)?                     SAVE_TOKEN; return TREAL;
.                                                      std::cerr << "Syntax error\n";  yyterminate();

%%
