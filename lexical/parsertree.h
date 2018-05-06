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

struct ParserNode
{
    NodeType m_NodeType;
    SyntaxType m_SyntaxType;
    int m_lineno;
    int m_depth;
    int m_childrennum;
    struct ParsingNode* m_firstchild;
    struct ParsingNode* m_parent;
    struct ParsingNode* m_nextsibiling;
	union
	{
		char* IDname; 
		TypeType type_value; 
    		int int_value; 
    		float float_value; 
    		RelopType relop_value; 
	};   
};
typedef struct ParserNode ParserNode;

extern ParserNode* root;

ParserNode* GenerateVariableNode(SyntaxType typex,int childrenNum,...);
ParserNode* GenerateTypeNode(int lineno,char* text);
ParserNode* GenerateIntNode(int lineno,char* text);
ParserNode* GenerateFloatNode(int lineno,char* text);
ParserNode* GenerateDummyNode(SyntaxType typex);
ParserNode* GenerateRelopNode(int lineno,char* text);
ParserNode* GenerateNormalTerminalNode(int lineno,SyntaxType typex);
ParserNode* GenerateIDNode(int lineno,char* text);
#endif
