//
// Created by tesla on 10/01/19.
//

#include <fcntl.h>
#include "../inc/HeartBeat.h"
#include "../inc/SupportFunc.h"
#include "../inc/CommonInclude.h"

////////////// DEFINE ////////////////////////////////
#define F_CONTINUE 1
#define F_X_SAMPLE_FIND 2
#define F_FFT_ENDED 3
#if DEBUG
    FILE * fp_data;
    FILE * fp_bpm;
    FILE * fp_power;
    FILE * fp_op;
    FILE * fp_data_fft;
#endif
////////////// FUNCTION ////////////////////////////////

uint8_t HeartBeat(int32_t value, uint16_t* BPM, uint8_t reset){
    static int16_t x[S_TOT]={0};
    static uint16_t sample_cnt = 0;
    static uint8_t start = 0;
    static uint16_t offset = 0;
    static uint16_t K=S_START_BPM;
    static uint16_t Max_K=0;
    static uint8_t BPM_flag = 1;
    static Comp X=(Comp){.r=0, .i=0};
    static float Max_Power=0;

    #if DEBUG
        char str[200];
        if(sample_cnt==0 && offset==0) {
            strcpy(str,RESULTDIR), strcat(str,"data.txt");
            fp_data = fopen(str, "w+");

            strcpy(str,RESULTDIR), strcat(str,"bpm.txt");
            fp_bpm = fopen(str, "w+");

            strcpy(str,RESULTDIR), strcat(str,"power.txt");
            fp_power = fopen(str, "w+");

            strcpy(str,RESULTDIR), strcat(str,"operation.txt");
            fp_op = fopen(str, "w+");

            strcpy(str,RESULTDIR), strcat(str,"data_fft.txt");
            fp_data_fft = fopen(str, "w+");
        }
    #endif

    int i;
    if (reset){
        for(i=0;i<S_TOT; i++)
            x[i] = 0;
        sample_cnt = 0;
        start = 0;
        offset = 0;
        K=S_START_BPM;
        Max_K=0;
        BPM_flag = 1;
        X.r=0, X.i=0;
        Max_Power=0;
    }

    // tolgo la media e filtro salvando nel vettore
    #if DEBUG
        int16_t  value_2 =value-averageDCEstimator(value) ;
        int16_t low=lowPassFIRFilter(value_2);
        x[sample_cnt] = low;
        DEBUG_FPRINTF(fp_data,"%d %d %d %d\n",sample_cnt, value_2, low, x[sample_cnt]);
    #else
        x[sample_cnt]=lowPassFIRFilter(value-averageDCEstimator(value));
    #endif
    if (start){
        BPM_flag = F_CONTINUE;
        // esegui N_OP operazioni
        for (i = 0; i < N_OP && BPM_flag != F_FFT_ENDED; i++) {
            DEBUG_FPRINTF(fp_op,"i=%d sample_cnt=%d ",i,sample_cnt);
            BPM_flag = Operation(x, offset, &K, &X);
            // se ha trovato una frequenza
            if ( BPM_flag==F_X_SAMPLE_FIND || BPM_flag==F_FFT_ENDED ){
                // se il modulo della potenza è maggiore della potenza massima
                DEBUG_FPRINTF(fp_power,"sample_cnt=%d i=%d %f BPM_FLAG=%d K=%d\n",sample_cnt, i, ( X.r*X.r + X.i*X.i),BPM_flag,K);
                if(( X.r*X.r + X.i*X.i)>Max_Power){
                    Max_K=K;
                    Max_Power = ( X.r*X.r + X.i*X.i);
                }
                X.i =0;
                X.r =0;
            }
            if(BPM_flag==F_FFT_ENDED){
                start=0;
                DEBUG_FPRINTF(fp_power,"%c\n",'#');
            }
        }
    }
    sample_cnt++;
    sample_cnt = (uint16_t)(sample_cnt % (S_TOT));
    if ( sample_cnt == S_HST + S_AN - 1 && offset == 0){
        start = 1;
    }
    // se ho salvato S_AN valori
    if( sample_cnt == offset ){
        if (BPM_flag!=F_FFT_ENDED){
            DEBUG_ERR3("FFT non finita quando dovrebbe");
            exit(1);
        }
        start = 1;
        offset += S_AN;
        offset %= (S_TOT);
        // trovo il BPM
        *BPM = (uint16_t)(( Max_K*FS*60 )/( S_FFT ));
        Max_Power=0;
        K=S_START_BPM;
        return 1;
    }
    return 0;
}

uint8_t Operation(int16_t *x, uint16_t offset, uint16_t *K, Comp *X){
    static uint16_t CX= 0;

    #if DEBUG
        if(*K==S_START_BPM)
            DEBUG_FPRINTF(fp_data_fft,"%d %f\n",CX,x[(offset+CX)%S_TOT]*WindowFunc(CX) );
        X->r += (float)(x[(offset+CX)%S_TOT] * cos(( 2*PI*(*K)*(CX))/S_FFT ) * WindowFunc(CX));
        X->i += (float)(x[(offset+CX)%S_TOT] * (-sin(( 2*PI*(*K)*(CX) )/S_FFT )) * WindowFunc(CX));
    #else
        X->r += (float)(x[(offset+CX)%S_TOT] * cos(( 2*PI*(*K)*((offset+CX)%S_TOT) )/S_FFT ) * WindowFunc(CX));
        X->i += (float)(x[(offset+CX)%S_TOT] * (-sin(( 2*PI*(*K)*((offset+CX)%S_TOT) )/S_FFT )) * WindowFunc(CX));
    #endif

    DEBUG_FPRINTF(fp_op,"CX=%d K=%d off=%d index=%d",CX,*K,offset,(offset+CX)%S_TOT);
    CX++;
    if ( CX < S_AN + S_HST ){
        DEBUG_FPRINTF(fp_op,"%s continue\n"," ");
        return F_CONTINUE;
    }
    // se arrivo qua vuol dire che  CX == S_AN + S_HST
    // azzero il contatore
    CX=0;
    // incremento K per andare a trovare un nuovo valore all'iterazione successiva
    *K = (uint16_t)((*K)+1);
    // se ho trovato tutti i valori
    if ( *K == S_START_BPM + S_FIND ){
        DEBUG_FPRINTF(fp_op,"%s","  fft finita\n");
        DEBUG_FPRINTF(fp_data_fft,"%c\n",'#');
        return F_FFT_ENDED;
    }
    DEBUG_FPRINTF(fp_op,"%s","  sample find\n");
    return F_X_SAMPLE_FIND;
}

