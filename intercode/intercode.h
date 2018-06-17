#ifndef __INTERCODE_H__
#define __INTERCODE_H__

#include <stdio.h>
#include "../semantic/symbolTable.h"
enum{IVARIABLE,ICONSTANT,ITEMP};
enum{IVALUE,IADDRESS,IREFER};
enum{ILABELDEC,IFUN,IASSIGN,IPLUS,IMINUS,ISTAR,IDIV,IGOTO,IIFGOTO,IRETURN,
	IDEC,IARG,ICALL,IPARAM,IREAD,IWRITE};
enum{IEQ=0, ILT, IGT, INEQ, ILEQ, IGEQ};

typedef struct ICVarEntry_ *ICVarEntry;
typedef struct ICFunEntry_ *ICFunEntry;
typedef struct InterCode_ *InterCode;
typedef struct Cond_ *Cond;
typedef struct Operand_ *Operand;
typedef struct ICEntry_ *ICEntry;
typedef struct ArgEntry_ *ArgEntry;

extern ICVarEntry GlobalICVarList;
extern ICFunEntry GlobalICFunList;
extern int GlobalTIndex,GlobalVIndex,GlobalLIndex;
extern ICEntry GlobalICList;
struct ICVarEntry_{
	char* VarName;
	Type VarType;
	int VIndex;
	int attr;
	ICVarEntry next;
};

struct ICFunEntry{
	char* Funname;
	ICFunEntry next;
};

struct ICEntry_{
	InterCode cur;
	ICEntry prev;
	ICEntry next;
};


struct Operand_{
	int kind;
	int attr;
	union{
		int VIndex;
		int TIndex;
		int ICons;
	};
};


struct Cond_{
	Operand op1,op2;
	int relop;
};

struct InterCode_{
	int kind;
	union{
		struct {int LIndex;} LABELDEC;
		struct {char* funcname;} FUN;
		struct {Operand right;Operand left;} ASSIGN;
		struct {Operand result; Operand op1; Operand op2;} BINOP;
		struct {int LIndex;} GOTO;
		struct {Cond cond; int LIndex;} IFGOTO;
		struct {Operand ret;} RETURN;
		struct {Operand address; int size;} DEC;
		struct {Operand argument;} ARG;
		struct {Operand ret;char* funName;} CALL;
		struct {Operand parameter;} PARAM;
		struct {Operand input;} READ;
		struct {Operand output;} WRITE;
	};
};

struct ArgEntry_{

	Operand arg;
	ArgEntry next;
};

ICEntry GenFunIC(char* funname,ICEntry head);
ICEntry MergeInterCode(ICEntry x1,ICEntry x2);
ICEntry NewICEntry(ICEntry head);
ICEntry NewLabelIC(int label,ICEntry head);
ICEntry NewAssignIC(Operand x1,Operand x2,ICEntry head);

int NewLabel();
Operand NewTOperand(int attr);
Operand NewConOperand(int value);
void printInterCode(InterCode x);
void printOperand(Operand x);
void printICEntry(ICEntry x);
void printRelop(int);

void InsertNewICVar(char*,Type);
Operand GetVarOp(char*);
void InitGlobalICVarList();

#endif