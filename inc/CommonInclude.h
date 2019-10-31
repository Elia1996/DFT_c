//
// Created by tesla on 10/01/19.
//

#ifndef DFT_COMMONINCLUDE_H
#define DFT_COMMONINCLUDE_H


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


///// DEFINE

struct comp{
    float r; // parte reale
    float i; // parte immaginaria
};

typedef struct comp Comp;


#define MAINDIR "/media/tesla/Storage/Linux/Scrivania/Progetti_work/Healt-Shirt/BPM-Detect/DFT"
#define RESULTDIR "/media/tesla/Storage/Linux/Scrivania/Progetti_work/Healt-Shirt/BPM-Detect/FFT_realtime/Results/"

// PARAMETRI MODIFICABILI
/*
 * Ci sono dei parametri di progetto che non vengono riportati perchè inutili al programma
 * ma fondamentali per ricavare i valori da inserire nelle define successive, questi sono:
 *      T_AN -> è il tempo fra un'BPM e l'altro, ogni T_AN secondi si ha un nuovo BPM in
 *              uscita dalla funzione.
 *
 *      T_HST -> è il tempo storico, ogni volta che viene calcolata un BPM questo è calcolato andando
 *               a fare l'FFT su un periodo temporale. questo periodo è T_AN+T_HST, i due tempi vengono divisi perchè
 *               una volta che si sono immagazzinati T_AN+T_HST valori si inizia a fare il calcolo del BPM, ma solo
 *               dopo T_AN secondi (o meglio dopo che la funzione è stata chiamata S_AN volte) il BPM è pronto.
 *               Quindi è da sapere che il BPM dato non è quello sugli ultimi S_AN campioni dati ma sugli S_AN+S_HST dati
 *               precedentemente.
 *
 *      DMAX -> è la dimensione di memoria massima che può essere utilizzata dal programma, è un limite di progetto.
 *
 *      T_FFT -> è il tempo totale impiegato per fare un'FFT e quindi per calcolare un BPM, la formula per trovarlo è:
 *                  T_FFT = ( n_op*(S_AN+S_HST)*S_FIND )/( FCPU)
 *                  T_FFT = ( n_op*FS*(T_AN+T_HST)*DELTA_BPM )/( FCPU * Nb )
 *                  T_FFT =  ( N_OP*S_AN )*( n_op/FCPU )
 *                                ^              ^
 *                     (operazioni totali)    (tempo a operazinoe)
 *
 *      Nb -> è la precisione in BPM, se Nb è 1 potranno essere trovati i battiti 40,41,42,43... mentre se è per esempio
 *              4 si potranno trovare solo 40,44,48,52... .
 *              Nb influenza quadraticamente il tempo totale per fare un'FFT
 *              Nb = DELTA_BPM/S_FIND
 *      FCPU -> è la frequenza della CPU
 *      n_op -> nè il numero di colpi di clock ad ogni operazione dell'FFT, non è uguale a N_OP (che è un'altra cosa)
 *
 *
 */
// FS
#define FS 50

/*                                                         esempio
 *  1) scelgo :                                        fs     200Hz    200Hz   50Hz  50Hz
 *              S_AN = T_AN*FS                                200      200     50    50
 *              S_HST = T_HST*FS                              800      800    150   200
 *              Nb                                              4        1      1     1
 *  2) trovo:
 *           1) S_TOT = S_AN*2 + T_HST                       1200     1200    250   300
 *           2) S_FFT = 60*FS/Nb                             3000    12000   3000  3000
 *           3) S_FIND = DELTA_BPM/Nb                          40      160    160   160
 *           4) S_START_BPM = MIN_BPM/Nb                       10       40     40    40
 *           5) N_OP = ( (S_AN+S_HST) * S_FIND )/S_AN         200      800    640   800
 *  3) posso trovare i tempi
 *           1) T_AN                                            1s       1s    1s    1s
 *           2) T_HST                                           4s       4s    3s    4s
 *
 *  4) posso ricavare il tempo di esecuzione
 *     ad ogni chiamata come (n_op è il numero di clk
 *     per eseguire una operazione, 6 va bene)
 *           T_FFT=(n_op*(S_AN+S_HST)*S_FIND)/(FCPU*S_AN)    300us    1.2ms   960us  1.2ms
 *
 *  5) posso ricavare lo spazio totale utilizzato come
 *     (B sta per Byte)
 *       SPACE=28B + S_TOT*2B                                2428B    2428B  528B  628B
 *                                                            97C      97C   210   274
 *
 */


// S_AN = T_AN*FS
#define S_AN 50

// S_HST = T_HST*FS
#define S_HST 200

// S_TOT = S_HST+2*San numero di campioni da salvare nel vettore
#define S_TOT 300

// S_FFT = 60*FS/Nb
#define S_FFT 3000

// S_FIND = DELTA_BPM/Nb
#define S_FIND 160

// S_START_BPM = MIN_BPM/Nb  campione in frequenza da cuii partire a calcolare l'FFT
#define S_START_BPM 40

// N_OP = ( (S_AN+S_HST) * S_FIND )/S_AN = ( (T_AN+T_HST)*DELTA_BPM )/( T_AN*N_b )
// è il numero di operazioni che vengono fatte quando viene chiamata la funzione al fine di eseguire un'fft
// completa in S_AN chiamate, infatti N_OP*S_AN è il numero di operazioni totali
#define N_OP 800




#define PI 3.14159265
// this definition needs to include or not include some part of code that
// are useful in debug but are wasteful when the code in load on microcontroller
// son BEBUG must be 0 when code is load on the microcontroller
#define DEBUG 0

#if DEBUG
    #define DEBUG_ERR1(_fmt, ...)  fprintf(stderr, "[file %s, line %d]: " _fmt, __FILE__, __LINE__, __VA_ARGS__);
    #define DEBUG_ERR2()  fprintf(stderr, "[file %s, line %d]: ", __FILE__, __LINE__);
    #define DEBUG_ERR3(_fmt)  fprintf(stderr, "[file %s, line %d]: " _fmt, __FILE__, __LINE__);
    #define DEBUG_FPRINTF(fp,_fmt, ...)  fprintf(fp, _fmt, __VA_ARGS__);
    #define DEBUG_PRINTF(_fmt, ...)  printf( _fmt, __VA_ARGS__);
#else

    #define DEBUG_ERR1(_fmt, ...)  "";
    #define DEBUG_ERR2()  "";
    #define DEBUG_ERR3(_fmt) "";
    #define DEBUG_FPRINTF(fp,_fmt, ...)  ""
    #define DEBUG_PRINTF(_fmt, ...) ""
#endif


#endif //DFT_COMMONINCLUDE_H
