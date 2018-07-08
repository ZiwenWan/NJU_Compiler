#ifndef __SPIM_H__
#define __SPIM_H__
#include "../intercode/intercode.h"
#include "../intercode/translate.h"
#include "stdio.h"




ICEntry ARGGenerator(ICEntry ICE,FILE* stream);
ICEntry FUNGenerator(ICEntry ICE,FILE* stream);
void WRITEGenerator(InterCode IC,FILE* stream);
void READGenerator(InterCode IC,FILE* stream);
ICEntry PARAMGenerator(ICEntry ICE);
void MachineCodeGenerator(char* filename);
void RETURNGenerator(InterCode ic,FILE* stream);
void MachineCodePreparation(FILE* stream);
void CALLGenerator(InterCode ic,FILE* stream);
void LABELDECGenerator(InterCode ic,FILE* stream);
void BINOPGenerator(InterCode ic,FILE* stream);
void GOTOGenerator(InterCode ic,FILE* stream);
void IFGOTOGenerator(InterCode ic,FILE* stream);
void ASSIGNGenerator(InterCode ic,FILE* stream);

void init_SPIM();


#endif

