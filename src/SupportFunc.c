//
// Created by tesla on 11/01/19.
//

#include "../inc/SupportFunc.h"

void error(int8_t condition, char* str){
    if(condition){
        printf("%s",str);
        exit(1);
    }
}

// funzioni di debug

void printComp(char* name, Comp C, char* dopo){
    printf("%s=%.2f i%.2f%s",name,C.r,C.i,dopo);
}

void printCompVect(int16_t begin, int16_t end, Comp* x){
    int i;
    for(i=begin;i<end+1;i++){
       DEBUG_PRINTF("----%f i*%f\n",x[i].r, x[i].i);
    }
}
float power(Comp value){
    return (value.r*value.r);
}
/*void fprintfComp(Comp* timeValue, char * filename){
    FILE *fp=fopen(filename,"a+");
    for (int i = 0; i < NSAMPLE; ++i)  {
        fprintf(fp,"%d %f %f %f\n",i,timeValue[i].r, timeValue[i].i, timeValue[i].r*timeValue[i].r+timeValue[i].i*timeValue[i].i); //,BPM_val,NSAMPLE);
    }
    fprintf(fp,"#\n");
    fclose(fp);
}*/

void fprintfComp(Comp* timeValue, int length, char* dir, char * filename, char mode){
    /*
     * This function print Comp vector into filename passed as argument.
     * Filename is open in append modality and is created if it doesn't exist
     * -mode is used to decide as open file;
     *      a -> append
     *      w -> write
     */
    FILE *fp;
    char * file[200];
    strcpy(file, dir);
    strcat(file, filename);
    char flag_for_divisor=1;
    switch (mode) {
        case 'w':
            fp = fopen(file, "w+");
            flag_for_divisor=0;
            break;
        case 'a':
            fp = fopen(file, "a+");
            break;
        default:
            error(1,"in fprintfComp, argomento mode errato");
    }
    for (int i = 0; i < length; ++i)  {
        fprintf(fp,"%d %f %f %f\n",i,timeValue[i].r, timeValue[i].i, timeValue[i].r*timeValue[i].r+timeValue[i].i*timeValue[i].i); //,BPM_val,NSAMPLE);
    }
    if(flag_for_divisor)
        fprintf(fp,"#\n");
    fclose(fp);
}