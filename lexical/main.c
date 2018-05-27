#include "lex.yy.c"
#include "syntax.tab.h"
#include <stdio.h>
#include "parsertree.h"
#include "../semantic/symbolTable.h"
extern FILE* yyin;
int LegalFlag=1;
int main(int argc,char** argv){
	if (argc<1) return 1;
	FILE* f=fopen(argv[1],"r");
	if (!f){
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yyparse();
//	if (LegalFlag==0) 
//		return 0;
	//if (root!=NULL)
	//PrintTree(root,0);
	//PrintStructList(GlobalStructList);

//	printf("!!!\n");
	ScanTree(root);
	return 0;
}
