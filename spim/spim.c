#include "spim.h"
#include "string.h"
#include "stdlib.h"

int VSize,TSize,VBegin,TBegin;
int* VOffset; int* TOffset;
Operand* temp_arg_list;
int argnum;




void init_SPIM(){
	TSize=GlobalTIndex-1;
	VSize=0;
	ICVarEntry temp;
	temp=GlobalICVarList;
	int Vnum=0;
	while (temp!=NULL){
		Vnum++;
		if (temp->VarType->kind==BASIC){
			VSize++;
		}		
		else
			VSize=VSize+temp->VarType->u.array.size;
		temp=temp->next;
	}	

	VOffset=(int*)malloc(sizeof(int*)*Vnum);
	TOffset=(int*)malloc(sizeof(int*)*TSize);

	temp=GlobalICVarList;
	Vnum=1; int offset=0;
	while (temp!=NULL){
		if (temp->VarType->kind==BASIC){
			*(VOffset+temp->VIndex)=offset;
			offset++;
		}		
		else{
			*(VOffset+temp->VIndex)=offset;
			offset=offset+temp->VarType->u.array.size;
		}
		Vnum++;
		temp=temp->next;
	}
	int i;
	for (i=1;i<=GlobalTIndex-1;i++)
		*(TOffset+i)=i-1;
	VBegin=0;

	TBegin=VBegin+VSize*4;
	temp_arg_list=(Operand*)malloc(sizeof(Operand*)*30);



	/*temp=GlobalICVarList;
	while (temp!=NULL){
		printf("%s :v%d,kind:%d\n",temp->VarName,temp->VIndex,temp->VarType->kind);
		temp=temp->next;
	}
	for (i=1;i<GlobalTIndex;i++)
		printf("t%d %d\n",i,-TSize*4-VSize*4-8+*(TOffset+i)*4+TBegin);
	for (i=1;i<Vnum;i++)
		printf("v%d %d\n",i,-TSize*4-VSize*4-8+*(VOffset+i)*4+VBegin);*/
}


int GetVTAddrRelFP(Operand op){
	if (op->kind==ITEMP)
		return TBegin+4*(*(TOffset+op->TIndex))-VSize*4-TSize*4-8;
	else
		return VBegin+4*(*(VOffset+op->VIndex))-TSize*4-VSize*4-8;
}

void ASSIGNGenerator(InterCode ic,FILE* stream){
	Operand left=ic->ASSIGN.left,right=ic->ASSIGN.right;
	if(right->kind == ICONSTANT)
    {
 	    int leftAddr = GetVTAddrRelFP(left);

    	if (left->attr==IVARIABLE){

        	fprintf(stream, "  li $t0, %d\n", right->ICons);
        	fprintf(stream, "  sw $t0, %d($fp)\n", leftAddr);
        }
        else
        if (left->attr==IADDRESS){
        	 fprintf(stream, "  li $t0, %d\n", right->ICons);
			 fprintf(stream, "  lw $t1, %d($fp)\n", leftAddr);
             fprintf(stream, "  sw $t0, 0($t1)\n");
        }
    }
    else
    {
        int rightAddr = GetVTAddrRelFP(right);
        int leftAddr = GetVTAddrRelFP(left);

        if(right->attr == IADDRESS)
        {
            fprintf(stream, "  lw $t0, %d($fp)\n", rightAddr);
            fprintf(stream, "  lw $t0, 0($t0)\n");

        }
        else if(right->attr == IREFER)
        {
            fprintf(stream, "  li $t0, %d\n", rightAddr);
            fprintf(stream, "  add $t0, $t0, $fp\n");
        }
        else
        {
            fprintf(stream, "  lw $t0, %d($fp)\n", rightAddr);
        }

        if(left->attr == IADDRESS)
        {
            fprintf(stream, "  lw $t1, %d($fp)\n", leftAddr);
            fprintf(stream, "  sw $t0, 0($t1)\n");

        }
        else
        {
            fprintf(stream, "  sw $t0, %d($fp)\n", leftAddr);
        }



    }
}

void IFGOTOGenerator(InterCode ic,FILE* stream){
	Operand op1=ic->IFGOTO.cond->op1;
	Operand op2=ic->IFGOTO.cond->op2;
//	printf("%d %d %d %d\n",op1->kind,op2->kind,op1->TIndex,op2->TIndex);
//	printf("%d %d\n",GetVTAddrRelFP(op1),GetVTAddrRelFP(op2));
	if (op1->kind==ICONSTANT){
		fprintf(stream,"  li $t0, %d\n",op1->ICons);
	}	
	else{
		int op1Addr=GetVTAddrRelFP(op1);
		fprintf(stream,"  lw $t0, %d($fp)\n",op1Addr);
	}

	if (op2->kind==ICONSTANT){
		fprintf(stream,"  li $t1, %d\n",op2->ICons);
	}	
	else{
		int op2Addr=GetVTAddrRelFP(op2);
		fprintf(stream,"  lw $t1, %d($fp)\n",op2Addr);
	}

	switch(ic->IFGOTO.cond->relop){
		case IEQ:fprintf(stream,"  beq ");break;
		case ILT:fprintf(stream,"  blt ");break;
		case IGT:fprintf(stream,"  bgt ");break;
		case INEQ:fprintf(stream,"  bne ");break;
		case ILEQ:fprintf(stream,"  ble ");break;
		case IGEQ:fprintf(stream,"  bge ");break;
	}	
	fprintf(stream,"$t0, $t1, label%d\n",ic->IFGOTO.LIndex);
}


void GOTOGenerator(InterCode ic,FILE* stream){
	fprintf(stream,"  j label%d\n",ic->GOTO.LIndex);
}

void BINOPGenerator(InterCode ic,FILE* stream){
	Operand op1,op2,result;
    op1 = ic->BINOP.op1;
    op2 = ic->BINOP.op2;
    result = ic->BINOP.result;

    int op1Addr, op2Addr, resultAddr;
    resultAddr = GetVTAddrRelFP(result);

    if(op1->kind == ICONSTANT)
    {
        fprintf(stream, "  li $t0, %d\n", op1->ICons);
    }
    else
    {
        op1Addr = GetVTAddrRelFP(op1);
        fprintf(stream, "  lw $t0, %d($fp)\n", op1Addr);
    }

    if(op2->kind == ICONSTANT)
    {
        fprintf(stream, "  li $t1, %d\n", op2->ICons);
    }
    else
    {
        op2Addr = GetVTAddrRelFP(op2);
        fprintf(stream, "  lw $t1, %d($fp)\n", op2Addr);
    }

    switch (ic->kind)
    {
        case IPLUS:fprintf(stream, "  add $t2, $t0, $t1\n");break;
        case IMINUS:fprintf(stream, "  sub $t2, $t0, $t1\n");break;
        case ISTAR:fprintf(stream, "  mul $t2, $t0, $t1\n");break;
        case IDIV:fprintf(stream, "  div $t0, $t1\n  mflo $t2\n");break;
    }
    if (result->attr==IVALUE)
    	fprintf(stream, "  sw $t2, %d($fp)\n", resultAddr);
    
}

void LABELDECGenerator(InterCode ic,FILE* stream){
	fprintf(stream,"  label%d:\n",ic->LABELDEC.LIndex);
}

void CALLGenerator(InterCode ic,FILE* stream){
	Operand op=ic->CALL.ret;
	int opAddr=GetVTAddrRelFP(op);
	fprintf(stream,"  jal %s\n",ic->CALL.funName);
	fprintf(stream,"  sw $v0, %d($fp)\n",opAddr);
}

void MachineCodePreparation(FILE* stream)
{

    fprintf(stream,
            ".data\n"
            "_prompt: .asciiz \"Enter an integer:\"\n"
            "_ret: .asciiz \"\\n\"\n"
            ".globl main\n"
            ".text\n"
            "read:\n"
            "  li $v0, 4\n"
            "  la $a0, _prompt\n"
            "  syscall\n"
            "  li $v0, 5\n"
            "  syscall\n"
            "  jr $ra\n\n"
            "write:\n"
            "  li $v0, 1\n"
            "  syscall\n"
            "  li $v0, 4\n"
            "  la $a0, _ret\n"
            "  syscall\n"
            "  move $v0, $0\n"
            "  jr $ra\n");
}

void RETURNGenerator(InterCode ic,FILE* stream){
	Operand ret=ic->RETURN.ret;
	if (ret->kind==ICONSTANT){
		fprintf(stream,"  li $v0, %d\n",ret->ICons);
	}
	else{
		int retAddr=GetVTAddrRelFP(ret);
		fprintf(stream,"  lw $v0, %d($fp)\n",retAddr);
	}
	fprintf(stream, "  lw $fp, -8($fp)\n");
    fprintf(stream, "  addi $sp, $sp, %d\n", (VSize + TSize)*4);
    fprintf(stream, "  addi $sp, $sp, 8\n");
    fprintf(stream, "  jr $ra\n");
}

void MachineCodeGenerator(char* filename){
	FILE* fp;

    if(filename == NULL)
    {
        fp = stdout;
    }
    else
    {
        fp = fopen(filename, "w");
    }

   // printf("Here\n");
    init_SPIM();
    MachineCodePreparation(fp);
    //printf("Here\n");
    ICEntry ICE ;
    ICE=GlobalICList;
    while (ICE!=NULL){
        InterCode IC = ICE->cur;
        switch (IC->kind)
        {
            case IFUN: ICE = FUNGenerator(ICE, fp); break;
            case IASSIGN: ASSIGNGenerator(IC, fp); break;
            case IREAD:READGenerator(IC, fp);break;
            case IWRITE:WRITEGenerator(IC,fp);break;
            case ILABELDEC:LABELDECGenerator(IC, fp);break;
            case IIFGOTO:IFGOTOGenerator(IC, fp);break;
            case IGOTO:GOTOGenerator(IC, fp);break;
            case IPLUS:
            case IMINUS:
            case ISTAR:
            case IDIV:BINOPGenerator(IC, fp);break;
            case IARG:ICE = ARGGenerator(ICE, fp);break;
            case IRETURN:RETURNGenerator(IC, fp);break;
            case IDEC:break;
        }
        ICE=ICE->next;
        //fprintf(fp,"------------------------------------\n");
    }
    fprintf(fp, "\n");

    //free(VOffset);
    //free(TOffset);
}

ICEntry PARAMGenerator(ICEntry ICE){
	ICEntry temp_ice;
	temp_ice=ICE;
	argnum=0;
	ICEntry prev_ice=NULL;
	while (temp_ice->cur->kind==IPARAM){
		//printf("PARAMGenerator a round\n");
		*(temp_arg_list+argnum)=temp_ice->cur->PARAM.parameter;
		argnum++;
		prev_ice=temp_ice;
		temp_ice=temp_ice->next;
	}
	return prev_ice;
}



void READGenerator(InterCode IC, FILE* stream)
{

    fprintf(stream,
            "  jal read\n"
            "  lw $ra, -4($fp)\n"
            "  move $t0, $v0\n");

    Operand input = IC->READ.input;
    int inputAddr = GetVTAddrRelFP(input);
    fprintf(stream, "  sw $t0, %d($fp)\n", inputAddr);
}

void WRITEGenerator(InterCode IC, FILE* stream){

    Operand ouput = IC->WRITE.output;
    int outputAddr = GetVTAddrRelFP(ouput);
    fprintf(stream, "  lw $t0, %d($fp)\n", outputAddr);
    fprintf(stream,
            "  move $a0, $t0\n"
            "  jal write\n"
            "  lw $ra, -4($fp)\n");
}

ICEntry FUNGenerator(ICEntry ICE,FILE* stream){

	ICEntry ret_ice;
	fprintf(stream,"\n%s:\n",ICE->cur->FUN.funcname);
	if (strcmp(ICE->cur->FUN.funcname,"main")==0){
		fprintf(stream,"  subu $sp, $sp, 8\n");
		fprintf(stream,"  sw $ra, 4($sp)\n");
		fprintf(stream,"  sw $fp, 0($sp)\n");
		fprintf(stream,"  addi $fp, $sp, 8\n");
        fprintf(stream, "  subu $sp, $sp, %d\n", (VSize + TSize)*4);

		ret_ice=ICE;
	}
	else{
		fprintf(stream,"  subu $sp, $sp, 8\n");
		fprintf(stream,"  sw $ra, 4($sp)\n");
		fprintf(stream,"  sw $fp, 0($sp)\n");
		fprintf(stream,"  addi $fp, $sp, 8\n");
		fprintf(stream, "  subu $sp, $sp, %d\n", (VSize + TSize)*4);


		if (ICE->next->cur->kind==IPARAM){
			ret_ice=PARAMGenerator(ICE->next);
		}
		//printf("FUNGenerator here\n");
		int i;
		for (i=0;i<argnum;i++){
			Operand param=temp_arg_list[i];
			int paramaddr=GetVTAddrRelFP(param);
			fprintf(stream,"  lw $t0, %d($fp)\n",i*4);
			fprintf(stream,"  sw $t0, %d($fp)\n",paramaddr);
		}
		argnum=0;
	
	}
	return ret_ice;
}

ICEntry ARGGenerator(ICEntry ICE,FILE* stream){
	ICEntry temp_ice=ICE;
	int num=0;
	while (temp_ice->cur->kind==IARG){
		num++;
		temp_ice=temp_ice->next;
	}
	fprintf(stream,"  subu $sp, $sp, %d\n",4*num);
	temp_ice=ICE;
	int i=0;
	while (temp_ice->cur->kind==IARG){
		Operand op=temp_ice->cur->ARG.argument;
		int argAddr=GetVTAddrRelFP(op);
		fprintf(stream,"  lw $t0, %d($fp)\n",argAddr);
		fprintf(stream,"  sw $t0, %d($sp)\n",4*(num-i-1));
		temp_ice=temp_ice->next;
		i++;
	}

	CALLGenerator(temp_ice->cur,stream);
	fprintf(stream,"  lw $ra, -4($fp)\n");
	fprintf(stream,"  addi $sp, $sp, %d\n",4*num);
	return temp_ice;
}