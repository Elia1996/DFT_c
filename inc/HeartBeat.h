//
// Created by tesla on 10/01/19.
//

#ifndef DFT_HEARTBEAT_H
#define DFT_HEARTBEAT_H

#include "CommonInclude.h"
#include "TimeConditioning.h"

#define MAX_BPM 200
#define MIN_BPM 40
// per comodità definisco anche la differenza
#define DELTA_BPM 160

uint8_t HeartBeat(int32_t value, uint16_t* BPM, uint8_t reset);
uint8_t Operation(int16_t *x, uint16_t offset, uint16_t *K, Comp *X);

#endif //DFT_HEARTBEAT_H
