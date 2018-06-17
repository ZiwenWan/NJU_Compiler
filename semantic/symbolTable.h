#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__
#include "../lexical/parsertree.h"
typedef struct Type_ *Type;
typedef struct FieldList_ *FieldList;
typedef struct SymbolTableEntry_ *SymbolTableEntry;
typedef struct StructList_ *StructList;
typedef struct FieldList_ FieldList_;

struct Type_ {
	enum{BASIC,ARRAY,STRUCTURE} kind;
	union {
		int basic;
		struct {Type elem; int size;} array;
		struct {char* name;FieldList memberlist;} structure;
	} u;
};

typedef struct Type_ Type_;


struct FieldList_ {
	char* name;
	Type type;
	FieldList next;
};

struct SymbolTableEntry_ {
	char* id;
	int lineno;
	enum{FUNC=0,VAR} kind;
	union{
		struct{Type VariableType;} Variable;
		struct{Type RetType; FieldList InputType;} Function;
	}u;
	SymbolTableEntry next;
};

struct StructList_{
	char* name;
	FieldList member;
	StructList next;
};



typedef struct StructList_ StructList_;
//pedef struct FieldList_ FieldList_;
extern StructList GlobalStructList;
extern SymbolTableEntry GlobalSymbolTable;
void AddStructVar2SymbolTable(int lineno,char* varname,Type typex);

FieldList SearchStructList(char* structname);
void InsertStructDef(char* structname,FieldList structfield);
void PrintFieldList(FieldList x);
void PrintType(Type x);
void PrintStructList(StructList x);
void AddFunc2SymbolTable(int lineno,char* funcname,Type typex,FieldList fieldlistx);
void AddVar2SymbolTable(int lineno,char* varname,Type typex);
void AddFuncDec2SymbolTable(ParserTreeNode* x,Type typex);
void AddVarDec2SymbolTable(ParserTreeNode* x,Type typex);
void AddExtDecList2SymbolTable(ParserTreeNode* x,Type typex);
void ScanTree(ParserTreeNode* x);
ParserTreeNode* GetithChild(ParserTreeNode* x,int n);
SymbolTableEntry SearchSymbolTable(char* idx,int kind);
void AddDecList2SymbolTable(ParserTreeNode* x,Type typex);
FieldList CovertVarList2FieldList(ParserTreeNode* x);


Type GetTypeFromFieldList(char* idx,FieldList fx);
int typecmp(Type t1,Type t2);
FieldList CovertParamDec2FieldList(ParserTreeNode* x);
void handleExp(ParserTreeNode* x);
Type GetExpType(ParserTreeNode* x);
void initGlobalSymbolTable();
#endif