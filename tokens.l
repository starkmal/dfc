%{
#include <string>
#include "node.h"
#include "parser.hpp"

#define SAVE_TOKEN  yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t)    (yylval.token = t)
%}

%option noyywrap

%%

[ \t\n]					        ;
"def"							return TOKEN(TDEF);
[a-zA-Z_][a-zA-Z0-9_]*  SAVE_TOKEN; return TIDENTIFIER;
[0-9]+(\.[0-9]*)?(E[\+\-][0-9]*)?  SAVE_TOKEN; return TNUMBER;

"("	          					return TOKEN(TLPAREN);
")"					          	return TOKEN(TRPAREN);

"+"				          		return TOKEN(TPLUS);
"-"		          				return TOKEN(TMINUS);
"*"		          				return TOKEN(TMUL);
"/"				          		return TOKEN(TDIV);

.                       printf("Unknown token!\n"); yyterminate();

%%
