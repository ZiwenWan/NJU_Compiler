#ifndef __PARSERTREE__
#define __PARSERTREE__

enum  SyntaxType{AINT = 0, AFLOAT, AID, ASEMI, ACOMMA, AASSIGNOP, ARELOP, 
		APLUS, AMINUS, ASTAR, ADIV, AAND, AOR, ADOT, ANOT, ATYPE,
		ALP, ARP, ALB, ARB, ALC, ARC, ASTRUCT, ARETURN, AIF, AELSE, AWHILE,
		AProgram, AExtDefList, AExtDef, ASpecifier, AExtDecList,
		AVarDec, AStructSpecifier, AOptTag, ATag, AFunDec, AVarList, 
		AParamDec, ACompSt, AStmtList, AStmt, AExp, ADef, ADefList, 
		ADec, ADecList, AArgs};
typedef enum SyntaxType SyntaxType;
enum NodeType{ Terminal=0, Variable, Dummy };
typedef enum NodeType NodeType;
enum RelopType{ EQ=0, LT, GT, NEQ, LEQ, GEQ };
typedef enum RelopType RelopType;
enum TypeType{ TYPEINT=0,TYPEFLOAT};
typedef enum TypeType TypeType;
extern  char* symbolsTable[48];
extern  char* typeTable[2];

extern char* relopTable[6];

struct ParserTreeNode
{
    NodeType m_NodeType;
    SyntaxType m_SyntaxType;
    int m_lineno;
    int m_depth;
    int m_childrennum;
    struct ParserTreeNode* m_firstchild;
    struct ParserTreeNode* m_parent;
    struct ParserTreeNode* m_nextsibiling;
	union
	{
		char* IDname; 
		TypeType type_value; 
    		int int_value; 
    		float float_value; 
    		RelopType relop_value; 
	};   
};
typedef struct ParserTreeNode ParserTreeNode;

extern ParserTreeNode* root;

ParserTreeNode* GenerateVariableNode(SyntaxType typex,int childrenNum,...);
ParserTreeNode* GenerateTypeNode(int lineno,char* text);
ParserTreeNode* GenerateIntNode(int lineno,char* text);
ParserTreeNode* GenerateFloatNode(int lineno,char* text);
ParserTreeNode* GenerateDummyNode(SyntaxType typex);
ParserTreeNode* GenerateRelopNode(int lineno,char* text);
ParserTreeNode* GenerateNormalTerminalNode(int lineno,SyntaxType typex);
ParserTreeNode* GenerateIDNode(int lineno,char* text);
char* getIDfromOptTag(ParserTreeNode* x);
char* getIDfromTag(ParserTreeNode* x);
void PrintTree(ParserTreeNode* x,int d);
#endif
