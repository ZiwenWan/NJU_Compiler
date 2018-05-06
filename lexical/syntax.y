%{
	#include <stdio.h>
	//#include "lex.yy.c"
	//#include "lex.yy.h"
	#include "parsertree.h"
	extern int LegalFlag;
	extern int yylineno;
	int previous_lineno;
	void PrintErrorMsg(int lineno, char* msg);
%}

%union{
	struct ParserNode* node;
}

%token <node>INT
%token <node>FLOAT ID
%token <node>SEMI COMMA ASSIGNOP RELOP 
%token <node>PLUS MINUS STAR DIV
%token <node>AND OR DOT NOT
%token <node>TYPE
%token <node>LP RP LB RB LC RC
%token <node>STRUCT RETURN IF ELSE WHILE

%type <node> Program ExtDefList ExtDef Specifier ExtDecList VarDec StructSpecifier 
%type <node> OptTag Tag FunDec VarList ParamDec CompSt StmtList Stmt Exp Def DefList Dec DecList Args

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS 
%left STAR DIV
%right NOT
%left LP RP LB RB DOT
%nonassoc ELSE SEMI RC COMMA
%%
/*High-Level Definitions*/
Program : ExtDefList { $$=GenerateVariableNode(AProgram,1,$1); root=$$;} 
	;
ExtDefList:ExtDef ExtDefList{$$=GenerateVariableNode(AExtDefList,2,$1,$2);}
	|{$$=GenerateDummyNode(AExtDefList);}
	;
ExtDef:Specifier ExtDecList SEMI{$$=GenerateVariableNode(AExtDef,3,$1,$2,$3);}
	|Specifier SEMI{$$=GenerateVariableNode(AExtDef,2,$1,$2);}
	|Specifier FunDec CompSt{$$=GenerateVariableNode(AExtDef,3,$1,$2,$3);}
	|Specifier ExtDecList error SEMI
	|Specifier error SEMI
	;
ExtDecList:VarDec{$$ = GenerateVariableNode(AExtDecList, 1, $1);}
	|VarDec COMMA ExtDecList{$$ = GenerateVariableNode(AExtDecList, 3, $1, $2, $3);}
	;
/*Specifier*/
Specifier:TYPE{$$ = GenerateVariableNode(ASpecifier, 1, $1);}
	|StructSpecifier{$$ = GenerateVariableNode(ASpecifier, 1, $1);}
	;
StructSpecifier:STRUCT OptTag LC DefList RC{$$ = GenerateVariableNode(AStructSpecifier, 5, $1, $2, $3, $4, $5);}
	|STRUCT Tag{$$ = GenerateVariableNode(AStructSpecifier, 2, $1, $2);}
	|STRUCT OptTag LC error RC
	;
OptTag:ID{$$ = GenerateVariableNode(AOptTag, 1, $1);}
	|{$$ = GenerateDummyNode(AOptTag);}
	;
Tag:ID{$$ = GenerateVariableNode(ATag, 1, $1);}
	;
/*Declarators*/
VarDec:ID{$$ = GenerateVariableNode(AVarDec, 1, $1);}
	|VarDec LB INT RB{$$ = GenerateVariableNode(AVarDec, 4, $1, $2, $3, $4);}
	|VarDec LB INT error RB
	|VarDec LB error RB	
	;
FunDec:ID LP VarList RP{$$ = GenerateVariableNode(AFunDec, 4, $1, $2, $3, $4);}
	|ID LP RP{$$ = GenerateVariableNode(AFunDec, 3, $1, $2, $3);}
	|ID LP error RB
	;
VarList:ParamDec COMMA VarList{$$ = GenerateVariableNode(AVarList, 3, $1, $2, $3);}
	|ParamDec{$$ = GenerateVariableNode(AVarList, 1, $1);}
	;
ParamDec:Specifier VarDec{$$ = GenerateVariableNode(AParamDec, 2, $1, $2);};
/*Statements*/
CompSt:LC DefList StmtList RC{$$ = GenerateVariableNode(ACompSt, 4, $1, $2, $3, $4);};
StmtList:Stmt StmtList{$$ = GenerateVariableNode(AStmtList, 2, $1, $2);}|{$$ = GenerateDummyNode(AStmtList);};
Stmt:Exp SEMI{$$ = GenerateVariableNode(AStmt, 2, $1, $2);}
	|CompSt{$$ = GenerateVariableNode(AStmt, 1, $1);}
	|RETURN Exp SEMI{$$ = GenerateVariableNode(AStmt, 3, $1, $2, $3);}
	|IF LP Exp RP Stmt{$$ = GenerateVariableNode(AStmt, 5, $1, $2, $3, $4, $5);}
	|IF LP Exp RP Stmt ELSE Stmt{$$ = GenerateVariableNode(AStmt, 7, $1, $2, $3, $4, $5, $6, $7);}
	|WHILE LP Exp RP Stmt{$$ = GenerateVariableNode(AStmt, 5, $1, $2, $3, $4, $5);}
	|Exp error SEMI
	|Exp error COMMA
	|RETURN error SEMI
	|RETURN Exp error SEMI
	|Exp error RP SEMI
	;
/*Local Definitions*/
DefList:Def DefList{$$ = GenerateVariableNode(ADefList, 2, $1, $2);}|{$$ = GenerateDummyNode(ADefList);};
Def:Specifier DecList SEMI{$$ = GenerateVariableNode(ADef, 3, $1, $2, $3);}
	|Specifier DecList error SEMI
	;
DecList:Dec{$$ = GenerateVariableNode(ADecList, 1, $1);}|Dec COMMA DecList{$$ = GenerateVariableNode(ADecList, 3, $1, $2, $3);};
Dec:VarDec{$$=GenerateVariableNode(ADec,1,$1);}
	|VarDec ASSIGNOP Exp{$$=GenerateVariableNode(ADec,3,$1,$2,$3);}
	;
/*Expressions*/
Exp:Exp ASSIGNOP Exp{$$ = GenerateVariableNode(AExp, 3, $1, $2, $3);}
	|Exp AND Exp{$$ = GenerateVariableNode(AExp, 3, $1, $2, $3);}
	|Exp OR Exp{$$ = GenerateVariableNode(AExp, 3, $1, $2, $3);}
	|Exp RELOP Exp{$$ = GenerateVariableNode(AExp, 3, $1, $2, $3);}
	|Exp PLUS Exp{$$ = GenerateVariableNode(AExp, 3, $1, $2, $3);}
	| Exp MINUS Exp{$$ = GenerateVariableNode(AExp, 3, $1, $2, $3);}
	| Exp STAR Exp{$$ = GenerateVariableNode(AExp, 3, $1, $2, $3);}
	| Exp DIV Exp{$$ = GenerateVariableNode(AExp, 3, $1, $2, $3);}
	| LP Exp RP{$$ = GenerateVariableNode(AExp, 3, $1, $2, $3);}
	| MINUS Exp{$$ = GenerateVariableNode(AExp, 2, $1, $2);}
	| NOT Exp{$$ = GenerateVariableNode(AExp, 2, $1, $2);}
	| ID LP Args RP{$$ = GenerateVariableNode(AExp, 4, $1, $2, $3, $4);}
	| ID LP RP{$$ = GenerateVariableNode(AExp, 3, $1, $2, $3);}
	| Exp LB Exp RB{$$ = GenerateVariableNode(AExp, 4, $1, $2, $3, $4);}
	| Exp DOT ID{$$ = GenerateVariableNode(AExp, 3, $1, $2, $3);}
	| ID{$$ = GenerateVariableNode(AExp, 1, $1);}
	| INT{$$ = GenerateVariableNode(AExp, 1, $1);}
	| FLOAT{$$ = GenerateVariableNode(AExp, 1, $1);}
	| Exp LB Exp error RB                       {PrintErrorMsg(previous_lineno, "Missing \"]\".");}
    | Exp LB error RB                           {PrintErrorMsg(previous_lineno, "Syntax error after \"[\".");}
    | ID LP error RP                            {PrintErrorMsg(previous_lineno, "Syntax error after \"(\".");}
    | ID LP Args error RP                       {PrintErrorMsg(previous_lineno, "Syntax error before \")\".");}
    | ID LP Args error RB RP                    {PrintErrorMsg(previous_lineno, "Syntax error before \")\".");} 
	;
Args:Exp COMMA Args{$$ = GenerateVariableNode(AArgs, 3, $1, $2, $3);}
	|Exp{$$ = GenerateVariableNode(AArgs, 1, $1);}
	;

%%
void yyerror(char* msg __attribute__((unused))) {  
    LegalFlag=0;
//    previous_linenoyylineno;
    //printf("syntax error\n");
    printf("Error type B at Line %d: %s.\n", yylineno, msg);
}

void PrintErrorMsg(int lineno, char* msg){
    LegalFlag=0;
   // printf("Error type B at Line %d: %s.\n", lineno, msg);
}
