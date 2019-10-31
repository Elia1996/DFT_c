//
// Created by tesla on 12/01/19.
//

#ifndef DFT_FILTER_H
#define DFT_FILTER_H

#include "CommonInclude.h"


int16_t lowPassFIRFilter(int16_t din);
float WindowFunc(int16_t index);
int16_t averageDCEstimator(int32_t x);


#endif //DFT_FILTER_H
