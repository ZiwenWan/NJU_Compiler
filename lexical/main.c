#include "lex.yy.c"
#include "syntax.tab.h"
#include <stdio.h>
#include "parsertree.h"
#include "../semantic/symbolTable.h"
#include "../intercode/intercode.h"
#include "../intercode/translate.h"
#include "../spim/spim.h"
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
	fclose(f);
	//PrintTree(root,0);
	//PrintStructList(GlobalStructList);

	initGlobalSymbolTable();
	ScanTree(root);
	//printf("End ScanTree\n");
	InitGlobalICVarList();
	GlobalICList=transProgram(root);
	printICEntry(GlobalICList);
	MachineCodeGenerator(argv[2]);
	return 0;
}
