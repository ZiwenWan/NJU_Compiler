#include "intercode.h"
#include <stdlib.h>
#include <string.h>

ICVarEntry GlobalICVarList=NULL;
ICFunEntry GlobalICFunList=NULL;
int GlobalTIndex=1,GlobalVIndex=0,GlobalLIndex=1;
ICEntry GlobalICList=NULL;

void InitGlobalICVarList(){
	SymbolTableEntry temp=GlobalSymbolTable;
	while(temp!=NULL){
		//printf("%s\n",temp->id);
		if (temp->kind==VAR &&
			temp->u.Variable.VariableType->kind==BASIC){
			InsertNewICVar(temp->id,temp->u.Variable.VariableType);
		}
		if (temp->kind==ARRAY){
			InsertNewICVar(temp->id,temp->u.Variable.VariableType);

		}

		temp=temp->next;
	}	
}

void InsertNewICVar(char* name,Type typex){
	//printf("Insert%s\n",name);
	GlobalVIndex++;
	ICVarEntry temp=malloc(sizeof(struct ICVarEntry_));
	temp->VarType=typex;
	temp->VarName=malloc(strlen(name)+1);
	strcpy(temp->VarName,name);
	temp->VIndex=GlobalVIndex;
	temp->attr=IVALUE;
	temp->next=GlobalICVarList;
	GlobalICVarList=temp;
}

Operand GetVarOp(char* name){
	ICVarEntry temp=GlobalICVarList;
	while (temp!=NULL){
		if (strcmp(temp->VarName,name)==0){
			Operand t=malloc(sizeof(struct Operand_));
			t->kind=IVARIABLE;
			t->attr=IVALUE;
			t->VIndex=temp->VIndex;
			return t;
		}
		temp=temp->next;
	}
}


ICEntry MergeInterCode(ICEntry x1,ICEntry x2){
	ICEntry temp=x1;
	if (x1==NULL) 
		return x2;
	if (x2==NULL)
		return x1;
	while (temp->next!=NULL)
		temp=temp->next;
	x2->prev=temp;
	temp->next=x2;
	return x1;
}

ICEntry NewICEntry(ICEntry head){
	ICEntry t1=malloc(sizeof(struct ICEntry_));
	InterCode t2=malloc(sizeof(struct InterCode_));
	t1->cur=t2;
	if (head!=NULL){
		ICEntry temp=head;
		while (temp->next!=NULL)
			temp=temp->next;
		temp->next=t1;
		t1->prev=temp;
	} 
	return t1;

};



ICEntry GenFunIC(char* funname,ICEntry head){
	ICEntry ret=NewICEntry(head);
	ret->cur->kind=IFUN;
	ret->cur->FUN.funcname=malloc(strlen(funname)+1);
	strcpy(ret->cur->FUN.funcname,funname);
	return ret;
}

ICEntry NewLabelIC(int label,ICEntry head){
	ICEntry ret=NewICEntry(head);
	ret->cur->kind=ILABELDEC;
	ret->cur->LABELDEC.LIndex=label;
	return ret;
}

ICEntry NewAssignIC(Operand x1,Operand x2,ICEntry head){
	ICEntry ret=NewICEntry(head);
	ret->cur->kind=IASSIGN;
	ret->cur->ASSIGN.left=x1;
	ret->cur->ASSIGN.right=x2;
	return ret;
}

Operand NewTOperand(int attr){
	Operand ret=malloc(sizeof(struct Operand_));
	ret->kind=ITEMP;
	ret->attr=attr;
	ret->TIndex=GlobalTIndex;
	GlobalTIndex++;
	return ret;
}

Operand NewConOperand(int value){
	Operand ret=malloc(sizeof(struct Operand_));
	ret->kind=ICONSTANT;
	ret->attr=IVALUE;
	ret->ICons=value;
}

void printICEntry(ICEntry x){
	ICEntry temp=x;
	while (temp!=NULL){
		printInterCode(temp->cur);
		temp=temp->next;
	}
}

void printInterCode(InterCode x){
	//printf("%X %d\n",x,x->kind);
	
	if (x->kind==IFUN){
		printf("FUNCTION %s :\n",x->FUN.funcname);
	}
	if (x->kind==IRETURN){
		printf("RETURN ");
		printOperand(x->RETURN.ret);
		printf("\n");
	}
	if (x->kind==IASSIGN){
		printOperand(x->ASSIGN.left);
		printf(" := ");
		printOperand(x->ASSIGN.right);
		printf("\n");
	}
	if (x->kind==IPLUS){
		printOperand(x->BINOP.result);
		printf(" := ");
		printOperand(x->BINOP.op1);
		printf(" + ");
		printOperand(x->BINOP.op2);
		printf("\n");	
	}
	if (x->kind==IMINUS){
		printOperand(x->BINOP.result);
		printf(" := ");
		printOperand(x->BINOP.op1);
		printf(" - ");
		printOperand(x->BINOP.op2);
		printf("\n");	
	}
	if (x->kind==ISTAR){
		printOperand(x->BINOP.result);
		printf(" := ");
		printOperand(x->BINOP.op1);
		printf(" * ");
		printOperand(x->BINOP.op2);
		printf("\n");	
	}
	if (x->kind==IDIV){
		printOperand(x->BINOP.result);
		printf(" := ");
		printOperand(x->BINOP.op1);
		printf(" / ");
		printOperand(x->BINOP.op2);
		printf("\n");	
	}
	if (x->kind==IGOTO){
		printf("GOTO label%d\n",x->GOTO.LIndex);
	}
	if (x->kind==IIFGOTO){
		printf("IF "); printOperand(x->IFGOTO.cond->op1);
		printRelop(x->IFGOTO.cond->relop); printOperand(x->IFGOTO.cond->op2);
		printf(" GOTO label%d\n",x->IFGOTO.LIndex);
	}
	if (x->kind==ILABELDEC){
		printf("LABEL label%d :\n",x->LABELDEC.LIndex);
	}
	if (x->kind==IREAD){
		printf("READ ");
		printOperand(x->READ.input);
		printf("\n");
	}
	if (x->kind==IWRITE){
		printf("WRITE ");
		printOperand(x->WRITE.output);
		printf("\n");
	}
	if (x->kind==ICALL){
		printOperand(x->CALL.ret);
		printf(" := CALL %s\n",x->CALL.funName);
	}
	if (x->kind==IARG){
		printf("ARG ");
		printOperand(x->ARG.argument);
		printf("\n");
	}
	if (x->kind==IPARAM){
		printf("PARAM ");
		printOperand(x->PARAM.parameter);
		printf("\n");
	}
}

void printRelop(int x){
	switch(x){
		case INEQ:
		printf(" != ");
		break;
		case IEQ:
		printf(" == ");
		break;
		case IGT:
		printf(" > ");
		break;
		case ILT:
		printf(" < ");
		break;
		case ILEQ:
		printf(" <= ");
		break;
		case IGEQ:
		printf(" >= ");
		break;
	}
}

void printOperand(Operand x){
	if (x->attr==IADDRESS)
		printf("*");
	else if (x->attr==IREFER)
		printf("&");
	if (x->kind==ICONSTANT){
		printf("#%d",x->ICons);
	}
	if (x->kind==ITEMP)
		printf("t%d",x->TIndex);
	if (x->kind==IVARIABLE){
		printf("v%d",x->VIndex);
	}
}

int NewLabel(){
	return GlobalLIndex++;
}