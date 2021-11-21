%{
	#include "node.h"
	#include <cstdio>
	#include <cstdlib>
	NFunction *body;
	extern int yylex();
	void yyerror(const char *s) { std::printf("Error: %s\n", s); std::exit(1); }
%}
/*类型*/
%union {
	NExpr *expr;
	NNumber *number;
	NIdentifier *ident;
	NBinaryExpr *binop;
	NPrototype *proto;
	NFunction *func;
	std::vector<std::string> *vstring;
	std::string *string;
	int token;
}

/*终结符*/
%token <string> TIDENTIFIER TNUMBER
%token <token> TLPAREN TRPAREN TDEF
%token <token> TPLUS TMINUS TMUL TDIV

/*非终结符*/

%type <expr> expr
%type <number> number
%type <ident> ident
%type <vstring> call_args
%type <proto> proto
%type <func> func

/*定义运算符优先级*/
%left TPLUS TMINUS
%left TMUL TDIV

/*定义开始符号*/
%start func

%%

ident : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
		;
number : TNUMBER { $$ = new NNumber(atof($1->c_str())); delete $1; }
		;

expr : ident { $<ident>$ = $1; }
	 | number { $<number>$ = $1; }
	| expr TMUL expr { $$ = new NBinaryExpr($2, $1, $3); }
	| expr TDIV expr { $$ = new NBinaryExpr($2, $1, $3); }
	| expr TPLUS expr { $$ = new NBinaryExpr($2, $1, $3); }
	| expr TMINUS expr { $$ = new NBinaryExpr($2, $1, $3); }
     | TLPAREN expr TRPAREN { $$ = $2; }
	 ;
	
call_args : /*blank*/  { $$ = new std::vector<std::string>(); }
		  | ident { $$ = new std::vector<std::string>(); $$->push_back($1->getName()); }
		  | call_args ident { $1->push_back($2->getName()); }
		  ;
proto : ident TLPAREN call_args TRPAREN { $$ = new NPrototype($1->getName(), *$3); }
		;
func : TDEF proto expr { body = new NFunction($2, $3)}
%%
