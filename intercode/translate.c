#include "translate.h"
#include "stdlib.h"
#include "string.h"

ICEntry transExtDefList(ParserTreeNode* x){
	if (x->m_childrennum==0)
		return NULL;
	ParserTreeNode* temp=x->m_firstchild;
	ICEntry list1=transExtDef(temp);
	ICEntry list2=transExtDefList(temp->m_nextsibiling);
	return MergeInterCode(list1,list2);
}

ICEntry transProgram(ParserTreeNode* x){
	return transExtDefList(x->m_firstchild);
}

ICEntry transExtDef(ParserTreeNode* x){
	if (x->m_childrennum==2)
		return NULL;
	if (GetithChild(x,2)->m_SyntaxType!=AFunDec)
		return NULL;
	ICEntry list1=transFuncDec(GetithChild(x,2));
	ICEntry list2=transCompSt(GetithChild(x,3));
	return MergeInterCode(list1,list2);
}

ICEntry transFuncDec(ParserTreeNode* x){
	ICEntry list1=GenFunIC((x->m_firstchild)->IDname,NULL);
	if (x->m_childrennum==4){
		ICEntry list2=transVarList(GetithChild(x,3));
		list1=MergeInterCode(list1,list2);
	}
	//printf("End Trans FuncDec\n");
	return list1;
}



ICEntry transVarList(ParserTreeNode* x){
	if (x->m_childrennum==1)
		return transParamDec(GetithChild(x,1));
	else{
		return MergeInterCode(transParamDec(GetithChild(x,1)),transVarList(GetithChild(x,3)));
	}
}

ICEntry transParamDec(ParserTreeNode* x){
	return transVarDec(GetithChild(x,2));
}

ICEntry transVarDec(ParserTreeNode* x){
	ICEntry temp_ice=NewICEntry(NULL);
	temp_ice->cur->kind=IPARAM;
	temp_ice->cur->PARAM.parameter=GetVarOp(GetithChild(x,1)->IDname);
	return temp_ice;
}

ICEntry transCompSt(ParserTreeNode* x){
	printf("Begin Trans Compst\n");
	ICEntry list1=transDefList(GetithChild(x,2));
	ICEntry list2=transStmtList(GetithChild(x,3));
	return MergeInterCode(list1,list2);
	printf("End Trans Compst");
}

ICEntry transDefList(ParserTreeNode* x){
	if (x->m_childrennum==0)
		return NULL;
	ICEntry list1=transDef(GetithChild(x,1));
	ICEntry list2=transDefList(GetithChild(x,2));
	return MergeInterCode(list1,list2);
}

ICEntry transDef(ParserTreeNode* x){
	return NULL;
}

ICEntry transStmtList(ParserTreeNode* x){
	if (x->m_childrennum==0)
		return NULL;
	ICEntry list1=transStmt(GetithChild(x,1));
	ICEntry list2=transStmtList(GetithChild(x,2));
	return MergeInterCode(list1,list2);
}

ICEntry transStmt(ParserTreeNode* x){
	if (x->m_childrennum==1){
		return transCompSt(GetithChild(x,1));
	}
	else if (x->m_childrennum==2){
		return transExp(GetithChild(x,1),NULL);
	}
	else if (x->m_childrennum==3){
		Operand t=NewTOperand(IVALUE);
		ICEntry list1=transExp(GetithChild(x,2),t);
		//printf("Break1\n");
		ICEntry temp_ice=NewICEntry(list1);
		//printf("Break2\n");
		temp_ice->cur->kind=IRETURN;
		temp_ice->cur->RETURN.ret=t;
		//printf("Break3\n");
		return list1;
	}
	else if (x->m_childrennum==5){
		if (GetithChild(x,1)->m_SyntaxType==AIF){
			int l1=NewLabel();
			int l2=NewLabel();
			ICEntry list1=transCond(GetithChild(x,3),l1,l2);
			NewLabelIC(l1,list1);
			ICEntry list2=transStmt(GetithChild(x,5));
			list1=MergeInterCode(list1,list2);
			NewLabelIC(l2,list1);
			return list1;
		}
		if (GetithChild(x,1)->m_SyntaxType==AWHILE){
			int l1=NewLabel(); int l2=NewLabel(); int l3=NewLabel();
			ICEntry list1=transCond(GetithChild(x,3),l1,l2);
			ICEntry list2=transStmt(GetithChild(x,5));
			ICEntry ret=NewICEntry(NULL);
			ret->cur->kind=ILABELDEC;
			ret->cur->LABELDEC.LIndex=l1;
			ret=MergeInterCode(ret,list1);
			NewLabelIC(l2,ret);
			ret=MergeInterCode(ret,list2);
			ICEntry temp_ice=NewICEntry(ret);
			temp_ice->cur->kind=IGOTO;
			temp_ice->cur->GOTO.LIndex=l1;
			NewLabelIC(l3,ret);
			return ret;
		}
	}
	else if (x->m_childrennum==7){
		if (GetithChild(x,1)->m_SyntaxType==AIF){
			//printf("Begin Trans IF\n");
			int l1=NewLabel(); int l2=NewLabel(); int l3=NewLabel();
			ICEntry list1=transCond(GetithChild(x,3),l1,l2);
			//printf("Break 1\n");
			ICEntry list2=transStmt(GetithChild(x,5));
			//printf("Break 2\n");
			ICEntry list3=transStmt(GetithChild(x,7));
			//printf("Middle IF\n");
			NewLabelIC(l1,list1);
			ICEntry temp_ice=NewICEntry(list2);
			temp_ice->cur->kind=IGOTO;
			temp_ice->cur->GOTO.LIndex=l3;
			NewLabelIC(l2,list2);
			NewLabelIC(l3,list3);
			list1=MergeInterCode(list1,list2);
			list1=MergeInterCode(list1,list3);
			//printf("End Trans IF\n");
			return list1;
		}
	}
	return NULL;
}

ICEntry transExp(ParserTreeNode* x,Operand place){
	if (x->m_childrennum==1){

		if (GetithChild(x,1)->m_SyntaxType==AINT){
			if (place==NULL) return NULL;
			ICEntry ret=NewICEntry(NULL);
			ret->cur->kind=IASSIGN;
			ret->cur->ASSIGN.left=place;
			ret->cur->ASSIGN.right=NewConOperand(GetithChild(x,1)->int_value);
			return ret;
		}
		if (GetithChild(x,1)->m_SyntaxType==AID){
			if (place==NULL) return NULL;
			ICEntry ret=NewICEntry(NULL);
			ret->cur->kind=IASSIGN;
			ret->cur->ASSIGN.left=place;
			ret->cur->ASSIGN.right=GetVarOp(GetithChild(x,1)->IDname);
			return ret;
		}
	}
	if (x->m_childrennum==2){
		if (GetithChild(x,1)->m_SyntaxType==AMINUS){
			if (place==NULL) return NULL;
			Operand t1=NewTOperand(IVALUE);
			ICEntry list1=transExp(GetithChild(x,2),t1);
			ICEntry temp_ice=NewICEntry(list1);
			temp_ice->cur->kind=IMINUS;
			temp_ice->cur->BINOP.result=place;
			temp_ice->cur->BINOP.op1=NewConOperand(0);
			temp_ice->cur->BINOP.op2=t1;
			return list1;
		}

		if (GetithChild(x,1)->m_SyntaxType==ANOT){
			if (place=NULL) return NULL;
			int l1=NewLabel();
			int l2=NewLabel();
			ICEntry list1=NewICEntry(NULL);
			list1->cur->kind=IASSIGN;
			Operand temp_op=malloc(sizeof(struct Operand_));
			temp_op->kind=ICONSTANT;
			temp_op->attr=IVALUE;
			temp_op->ICons=0;
			list1->cur->ASSIGN.left=place;
			list1->cur->ASSIGN.right=temp_op;
			list1=MergeInterCode(list1,transCond(x,l1,l2));
			NewLabelIC(l1,list1);
			NewAssignIC(place,NewConOperand(1),list1);
			NewLabelIC(l2,list1);
			return list1;
		}
	} 

	if (x->m_childrennum==3){
		if (GetithChild(x,2)->m_SyntaxType==AExp){
			return transExp(GetithChild(x,2),place);
		}

		if (GetithChild(x,2)->m_SyntaxType==APLUS||
			GetithChild(x,2)->m_SyntaxType==AMINUS||
			GetithChild(x,2)->m_SyntaxType==ASTAR||
			GetithChild(x,2)->m_SyntaxType==ADIV){
			//printf("Begin Trans Star\n");
			Operand t1=NewTOperand(IVALUE);
			Operand t2=NewTOperand(IVALUE);
			ICEntry list1=transExp(GetithChild(x,1),t1);
			ICEntry list2=transExp(GetithChild(x,3),t2);
			list1=MergeInterCode(list1,list2);
			if (place==NULL) return list1;
			ICEntry temp_ice=NewICEntry(list1);
			temp_ice->cur->BINOP.op1=t1;
			temp_ice->cur->BINOP.op2=t2;
			temp_ice->cur->BINOP.result=place;
			switch(GetithChild(x,2)->m_SyntaxType){
				case APLUS:
					temp_ice->cur->kind=IPLUS;
					break;
				case AMINUS:
					temp_ice->cur->kind=IMINUS;
					break;
				case ASTAR:
					temp_ice->cur->kind=ISTAR;
					break;
				case ADIV:
					temp_ice->cur->kind=IDIV;
					break;
			}
			//printf("End Trans STar\n");
			return list1;

		}

		if (GetithChild(x,2)->m_SyntaxType==AOR||
			GetithChild(x,2)->m_SyntaxType==AAND||
			GetithChild(x,2)->m_SyntaxType==ARELOP){
			int l1=NewLabel();
			int l2=NewLabel();
			ICEntry list1=NewICEntry(NULL);
			list1->cur->kind=IASSIGN;
			Operand temp_op=malloc(sizeof(struct Operand_));
			temp_op->kind=ICONSTANT;
			temp_op->attr=IVALUE;
			temp_op->ICons=0;
			list1->cur->ASSIGN.left=place;
			list1->cur->ASSIGN.right=temp_op;
			list1=MergeInterCode(list1,transCond(x,l1,l2));
			NewLabelIC(l1,list1);
			NewAssignIC(place,NewConOperand(1),list1);
			NewLabelIC(l2,list1);
			return list1;
		}
		if (GetithChild(x,2)->m_SyntaxType==AASSIGNOP){
			Operand temp_op=GetLeftAssignOp(GetithChild(x,1));
			Operand t1=NewTOperand(IVALUE);
			ICEntry list1=transExp(GetithChild(x,3),t1);
			ICEntry temp_ice=NewICEntry(list1);
			temp_ice->cur->kind=IASSIGN;
			temp_ice->cur->ASSIGN.left=temp_op; 
			temp_ice->cur->ASSIGN.right=t1;

			if (place!=NULL){
				temp_ice=NewICEntry(list1);
				temp_ice->cur->kind=IASSIGN;
				temp_ice->cur->ASSIGN.left=place;
				temp_ice->cur->ASSIGN.right=t1;
			}			
			return list1;
		}

		if (GetithChild(x,1)->m_SyntaxType==AID){
			if (strcmp(GetithChild(x,1)->IDname,"read")==0){
				//printf("Handle Read\n");
				ICEntry temp_ice=NewICEntry(NULL);
				temp_ice->cur->kind=IREAD;
				if (place!=NULL)
					temp_ice->cur->READ.input=place;
				else temp_ice->cur->READ.input=NewTOperand(IVALUE);
				return temp_ice;
			}

			else{
				ICEntry temp_ice=NewICEntry(NULL);
				temp_ice->cur->kind=ICALL;
				temp_ice->cur->CALL.funName=malloc(strlen(GetithChild(x,1)->IDname)+1);
				strcpy(temp_ice->cur->CALL.funName,GetithChild(x,1)->IDname);
				if (place==NULL)
					temp_ice->cur->CALL.ret=NewTOperand(IVALUE);
				else 
					temp_ice->cur->CALL.ret=place;
				return temp_ice;
			}
		}
	}

	if (x->m_childrennum==4&&GetithChild(x,1)->m_SyntaxType==AID){
		//printf("Begin Trans FuncCall\n");
		ArgEntry targlist=malloc(sizeof(struct ArgEntry_));
		targlist->next=NULL;
		targlist->arg=NULL;
		ICEntry list1=transArgs(GetithChild(x,3),targlist);
		if (strcmp(GetithChild(x,1)->IDname,"write")==0){
			ICEntry temp_ice=NewICEntry(list1);
			temp_ice->cur->kind=IWRITE;
			temp_ice->cur->WRITE.output=targlist->next->arg;
			return list1;
		}
		else{
			ArgEntry temp_arg=targlist->next;
			while (temp_arg!=NULL){
				ICEntry temp_ice=NewICEntry(list1);
				temp_ice->cur->kind=IARG;
				temp_ice->cur->ARG.argument=temp_arg->arg;
				temp_arg=temp_arg->next;
			}
			ICEntry temp_ice=NewICEntry(list1);
			temp_ice->cur->kind=ICALL;
			if (place!=NULL)
				temp_ice->cur->CALL.ret=place;
			else
				temp_ice->cur->CALL.ret=NewTOperand(IVALUE);
			temp_ice->cur->CALL.funName=malloc(strlen(GetithChild(x,1)->IDname)+1);
			strcpy(temp_ice->cur->CALL.funName,GetithChild(x,1)->IDname);
			return list1;
		}
		//printf("End Trans FuncCall\n");
	}
	if (x->m_childrennum==4&&GetithChild(x,2)->m_SyntaxType==ALB){
		if (place==NULL) return NULL;
		printf("Begin ALB\n");
		Operand t1=NewTOperand(IVALUE);
		Operand base=GetVarOp(GetithChild(GetithChild(x,1),1)->IDname);
		base->attr=IREFER;
		ICEntry list1=NewICEntry(NULL);
		list1->cur->kind=IASSIGN;
		list1->cur->ASSIGN.left=t1;
		list1->cur->ASSIGN.right=base;

		Operand t2=NewTOperand(IVALUE);
		ICEntry list2=transExp(GetithChild(x,3),t2);

		list1=MergeInterCode(list1,list2);

		ICEntry temp_ice=NewICEntry(list1);
		temp_ice->cur->kind=ISTAR;
		temp_ice->cur->BINOP.result=t2;
		temp_ice->cur->BINOP.op1=t2;
		temp_ice->cur->BINOP.op2=NewConOperand(4);

		temp_ice=NewICEntry(NULL);
		temp_ice->cur->kind=IPLUS;
		temp_ice->cur->BINOP.result=t1;
		temp_ice->cur->BINOP.op1=base;
		temp_ice->cur->BINOP.op2=t2;

		temp_ice=NewICEntry(list1);
		temp_ice->cur->kind=IASSIGN;
		temp_ice->cur->ASSIGN.left=place;
		Operand temp_op=malloc(sizeof(struct Operand_));
		temp_op->kind=t1->kind;
		temp_op->TIndex=t1->TIndex;
		temp_op->attr=IADDRESS;

		temp_ice->cur->ASSIGN.right=temp_op;
		printf("End ALB\n");
		return list1;
	}
}

void InsertOp2ArgList(Operand x,ArgEntry dst){
	ArgEntry temp=dst->next;
	ArgEntry new_arg=malloc(sizeof(struct ArgEntry_));
	new_arg->next=temp;
	new_arg->arg=x;
	dst->next=new_arg;
}

ICEntry transArgs(ParserTreeNode* x,ArgEntry targlist){
	if (x->m_childrennum==1){
		Operand t1=NewTOperand(IVALUE);
		ICEntry list1=transExp(GetithChild(x,1),t1);
		InsertOp2ArgList(t1,targlist);
		return list1;
	}
	else{
		Operand t1=NewTOperand(IVALUE);
		ICEntry list1=transExp(GetithChild(x,1),t1);
		InsertOp2ArgList(t1,targlist);
		ICEntry list2=transArgs(GetithChild(x,3),targlist);
		return MergeInterCode(list1,list2);
	}
}

Operand GetLeftAssignOp(ParserTreeNode* x){
	if (x->m_childrennum==1 && GetithChild(x,1)->m_SyntaxType==AID){
		return GetVarOp(GetithChild(x,1)->IDname);
	}
}

ICEntry transCond(ParserTreeNode* x,int label_true,int label_false){
	if (x->m_childrennum==2&&
		GetithChild(x,1)->m_SyntaxType==ANOT){
		return transCond(GetithChild(x,2),label_false,label_true);
	}
	else if (x->m_childrennum==3&&
		GetithChild(x,2)->m_SyntaxType==AOR){
		int l1=NewLabel();
		ICEntry list1=transCond(GetithChild(x,1),label_true,l1);
		ICEntry list2=transCond(GetithChild(x,3),label_true,label_false);
		ICEntry temp_ice=NewICEntry(list1);
		temp_ice->cur->kind=ILABELDEC;
		temp_ice->cur->LABELDEC.LIndex=l1;
		return MergeInterCode(list1,list2);
	}
	else if (x->m_childrennum==3&&
		GetithChild(x,2)->m_SyntaxType==AAND){
		int l1=NewLabel();
		ICEntry list1=transCond(GetithChild(x,1),l1,label_false);
		ICEntry list2=transCond(GetithChild(x,3),label_true,label_false);
		ICEntry temp_ice=NewICEntry(list1);
		temp_ice->cur->kind=ILABELDEC;
		temp_ice->cur->LABELDEC.LIndex=l1;
		return MergeInterCode(list1,list2);
	}
	else if (x->m_childrennum==3&&
		GetithChild(x,2)->m_SyntaxType==ARELOP){
		Operand t1=NewTOperand(IVALUE);
		Operand t2=NewTOperand(IVALUE);
		ICEntry list1=transExp(GetithChild(x,1),t1);
		ICEntry list2=transExp(GetithChild(x,3),t2);
		list1=MergeInterCode(list1,list2);
		ICEntry temp_ice=NewICEntry(list1);
		temp_ice->cur->kind=IIFGOTO;
		temp_ice->cur->IFGOTO.LIndex=label_true;
		Cond temp_cond=malloc(sizeof(struct Cond_));
		temp_cond->op1=t1; temp_cond->op2=t2;
		temp_cond->relop=GetithChild(x,2)->relop_value;
		temp_ice->cur->IFGOTO.cond=temp_cond;
		temp_ice=NewICEntry(list1);
		temp_ice->cur->kind=IGOTO;
		temp_ice->cur->GOTO.LIndex=label_false;
		return list1;
	}
	else{
		Operand t1=NewTOperand(IVALUE);
		ICEntry list1=transExp(x,t1);
		ICEntry temp_ice=NewICEntry(list1);
		temp_ice->cur->kind=IIFGOTO;
		temp_ice->cur->IFGOTO.LIndex=label_true;
		Cond temp_cond=malloc(sizeof(struct Cond_));
		temp_cond->relop=INEQ;
		temp_cond->op1=t1;
		temp_cond->op2=NewConOperand(0);
		temp_ice->cur->IFGOTO.cond=temp_cond;
		temp_ice=NewICEntry(list1);
		temp_ice->cur->kind=IGOTO;
		temp_ice->cur->IFGOTO.LIndex=label_false;
		return list1;
	}
}