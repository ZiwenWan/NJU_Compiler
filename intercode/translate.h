#ifndef __TRANSLATE_H__
#define __TRANSLATE_H__
#include "intercode.h"
#include "../lexical/parsertree.h"

ICEntry transExtDefList(ParserTreeNode* x);
ICEntry transProgram(ParserTreeNode* x);
ICEntry transExtDef(ParserTreeNode* x);
ICEntry transFuncDec(ParserTreeNode* x);
ICEntry transVarList(ParserTreeNode* x);
ICEntry transCompSt(ParserTreeNode* x);

ICEntry transDefList(ParserTreeNode* x);
ICEntry transDef(ParserTreeNode* x);
ICEntry transStmtList(ParserTreeNode* x);
ICEntry transStmt(ParserTreeNode* x);
ICEntry transExp(ParserTreeNode* x,Operand place);
ICEntry transCond(ParserTreeNode* x,int label_true,int label_false);
Operand GetLeftAssignOp(ParserTreeNode* x,ICEntry);

ICEntry transArgs(ParserTreeNode* x,ArgEntry targlist);
void InsertOp2ArgList(Operand x,ArgEntry dst);
ICEntry transVarDec(ParserTreeNode* x,int);
ICEntry transParamDec(ParserTreeNode* x);
ICEntry transVarList(ParserTreeNode* x);
ICEntry transDecList(ParserTreeNode* x);
ICEntry transDec(ParserTreeNode* x);
#endif