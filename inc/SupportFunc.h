//
// Created by tesla on 11/01/19.
//

#ifndef DFT_SUPPORTFUNC_H
#define DFT_SUPPORTFUNC_H


#include "CommonInclude.h"


///// FUNCTION
void error(int8_t condition, char* str);
void printComp(char* name, Comp C, char* dopo);
void printCompVect(int16_t begin, int16_t end, Comp* x);
float power(Comp value);
void fprintfComp(Comp* timeValue, int length,char* dir,  char * filename, char mode);

#endif //DFT_SUPPORTFUNC_H
