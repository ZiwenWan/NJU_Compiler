#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "../semantic/symbolTable.h"
#include "parsertree.h"
ParserTreeNode* root=NULL;
char* symbolsTable[48] =
                {"INT", "FLOAT", "ID", "SEMI", "COMMA", "ASSIGNOP", "RELOP",
                "PLUS", "MINUS", "STAR", "DIV", "AND", "OR", "DOT", "NOT", "TYPE",
                "LP", "RP", "LB", "RB", "LC", "RC", "STRUCT", "RETURN", "IF", "ELSE", "WHILE",
                "Program", "ExtDefList", "ExtDef", "Specifier", "ExtDecList",
                "VarDec", "StructSpecifier", "OptTag", "Tag", "FunDec", "VarList",
                "ParamDec", "CompSt", "StmtList", "Stmt", "Exp", "Def", "DefList",
                "Dec","DecList", "Args"};

char* typeTable[2] = {"int", "float"};

char* relopTable[6] = {"==", "<", ">", "!=", "<=", ">="};


ParserTreeNode* GenerateIDNode(int lineno,char* text){
	//printf("Begin ID:%s\n",text);
	ParserTreeNode* ret=(ParserTreeNode*)malloc(sizeof(ParserTreeNode));
        ret->m_NodeType=Terminal;
        ret->m_SyntaxType=AID;
        ret->m_lineno=lineno;
        ret->m_depth=0;
        ret->m_childrennum=0;
        ret->m_parent=NULL;
        ret->m_firstchild=NULL;
        ret->m_nextsibiling=NULL;
	ret->IDname=malloc(strlen(text));
	strcpy(ret->IDname,text);
	//printf("End ID:%s\n",text);
        return ret;
} 

ParserTreeNode* GenerateNormalTerminalNode(int lineno,SyntaxType typex){
	//printf("Begin %s\n",symbolsTable[typex]);
	ParserTreeNode* ret=(ParserTreeNode*)malloc(sizeof(ParserTreeNode));
	ret->m_NodeType=Terminal;
	ret->m_SyntaxType=typex;
	ret->m_lineno=lineno;
	ret->m_depth=0;
	ret->m_childrennum=0;
	ret->m_parent=NULL;
	ret->m_firstchild=NULL;
	ret->m_nextsibiling=NULL;
	//printf("End %s\n",symbolsTable[typex]);
	return ret;
}

ParserTreeNode* GenerateRelopNode(int lineno,char* text){
        ParserTreeNode* ret=(ParserTreeNode*)malloc(sizeof(ParserTreeNode));
        ret->m_NodeType=Terminal;
        ret->m_SyntaxType=ARELOP;
        ret->m_lineno=lineno;
        ret->m_depth=0;
        ret->m_childrennum=0;
        ret->m_parent=NULL;
        ret->m_firstchild=NULL;
        ret->m_nextsibiling=NULL;
	if(strcmp(text, "==") == 0)ret->relop_value = EQ;
	else if(strcmp(text, "<") == 0)ret->relop_value = LT;
	else if(strcmp(text, ">") == 0)ret->relop_value = GT;
	else if(strcmp(text, "!=") == 0)ret->relop_value = NEQ;
	else if(strcmp(text, "<=") == 0)ret->relop_value = LEQ;
	else ret->relop_value = GEQ;
        return ret;
}

ParserTreeNode* GenerateDummyNode(SyntaxType typex){
        ParserTreeNode* ret=(ParserTreeNode*)malloc(sizeof(ParserTreeNode));
        ret->m_NodeType=Dummy;
        ret->m_SyntaxType=typex;
        ret->m_depth=0;
        ret->m_childrennum=0;
        ret->m_parent=NULL;
        ret->m_firstchild=NULL;
        ret->m_nextsibiling=NULL;
        return ret;
}

ParserTreeNode* GenerateFloatNode(int lineno,char* text){
        ParserTreeNode* ret=(ParserTreeNode*)malloc(sizeof(ParserTreeNode));
        ret->m_NodeType=Terminal;
        ret->m_SyntaxType=AFLOAT;
        ret->m_lineno=lineno;
        ret->m_depth=0;
        ret->m_childrennum=0;
        ret->m_parent=NULL;
        ret->m_firstchild=NULL;
        ret->m_nextsibiling=NULL;
		ret->float_value=(float)strtod(text,NULL);
        return ret;
}



ParserTreeNode* GenerateIntNode(int lineno,char* text){
	ParserTreeNode* ret=(ParserTreeNode*)malloc(sizeof(ParserTreeNode));
        ret->m_NodeType=Terminal;
        ret->m_SyntaxType=AINT;
        ret->m_lineno=lineno;
        ret->m_depth=0;
        ret->m_childrennum=0;
        ret->m_parent=NULL;
        ret->m_firstchild=NULL;
        ret->m_nextsibiling=NULL;
	long len = strlen(text);
 	if(text[0]=='0')
    	{
        	if(len==1)
            		ret->int_value=0;
        	else if(text[1]=='x' || text[1]=='X')
            		ret->int_value=(int)strtol(text, NULL, 16);
        	else 
			ret->int_value=(int)strtol(text, NULL, 8);
    	}
    	else ret->int_value= (int)strtol(text, NULL, 10);
        return ret;

}

ParserTreeNode* GenerateTypeNode(int lineno,char* text){
        ParserTreeNode* ret=(ParserTreeNode*)malloc(sizeof(ParserTreeNode));
        ret->m_NodeType=Terminal;
        ret->m_SyntaxType=ATYPE;
        ret->m_lineno=lineno;
        ret->m_depth=0;
        ret->m_childrennum=0;
        ret->m_parent=NULL;
        ret->m_firstchild=NULL;
        ret->m_nextsibiling=NULL;
	if(strcmp(text, "int")==0) ret->type_value = TYPEINT;
		else ret->type_value = TYPEFLOAT;
        return ret;
}

char* getIDfromOptTag(ParserTreeNode* x){
	if (x->m_childrennum==0) return NULL;
	ParserTreeNode* temp=x->m_firstchild;
	if (temp->m_NodeType==Dummy) return NULL;
	else return temp->IDname;
}

char* getIDfromTag(ParserTreeNode* x){
	ParserTreeNode* temp=x->m_firstchild;
	return temp->IDname;
}
ParserTreeNode* GenerateVariableNode(SyntaxType typex,int childrennum,...){
	ParserTreeNode* ret=(ParserTreeNode*)malloc(sizeof(ParserTreeNode));
	ret->m_NodeType=Variable;
	ret->m_SyntaxType=typex;
	ret->m_childrennum=childrennum;
	ParserTreeNode* child;
	ParserTreeNode* previous;
	ret->m_nextsibiling=NULL;
	

	va_list arg_ptr;
	va_start(arg_ptr,childrennum);
	int i;
	int is_dummy=1;
	int met_first=0;
	for (i=0;i<childrennum;i++){
		child=va_arg(arg_ptr,ParserTreeNode*);
		if (i==0)
			ret->m_firstchild=child;
		else
			previous->m_nextsibiling=child;
		if (child->m_NodeType!=Dummy){
			is_dummy=0;
			if (met_first==0){
				met_first=1;
				ret->m_lineno=child->m_lineno;
			}
		}
		child->m_parent=ret;
		previous=child;
	}
	va_end(arg_ptr);
	
//	if (is_dummy==1)
//		ret->m_NodeType=Dummy;


	/*if (typex==AStructSpecifier){
		child=ret->m_firstchild;
		child=child->m_nextsibiling;
		char* structname=getIDfromOptTag(child);
		if (structname!=NULL){
			child=child->m_nextsibiling;
			child=child->m_nextsibiling;
			InsertStructDef(structname,CovertDefList2FieldList(child));

		}
	}*/
	return ret;
}

void PrintTree(ParserTreeNode* x,int d){
	if (x->m_NodeType==Dummy){
	//	printf("Dummy\n");
		return;
	}
	//printf("%d",x->m_childrennum);
	int i;
	for (i=0;i<d;i++)
		printf("  ");
	
	if (x->m_NodeType==Variable){
		printf("%s (%d)\n",symbolsTable[x->m_SyntaxType],x->m_lineno);
	}
	else{
		switch(x->m_SyntaxType){
			case AID:
			printf("ID: %s\n",x->IDname);
			break;
			case ARELOP:
			printf("RELOP: %s\n",relopTable[x->relop_value]);
			break;
			case ATYPE:
			printf("TYPE: %s\n",typeTable[x->type_value]);
			break;
			case AINT:
			printf("INT: %d\n",x->int_value);
			break;
			case AFLOAT:
			printf("FLOAT: %f\n",x->float_value);
			break;
			default:
			printf("%s\n",symbolsTable[x->m_SyntaxType]);
		}
	}
	ParserTreeNode* child;
	if (x->m_childrennum>0)
		child=x->m_firstchild;
	for (i=0;i<x->m_childrennum;i++){
		PrintTree(child,d+1);
		child=child->m_nextsibiling;
	}
}
