//
// Created by tesla on 12/01/19.
//

//  Low Pass FIR Filter

#include "../inc/TimeConditioning.h"
#include "../inc/CommonInclude.h"

///////////////  DEFINE    //////////////////////////////
    // this variable set if windowing function used saved value for window
    // or every time calculate value by window function
    #define MEMORYON 0
    // Here we define constant which decide if window is Hamming Hanning or flat Top
    // the code is structured in such a way that could be added easly new window function
    // whoever only desired window constant could be defined, indeed in code is verified
    // both definitions of windows constants and its assertion (specific constant must be 1)
    // if multiple constant are defined at 1 the priority is assigned by following order
    #define HANNING 1
    #define HAMMING 0
    #define FLATTOP 0

    // function for multiplication
    #define mul16(x,y) (int32_t)((long)(x)*(long)(y))

///////////////  CONSTANT  //////////////////////////////
    // CONSTANT FOR lowPassFIRFilter and averageDCEstimator function
        //static const uint16_t FIRCoeffs[32] = {140, 150, 160, 170, 180, 190, 200, 200, 200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 300, 310, 320, 421, 579, 927, 900, 958, 990, 916, 990, 1000, 1012, 1700};
        //static const uint16_t FIRCoeffs[13] = {172, 321, 579, 927, 2360, 2858, 3390, 3916, 4391, 4768, 4012, 4096, 5090};
        static const uint16_t FIRCoeffs[12] = {172, 321, 579, 927, 1360, 1858, 2390, 2916, 3391, 3768, 4012, 4096};
        static int16_t cbuf[32];
        static uint8_t offset = 0;
        static int32_t average_buf=0;



///////////////  FUNCTION  //////////////////////////////

////// FILTER

    int16_t lowPassFIRFilter(int16_t din){
        // this function is taken from https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library
        // and has been modified using preprocessor directive
        cbuf[offset] = din;
        uint8_t i;
        // 31 0x3F
        // 11 0x1F
        int32_t z = mul16(FIRCoeffs[11], cbuf[(offset - 11) & 0x1F]);
        // 31,64,0x3F
        // 11,22,0x3F
        for (i = 0 ; i < 11 ; i++){
            z += mul16(FIRCoeffs[i], cbuf[(offset - i) & 0x1F] + cbuf[(offset - 22 + i) & 0x1F]);
        }

        offset++;
        //64
        offset %= 32; //Wrap condition

        return (int16_t)(z >> 15);
    }

    //  Average DC Estimator
    int16_t averageDCEstimator(int32_t x)
    {
        // this function is taken from https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library
        average_buf += ((((long) x << 15) - average_buf) >> 4);
        return (average_buf >> 15);
    }



////// WINDOWING

    float WindowFunc(int16_t index){
        /*
         * this function take a single complex element and change its value multiplying its value for
         * a windowing function, is needed position of value in fft vector for evaluate correct multiplying coefficient
         */
        #ifdef HANNING
            #if HANNING
                // this formula would be  0.5-0.5*cos(2*PI*t/Tsample) but t/Tsample=index/SAMPLEUSED
                return ((float)(0.5-0.5*cos((2*PI*((float)index)/(S_AN+S_HST)))));
            #endif
        #elif HAMMING
            #if HANNING
                // this formula would be  0.54-0.46*cos(2*PI*t/Tsample) but t/Tsample=index/(S_AN+S_HST)
                x->r = x->r*(0.54-0.46*cos(2*PI*index/(S_AN+S_HST)));
                return ;
            #endif
        #elif  FLATTOP
            #if HANNING
                // this formula would be ???
            #endif
        #endif
    }


