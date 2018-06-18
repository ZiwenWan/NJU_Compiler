#include <stdlib.h>
#include <stdio.h>
#include "symbolTable.h"
#include <string.h>
#include "covert.h"
StructList GlobalStructList=NULL;
SymbolTableEntry GlobalSymbolTable=NULL;
void initGlobalSymbolTable(){
	//printf("Begin Init\n");
	Type t_int=malloc(sizeof(struct Type_));
	t_int->kind=BASIC;
	t_int->u.basic=0;
	SymbolTableEntry readSym=malloc(sizeof(struct SymbolTableEntry_));
	readSym->id=malloc(strlen("read")+1);
	strcpy(readSym->id,"read");
	readSym->kind=FUNC;
	readSym->u.Function.RetType=t_int;
	readSym->u.Function.InputType=NULL;

	SymbolTableEntry writeSym=malloc(sizeof(struct SymbolTableEntry_));
	writeSym->id=malloc(strlen("write")+1);
	strcpy(writeSym->id,"write");
	writeSym->kind=FUNC;
	writeSym->u.Function.RetType=t_int;
	FieldList tempfl=malloc(sizeof(struct FieldList_));
	tempfl->type=t_int;
	tempfl->next=NULL;
	tempfl->name=malloc(strlen("aaa")+1);
	strcpy(tempfl->name,"aaa");
	writeSym->u.Function.InputType=tempfl;

	readSym->next=writeSym;
	writeSym->next=NULL;
	GlobalSymbolTable=readSym;
	//printf("End INit\n");
}

ParserTreeNode* GetithChild(ParserTreeNode* x,int n){
	if (n==1) return x->m_firstchild;
	if (n<x->m_childrennum+1){
		ParserTreeNode* temp=x->m_firstchild;
		int i;
		for (int i=0;i<n-1;i++)
			temp=temp->m_nextsibiling;
		return temp;
	}
	return NULL;
}

FieldList CovertArgs2FieldList(ParserTreeNode* x){
	if (x->m_childrennum==1){
		FieldList newfield=malloc(sizeof(struct FieldList_));
		newfield->next=NULL;
		newfield->type=GetExpType(GetithChild(x,1));
		if (newfield->type==NULL) return NULL;
		return newfield;
	}	
	else{
		FieldList newfield=malloc(sizeof(struct FieldList_));
		newfield->type=GetExpType(GetithChild(x,1));
		if (newfield->type==NULL) return NULL;
		newfield->next=CovertArgs2FieldList(GetithChild(x,3));
		return newfield;
	}
}

int FieldListcmp(FieldList a,FieldList b){
	if (a==NULL||b==NULL) return 1;
	FieldList temp1=a; FieldList temp2=b;
	while (temp1!=NULL&& temp2!=NULL){
		if (typecmp(temp1->type,temp2->type)==0)
			return 0;
		temp1=temp1->next;
		temp2=temp2->next;
	}
	if (temp1!=NULL ||temp2!=NULL) return 0;
	else return 1;
}

Type GetReturnType(ParserTreeNode* x){
	ParserTreeNode* temp=x;
	while (temp->m_SyntaxType!=AExtDef)
		temp=temp->m_parent;
	return CovertSpecifier2Type(GetithChild(temp,1));
}

void ScanTree(ParserTreeNode* x){
	if (x==NULL) return;
	//printf("Begin %s\n",symbolsTable[x->m_SyntaxType]);

	ParserTreeNode* temp=x->m_firstchild;

	if (x->m_SyntaxType==AExtDef){

		if (GetithChild(x,2)->m_SyntaxType==ASEMI)
			CovertSpecifier2Type(GetithChild(x,1));
		if (GetithChild(x,2)->m_SyntaxType==AExtDecList){
			Type temptype=CovertSpecifier2Type(GetithChild(x,1));
			AddExtDecList2SymbolTable(GetithChild(x,2),temptype);
		}
		if (GetithChild(x,2)->m_SyntaxType==AFunDec){
			Type temptype=CovertSpecifier2Type(GetithChild(x,1));
			AddFuncDec2SymbolTable(GetithChild(x,2),temptype);
		}
	}

	
	if (x->m_SyntaxType==ADef){
		Type temptype=CovertSpecifier2Type(GetithChild(x,1));
		//PrintType(temptype);
		//printf("Middle Covert Def\n");
		AddDecList2SymbolTable(GetithChild(x,2),temptype);
		//printf("End Covert Def\n");
	}

	if (x->m_SyntaxType==AStmt){
		if (GetithChild(x,1)->m_SyntaxType==ARETURN){
			Type temp1=GetExpType(GetithChild(x,2));
			Type temp2=GetReturnType(x);
			if (temp1!=NULL &&temp2!=NULL){
				if (typecmp(temp1,temp2)==0){
					printf("Error type 8 at Line %d: Type mismatched for return.\n",x->m_lineno);
				}
			}
		}
	}

	if (x->m_SyntaxType==AExp){
		//printf("Check Exp\n");
		/*printf("Childnum%d\n",x->m_childrennum);
		if (x->m_childrennum==1){
			if ((x->m_firstchild)->m_SyntaxType==AID){
				SymbolTableEntry tempindex;
				//printf("Begin Search %s\n",(x->m_firstchild)->IDname);
				tempindex=SearchSymbolTable((x->m_firstchild)->IDname,1);
				//printf("End Search %s\n",(x->m_firstchild)->IDname);
				if (tempindex==NULL){
					printf("Error type 1 at Line %d: Undefined variable '%s'.\n",x->m_lineno,(x->m_firstchild)->IDname);
				}
			}
		}
		else {
			if (GetithChild(x,1)->m_SyntaxType==AID && GetithChild(x,2)->m_SyntaxType==ALP){
				
				SymbolTableEntry tempindex;
				tempindex=SearchSymbolTable((x->m_firstchild)->IDname,0);
				if (tempindex==NULL){
					printf("Error type 2 at Line %d: Undefined function '%s'.\n",x->m_lineno,(x->m_firstchild)->IDname);
				}
			}
			if (GetithChild(x,1)->m_SyntaxType==AID && GetithChild(x,2)->m_SyntaxType==ALB){
				printf("Here\n");
				SymbolTableEntry tempindex;
				tempindex=SearchSymbolTable((x->m_firstchild)->IDname,1);
				if (tempindex->u.Variable.VariableType->kind!=ARRAY){
					printf("Error type 10 at Line %d: '%s' is not an array.",x->m_lineno,(x->m_firstchild)->IDname);
				}
			}
		}*/
		handleExp(x);
	}
	/*if (x->m_SyntaxType==AExp){
		if (x->m_childrennum==1){
			if (GetithChild(x,1)->m_SyntaxType==AID){
				SymbolTableEntry t=SearchSymbolTable((x->m_firstchild)->IDname);
				if (t==NULL){
					printf("Error type 1 at Line %d: Undefined variable '%s'.\n",x->m_lineno,(x->m_firstchild)->IDname);
				}
			}
		}

	}*/


	temp=x->m_firstchild;
	while (temp!=NULL){
		ScanTree(temp);
		temp=temp->m_nextsibiling;
	}
	//printf("End %s\n",symbolsTable[x->m_SyntaxType]);
}

int typecmp(Type t1,Type t2){
	if (t1->kind!=t2->kind) return 0;
	//printf("%d %d %d %d\n",t1->kind,t2->kind,t1->u.basic,t2->u.basic);
	if ((t1->kind==BASIC) && ((t1->u.basic)!=(t2->u.basic))) return 0;
	//printf("HEREEEE\n");
	if (t1->kind==ARRAY) return 0;
	if (t1->kind==STRUCTURE && (strcmp(t1->u.structure.name,t2->u.structure.name)!=0)) return 0;
	return 1;
}

Type GetTypeFromFieldList(char* idx,FieldList fx){
	FieldList temp=fx;
	while (temp!=NULL){
		if (strcmp(temp->name,idx)==0)
			return temp->type;
		temp=temp->next;
	}
	return NULL;

}

int checkleftExp(ParserTreeNode* x){
	if (x->m_childrennum==1 && GetithChild(x,1)->m_SyntaxType==AID) return 1;
	if (x->m_childrennum==3 && GetithChild(x,1)->m_SyntaxType==AExp
		&& GetithChild(x,2)->m_SyntaxType==ADOT) return 1;
	if (x->m_childrennum==4 && GetithChild(x,1)->m_SyntaxType==AExp) return 1;
	return 0;

}

void handleExp(ParserTreeNode* x){
	if (x->m_childrennum==1){
		if (GetithChild(x,1)->m_SyntaxType==AID){
			//printf("Here %s\n",GetithChild(x,1)->IDname);
			SymbolTableEntry tempindex;
				//printf("Begin Search %s\n",(x->m_firstchild)->IDname);
			tempindex=SearchSymbolTable((x->m_firstchild)->IDname,1);
				//printf("End Search %s\n",(x->m_firstchild)->IDname);
			if (tempindex==NULL){
				printf("Error type 1 at Line %d: Undefined variable '%s'.\n",x->m_lineno,(x->m_firstchild)->IDname);
			}
			
		}
	}

	if (x->m_childrennum==2){
		/*if (GetithChild(x,1)->m_SyntaxType==AMINUS){
			Type temp=GetExpType(GetithChild(x,2));
			if (temp==NULL) return NULL;
			if (temp->kind!=BASIC) return NULL;
			return temp;
		}

		if (GetithChild(x,1)->m_SyntaxType==AMINUS){
			Type temp=GetExpType(GetithChild(x,2));
			if (temp==NULL) return NULL;
			if (temp->kind!=BASIC||temp->u.basic==1) return NULL;
			return temp;
		}*/

	}

	if (x->m_childrennum==3){
		/*if (GetithChild(x,2)->m_SyntaxType==AAND||GetithChild(x,2)->m_SyntaxType==AOR){
			Type temp=GetExpType(GetithChild(x,1));
			if (temp==NULL) return NULL;
			if (temp->kind!=BASIC||temp->u.basic==1) return NULL;
			temp=GetExpType(GetithChild(x,3));
			if (temp==NULL) return NULL;
			if (temp->kind!=BASIC||temp->u.basic==1) return NULL;
			return temp;
		}*/



		if (GetithChild(x,2)->m_SyntaxType==AASSIGNOP){
			Type temp1=GetExpType(GetithChild(x,1));
			Type temp2=GetExpType(GetithChild(x,3));
			//PrintType(temp1);
			//PrintType(temp2);
			if (temp1==NULL ||temp2==NULL) return;
			if (typecmp(temp1,temp2)==0){
				printf("Error type 5 at Line %d: Type mismatched for assignment.\n",x->m_lineno);
			}
			if (checkleftExp(GetithChild(x,1))==0){
				printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n",x->m_lineno);
			}
		}

		if (GetithChild(x,2)->m_SyntaxType==ADOT){
			Type temp=GetExpType(GetithChild(x,1));
			if (temp->kind!=STRUCTURE){
				printf("Error type 13 at Line %d: Illegal use of '.'.\n",x->m_lineno);
			}
			else if (GetTypeFromFieldList(GetithChild(x,3)->IDname,temp->u.structure.memberlist)==NULL){
				printf("Error type 14 at line %d:Nonexist field '%s'\n",x->m_lineno,GetithChild(x,3)->IDname);
			}
		}

		if (GetithChild(x,1)->m_SyntaxType==AID){
			SymbolTableEntry tempindex=SearchSymbolTable(GetithChild(x,1)->IDname,0);
			if (tempindex==NULL){
				printf("Error type 2 at Line %d: Undefined function '%s'\n",x->m_lineno,GetithChild(x,1)->IDname);
			}
			else if (tempindex->kind==1){
				printf("Error type 11 at Line %d: '%s' is not a function.\n",x->m_lineno,GetithChild(x,1)->IDname);
			}else if (tempindex->u.Function.InputType!=NULL){
				printf("Error type 9 at Line %d: Function is not applicable for arguments.\n",x->m_lineno);
			}
		}

		if (GetithChild(x,2)->m_SyntaxType==APLUS
			||GetithChild(x,2)->m_SyntaxType==ARELOP
			||GetithChild(x,2)->m_SyntaxType==ASTAR
			||GetithChild(x,2)->m_SyntaxType==ADIV
			||GetithChild(x,2)->m_SyntaxType==AMINUS){
			Type temp1=GetExpType(GetithChild(x,1));
			Type temp2=GetExpType(GetithChild(x,3));
		//	PrintType(temp1);
		//	PrintType(temp2);
			if (temp1==NULL ||temp2==NULL) return;
			if (temp1->kind==BASIC && temp2->kind==BASIC && temp1->u.basic==temp2->u.basic){
				return;
			}
			else{
				printf("Error type 7 at Line %d: Type mismatched for operands.\n",x->m_lineno);
			}
		}

		/*if (GetithChild(x,2)->m_SyntaxType==AExp){
			Type temp=GetExpType(GetithChild(x,2));
			return temp;
		}

		if (GetithChild(x,1)->m_SyntaxType==AExp){
			Type temp=GetExpType(GetithChild(x,1));
			if (temp==NULL) return NULL;
			if (temp->kind!=STRUCTURE) return NULL;
			Type temp1=GetTypeFromFieldList(temp->u.structure.memberlist);
			return temp1;
		}

		if (GetithChild(x,1)->m_SyntaxType==AID){
			SymbolTableEntry tempindex=SearchSymbolTable((x->m_firstchild)->IDname,0);
			if (tempindex==NULL) return NULL;
			return tempindex->u.Function.RetType;
		}*/
	}
	if (x->m_childrennum==4){
		if (GetithChild(x,1)->m_SyntaxType==AID){
			SymbolTableEntry tempindex=SearchSymbolTable(GetithChild(x,1)->IDname,0);
			if (tempindex==NULL){
				printf("Error type 2 at Line %d: Undefined function '%s'\n",x->m_lineno,GetithChild(x,1)->IDname);
			}
			else if (tempindex->kind==1){
				printf("Error type 11 at Line %d: '%s' is not a function.\n",x->m_lineno,GetithChild(x,1)->IDname);
			}
		}

		if (GetithChild(x,1)->m_SyntaxType==AExp){
			Type temp=GetExpType(GetithChild(x,1));
			if (temp->kind!=ARRAY){
				printf("Error type 10 at Line %d: not an array.\n",x->m_lineno);
			}
			Type temp1=GetExpType(GetithChild(x,3));
			if (temp1->kind!=BASIC||temp1->u.basic==1){
				printf("Error type 12 at Line %d:array index is not an integer\n",x->m_lineno);
			}
		}
		if (GetithChild(x,1)->m_SyntaxType==AID){
			SymbolTableEntry tempindex=SearchSymbolTable((x->m_firstchild)->IDname,0);
			if (tempindex==NULL){
				printf("Error type 2 at Line %d: Undefined function '%s'\n",x->m_lineno,GetithChild(x,1)->IDname);
			}
			else if (tempindex->kind==1){
				printf("Error type 11 at Line %d: '%s' is not a function.\n",x->m_lineno,GetithChild(x,1)->IDname);
			}else
			if (FieldListcmp(CovertArgs2FieldList(GetithChild(x,3)),tempindex->u.Function.InputType)==0){
				printf("Error type 9 at Line %d: Function is not applicable for arguments.\n",x->m_lineno);

			}
		}
		/*
		if (GetithChild(x,1)->m_SyntaxType==AExp){
			Type temp1=GetExpType(GetithChild(x,1));
			Type temp2=GetExpType(GetithChild(x,2));
			if (temp1==NULL||temp2==NULL) return NULL;
			if (temp2->kind!=BASIC&&temp2->u.basic==1) return NULL;
			if (temp1->kind!=ARRAY) return NULL;
			return temp1->u.structure.elem;
		}*/
	}
}


void AddDecList2SymbolTable(ParserTreeNode* x,Type typex){
	ParserTreeNode* temp=x->m_firstchild;
	AddVarDec2SymbolTable(temp->m_firstchild,typex);
	if (x->m_childrennum==3)
		AddDecList2SymbolTable(GetithChild(x,3),typex);
}

void AddExtDecList2SymbolTable(ParserTreeNode* x,Type typex){
	ParserTreeNode* temp=x->m_firstchild;
	AddVarDec2SymbolTable(temp,typex);
}

int checkparentstruct(ParserTreeNode* x){
	ParserTreeNode* temp=x;
	while (temp!=NULL){
		if (temp->m_SyntaxType==AStructSpecifier)
			return 1;
		temp=temp->m_parent;
	}
	return 0;
}
void AddVarDec2SymbolTable(ParserTreeNode* x,Type typex){
	//printf("begin AddVarDec\n");
	if (x->m_childrennum==1){
		if (checkparentstruct(x)==0)
			AddVar2SymbolTable(x->m_lineno,(x->m_firstchild)->IDname,typex);
		else
			AddStructVar2SymbolTable(x->m_lineno,(x->m_firstchild)->IDname,typex);
	}
	else{
		//printf("Here AddVarDec\n");
		ParserTreeNode* temp=x->m_firstchild;	
		temp=temp->m_nextsibiling;
		temp=temp->m_nextsibiling;
		Type newType=malloc(sizeof(Type_));
		newType->kind=ARRAY;
		newType->u.array.size=temp->int_value;
		newType->u.array.elem=typex;
	//	AddVarDec2SymbolTable(x->m_firstchild,newType);
		//printf("Here Mid ADDVARDEc\n");
		//PrintType(typex);
		if (checkparentstruct(x)==0)
			AddVarDec2SymbolTable(x->m_firstchild,newType);
		else
			AddStructVar2SymbolTable(x->m_lineno,(x->m_firstchild)->IDname,newType);

	}
	//printf("end AddVarDec\n");
}


void InsertStructDef(char* structname,FieldList structfield){
	StructList temp=malloc(sizeof(struct StructList_));
	temp->next=GlobalStructList;
	temp->name=structname;
	temp->member=structfield;
	GlobalStructList=temp;
}

void AddFunc2SymbolTable(int lineno,char* funcname,Type typex,FieldList fieldlistx){
	SymbolTableEntry tempindex;
	tempindex=SearchSymbolTable(funcname,0);
	if (tempindex!=NULL){
		printf("Error type 4 at Line %d: Redefined function '%s'.\n",lineno,funcname);
	}
	SymbolTableEntry temp=malloc(sizeof(struct SymbolTableEntry_));
	temp->kind=FUNC;
	temp->id=funcname;
	temp->lineno=lineno;
	temp->u.Function.RetType=typex;
	temp->u.Function.InputType=fieldlistx;
	temp->next=GlobalSymbolTable;
	GlobalSymbolTable=temp;
}

void AddFieldList2SymbolTable(FieldList x,int lineno){
	//PrintFieldList(x);
	FieldList temp=x;
	while (temp!=NULL){
		//		printf("GEt\n");

		SymbolTableEntry news=malloc(sizeof(struct SymbolTableEntry_));
		//printf("%s\n",temp->name);
		news->id=malloc(strlen(temp->name)+1);
		strcpy(news->id,temp->name);
		//printf("GEt\n");

		news->lineno=lineno;
		news->kind=VAR;
		news->u.Variable.VariableType=temp->type;
		news->next=GlobalSymbolTable;
		GlobalSymbolTable=news;
		temp=temp->next;
	}
//	printf("Finished AddFieldList\n");
}

void AddFuncDec2SymbolTable(ParserTreeNode* x,Type typex){
	FieldList temp=NULL;
	if (GetithChild(x,3)->m_SyntaxType==AVarList){
		temp=CovertVarList2FieldList(GetithChild(x,3));
	}
	//printf("Here\n");
	if (temp!=NULL)
		AddFieldList2SymbolTable(CovertVarList2FieldList(GetithChild(x,3)),x->m_lineno);
	AddFunc2SymbolTable(x->m_lineno,GetithChild(x,1)->IDname,typex,temp);
}

FieldList CovertVarList2FieldList(ParserTreeNode* x){
	if (x->m_childrennum==1)
		return CovertParamDec2FieldList(x->m_firstchild);
	else{
		FieldList newfield=CovertParamDec2FieldList(x->m_firstchild);
		FieldList temp=newfield;
		while (temp->next!=NULL)
			temp=temp->next;
		temp->next=CovertVarList2FieldList(GetithChild(x,3));
	}
}
FieldList CovertParamDec2FieldList(ParserTreeNode* x){
	Type typex=CovertSpecifier2Type(GetithChild(x,1));
	return AddVarDec2FieldList(GetithChild(x,2),typex,NULL);
}

void AddStructVar2SymbolTable(int lineno,char* varname,Type typex){
	//printf("Add Var %s\n",varname);
	SymbolTableEntry tempindex;
	tempindex=SearchSymbolTable(varname,1);
	if (tempindex!=NULL){
		printf("Error type 17 at Line %d: Redefined field '%s'.\n",lineno,varname);
	}
	

	SymbolTableEntry temp=malloc(sizeof(struct SymbolTableEntry_));
	temp->kind=VAR;
	temp->id=varname;
	temp->lineno=lineno;
	temp->u.Variable.VariableType=typex;
	temp->next=GlobalSymbolTable;
	GlobalSymbolTable=temp;
}

void AddVar2SymbolTable(int lineno,char* varname,Type typex){
	//printf("Add Var %s\n",varname);
	SymbolTableEntry tempindex;
	tempindex=SearchSymbolTable(varname,1);
	if (tempindex!=NULL){
		printf("Error type 3 at Line %d: Redefined variable '%s'.\n",lineno,varname);
	}
	

	SymbolTableEntry temp=malloc(sizeof(struct SymbolTableEntry_));
	temp->kind=VAR;
	temp->id=varname;
	temp->lineno=lineno;
	temp->u.Variable.VariableType=typex;
	temp->next=GlobalSymbolTable;
	GlobalSymbolTable=temp;
}

FieldList SearchStructList(char* structname){
	StructList temp=GlobalStructList;
	while (temp!=NULL){
		if (strcmp(structname,temp->name)==0)
			return temp->member;
		temp=temp->next;
	}
	return NULL;
}

void PrintStructList(StructList x){
	if (x==NULL)
		return;
	printf("Name:%s",x->name);
	PrintFieldList(x->member);
	PrintStructList(x->next);

}

void PrintType(Type x){
	printf("{");
	if (x->kind==BASIC){
		int i;
		if (x->u.basic==0)
			printf("INT");
		else
			printf("FLOAT");
	}
	else if (x->kind==ARRAY){
		printf("[%d]->",x->u.array.size);
		PrintType(x->u.array.elem);
	}
	else {

		PrintFieldList(x->u.structure.memberlist);
	}
	printf("}\n");
}

void PrintFieldList(FieldList x){
	printf("{");
	FieldList temp=x;
	while (temp!=NULL){
		printf("Name:%s",temp->name);
		PrintType(temp->type);
		temp=temp->next;
	}
	printf("}\n");
}

SymbolTableEntry SearchSymbolTable(char* idx,int kind){
	SymbolTableEntry temp=GlobalSymbolTable;

	while (temp!=NULL){
		//printf("%s  %s\n",temp->id,idx);
		if (strcmp(idx,temp->id)==0 /*&& kind==temp->kind*/){
			return temp;
		}
		temp=temp->next;

	}
	return NULL;
}


Type GetExpType(ParserTreeNode* x){
	if (x->m_childrennum==1){
		if (GetithChild(x,1)->m_SyntaxType==AID){
			SymbolTableEntry tempindex=SearchSymbolTable((x->m_firstchild)->IDname,1);
			if (tempindex==NULL) return NULL;
			return tempindex->u.Variable.VariableType;
		}
		if (GetithChild(x,1)->m_SyntaxType==AINT){
			Type newtype=malloc(sizeof(Type_));
			newtype->kind=BASIC;
			newtype->u.basic=0;
			return newtype;
		}
		if (GetithChild(x,1)->m_SyntaxType==AFLOAT){
			Type newtype=malloc(sizeof(Type_));
			newtype->kind=BASIC;
			newtype->u.basic=1;
			return newtype;
		}
	}

	if (x->m_childrennum==2){
		if (GetithChild(x,1)->m_SyntaxType==AMINUS){
			Type temp=GetExpType(GetithChild(x,2));
			if (temp==NULL) return NULL;
			if (temp->kind!=BASIC) return NULL;
			return temp;
		}

		if (GetithChild(x,1)->m_SyntaxType==ANOT){
			Type temp=GetExpType(GetithChild(x,2));
			if (temp==NULL) return NULL;
			if (temp->kind!=BASIC||temp->u.basic==1) return NULL;
			return temp;
		}

	}

	if (x->m_childrennum==3){
		if (GetithChild(x,2)->m_SyntaxType==AAND||GetithChild(x,2)->m_SyntaxType==AOR){
			Type temp=GetExpType(GetithChild(x,1));
			if (temp==NULL) return NULL;
			if (temp->kind!=BASIC||temp->u.basic==1) return NULL;
			temp=GetExpType(GetithChild(x,3));
			if (temp==NULL) return NULL;
			if (temp->kind!=BASIC||temp->u.basic==1) return NULL;
			return temp;
		}

		if (GetithChild(x,2)->m_SyntaxType==ARELOP
			||GetithChild(x,2)->m_SyntaxType==APLUS
			||GetithChild(x,2)->m_SyntaxType==AMINUS
			||GetithChild(x,2)->m_SyntaxType==ASTAR
			||GetithChild(x,2)->m_SyntaxType==ADIV
			||GetithChild(x,2)->m_SyntaxType==AASSIGNOP){
			Type temp1=GetExpType(GetithChild(x,1));
			if (temp1==NULL) return NULL;
			if (temp1->kind!=BASIC) return NULL;
			Type temp2=GetExpType(GetithChild(x,3));
			if (temp2==NULL) return NULL;
			if (temp2->kind!=BASIC) return NULL;
			if (temp1->u.basic!=temp2->u.basic) return NULL;
			return temp1;
		}

		if (GetithChild(x,2)->m_SyntaxType==AExp){
			Type temp=GetExpType(GetithChild(x,2));
			return temp;
		}

		if (GetithChild(x,1)->m_SyntaxType==AExp){
			Type temp=GetExpType(GetithChild(x,1));
			if (temp==NULL) return NULL;
			if (temp->kind!=STRUCTURE) return NULL;
			Type temp1=GetTypeFromFieldList(GetithChild(x,3)->IDname,temp->u.structure.memberlist);
			return temp1;
		}

		if (GetithChild(x,1)->m_SyntaxType==AID){
			SymbolTableEntry tempindex=SearchSymbolTable((x->m_firstchild)->IDname,0);
			if (tempindex==NULL) return NULL;
			return tempindex->u.Function.RetType;
		}
	}
	if (x->m_childrennum==4){
		if (GetithChild(x,1)->m_SyntaxType==AID){
			SymbolTableEntry tempindex=SearchSymbolTable((x->m_firstchild)->IDname,0);
			if (tempindex==NULL) return NULL;
			return tempindex->u.Function.RetType;
		}
		if (GetithChild(x,1)->m_SyntaxType==AExp){
			Type temp1=GetExpType(GetithChild(x,1));
			Type temp2=GetExpType(GetithChild(x,2));
			if (temp1==NULL||temp2==NULL) return NULL;
			if (temp2->kind!=BASIC&&temp2->u.basic==1) return NULL;
			if (temp1->kind!=ARRAY) return NULL;
			return temp1->u.array.elem;
		}
	}
}