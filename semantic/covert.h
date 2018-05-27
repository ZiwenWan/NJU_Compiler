#ifndef __COVERT_H__
#define __COVERT_H__

#include "symbolTable.h"
#include "../lexical/parsertree.h"


FieldList CovertDef2FieldList(ParserTreeNode* x);
FieldList AddDec2FieldList(ParserTreeNode* x,Type y,FieldList z);
FieldList AddVarDec2FieldList(ParserTreeNode* x,Type y,FieldList z);
FieldList CovertDecList2FieldList(ParserTreeNode* x,Type y);
FieldList CovertDefList2FieldList(ParserTreeNode* x);
Type CovertSpecifier2Type(ParserTreeNode* x);
Type CovertStructSpecifier2Type(ParserTreeNode* x);

FieldList AddVarDec2FieldList(ParserTreeNode* x,Type y,FieldList z);

#endif
