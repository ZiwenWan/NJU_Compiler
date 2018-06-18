#include "symbolTable.h"
#include <stdio.h>
#include <stdlib.h>
#include "covert.h"
#include "../lexical/parsertree.h"

FieldList CovertDef2FieldList(ParserTreeNode* x){
	//printf("Here\n");

	ParserTreeNode* temp=x->m_firstchild;
	Type inittype=CovertSpecifier2Type(temp);
	temp=temp->m_nextsibiling;
	FieldList ret=CovertDecList2FieldList(temp,inittype);
	return ret;	
}

FieldList AddDec2FieldList(ParserTreeNode* x,Type y,FieldList z){
	FieldList ret;
	if (x->m_childrennum==1){
		ret=AddVarDec2FieldList(x->m_firstchild,y,z);
	}
}

FieldList AddVarDec2FieldList(ParserTreeNode* x,Type y,FieldList z){
	//printf("Begin Covert VarDec\n");
	FieldList newField=malloc(sizeof(FieldList_));
	if (x->m_childrennum==1){
		//FieldList newField=malloc(sizeof(struct FieldList_));
		newField->name=(x->m_firstchild)->IDname;
		newField->type=y;
		newField->next=z;
		return newField;
	}
	else{
		ParserTreeNode* temp=x->m_firstchild;	
		temp=temp->m_nextsibiling;
		temp=temp->m_nextsibiling;
		Type newType=malloc(sizeof(Type_));
		newType->kind=ARRAY;
		newType->u.array.size=temp->int_value;
		newType->u.array.elem=y;
		return AddVarDec2FieldList(x->m_firstchild,newType,z);
	}
	//printf("End Covert VarDec\n");
}

FieldList CovertDecList2FieldList(ParserTreeNode* x,Type y){
	ParserTreeNode* temp=x->m_firstchild;
	FieldList ret=AddDec2FieldList(temp,y,NULL);
	FieldList tail=ret;
	while (tail->next!=NULL)
		tail=tail->next;
	if (temp->m_nextsibiling!=NULL){
		temp=temp->m_nextsibiling;
		temp=temp->m_nextsibiling;
		tail->next=CovertDecList2FieldList(temp,y);
	}
	return ret;
}

FieldList CovertDefList2FieldList(ParserTreeNode* x){
	if (x->m_NodeType==Dummy) return NULL;
	ParserTreeNode* temp=x->m_firstchild;
	FieldList ret=CovertDef2FieldList(temp);
	FieldList tf=ret;
	while(tf->next!=NULL)
		tf=tf->next;
	tf->next=CovertDefList2FieldList(temp->m_nextsibiling);
	return ret;

}



Type CovertSpecifier2Type(ParserTreeNode* x){
	ParserTreeNode* temp=x->m_firstchild;
	Type ret;
	if (temp->m_SyntaxType==ATYPE){
		ret=malloc(sizeof(Type_));
		ret->kind=BASIC;
		ret->u.basic=temp->type_value;
	}
	else{
		ret=CovertStructSpecifier2Type(temp);
	}
	return ret;
}

Type CovertStructSpecifier2Type(ParserTreeNode* x){


	if (x->m_SyntaxType==AStructSpecifier){
		if (GetithChild(x,2)->m_SyntaxType==ATag){
			if (SearchStructList(GetithChild(GetithChild(x,2),1)->IDname)==NULL){
				printf("Error type 17 at Line %d: Undefined structure.\n",x->m_lineno);
				return NULL;
			}
		}
		if (GetithChild(x,2)->m_SyntaxType==AOptTag){
			
			if (GetithChild(x,2)->m_childrennum!=0)
			if (SearchStructList(GetithChild(GetithChild(x,2),1)->IDname)!=NULL){
				printf("Error type 16 at Line %d: Duplicated name structure.\n",x->m_lineno);
			}
		}
	}
	ParserTreeNode* temp=x->m_firstchild;
	temp=temp->m_nextsibiling;
	Type ret;
	if (temp->m_SyntaxType==ATag){
		char* structname=getIDfromTag(temp);
		FieldList tf=SearchStructList(structname);
		ret=malloc(sizeof(Type_));
		ret->kind=STRUCTURE;
		ret->u.structure.name=structname;
		ret->u.structure.memberlist=tf;
	}
	else{
		char* structname=getIDfromOptTag(temp);
		//printf("!!!\n");
		temp=temp->m_nextsibiling;
		temp=temp->m_nextsibiling;
		FieldList tf=CovertDefList2FieldList(temp);
		InsertStructDef(structname,tf);
		ret=malloc(sizeof(Type_));
		ret->kind=STRUCTURE;
		ret->u.structure.name=structname;
		ret->u.structure.memberlist=tf;
	}
	return ret;
}
