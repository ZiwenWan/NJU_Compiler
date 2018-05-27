%{
#include <stdio.h>
#include "parsertree.h"
#define YYSTYPE ParserTreeNode*
#include "syntax.tab.h"
int yycolumn=1;
#define YY_USER_ACTION\
yycolumn+=yyleng;
extern int LegalFlag;
%}


%option yylineno
LETTER [a-zA-Z]
DIGIT [0-9]
OCT_DIGIT [0-7]
HEX_DIGIT [0-9a-fA-F]
LETTER_ [a-zA-Z_]
ID {LETTER_}({LETTER_}|{DIGIT})*
TYPE "int"|"float"
RELOP ">"|"<"|">="|"<="|"=="|"!="
INT [1-9]{DIGIT}*|0{OCT_DIGIT}*|0[Xx]{HEX_DIGIT}*
FLOAT ({DIGIT}*\.?({DIGIT}+|({DIGIT}+\.)))([eE][+-]?{DIGIT}+)?
BLANK (" "|[\t\v])+

%%
"if" {yylval=GenerateNormalTerminalNode(yylineno,AIF);return IF;}
"else" {yylval=GenerateNormalTerminalNode(yylineno,AELSE);return ELSE;}
"while" {yylval=GenerateNormalTerminalNode(yylineno,AWHILE);return WHILE;}
"return" {yylval=GenerateNormalTerminalNode(yylineno,ARETURN);return RETURN;}
";" {yylval=GenerateNormalTerminalNode(yylineno,ASEMI);return SEMI;}
"," {yylval=GenerateNormalTerminalNode(yylineno,ACOMMA);return COMMA;}
"=" {yylval=GenerateNormalTerminalNode(yylineno,AASSIGNOP);return ASSIGNOP;}
{RELOP} {yylval=GenerateRelopNode(yylineno,yytext);return RELOP;}
"+" {yylval=GenerateNormalTerminalNode(yylineno,APLUS);return PLUS;}
"-" {yylval=GenerateNormalTerminalNode(yylineno,AMINUS);return MINUS;}
"*" {yylval=GenerateNormalTerminalNode(yylineno,ASTAR);return STAR;}
"/" {yylval=GenerateNormalTerminalNode(yylineno,ADIV);return DIV;}
"&&" {yylval=GenerateNormalTerminalNode(yylineno,AAND);return AND;}
"||" {yylval=GenerateNormalTerminalNode(yylineno,AOR);return OR;}
"." {yylval=GenerateNormalTerminalNode(yylineno,ADOT);return DOT;}
"!" {yylval=GenerateNormalTerminalNode(yylineno,ANOT);return NOT;}
"struct" {yylval=GenerateNormalTerminalNode(yylineno,ASTRUCT);return STRUCT;}
"(" {yylval=GenerateNormalTerminalNode(yylineno,ALP);return LP;}
")" {yylval=GenerateNormalTerminalNode(yylineno,ARP);return RP;}
"[" {yylval=GenerateNormalTerminalNode(yylineno,ALB);return LB;}
"]" {yylval=GenerateNormalTerminalNode(yylineno,ARB);return RB;}
"{" {yylval=GenerateNormalTerminalNode(yylineno,ALC);return LC;}
"}" {yylval=GenerateNormalTerminalNode(yylineno,ARC);return RC;}
{INT} {yylval=GenerateIntNode(yylineno,yytext);return INT;}
{FLOAT} {yylval=GenerateFloatNode(yylineno,yytext);return FLOAT;}
{TYPE} {yylval=GenerateTypeNode(yylineno,yytext);return TYPE;}
{ID} {yylval=GenerateIDNode(yylineno,yytext);return ID;}
\n {yycolumn=1;}
{BLANK} {}
. {LegalFlag=0;printf("Error type A at Line %d: Mysterious characters \'%s\'.\n",yylineno, yytext);}


%%
/*int main(int argc,char** argv){
	if (argc>1){
		if (!(yyin=fopen(argv[1],"r"))){
			perror(argv[1]);
			return 1;
		}
	}
	yylex();
	return 0;
}*/
