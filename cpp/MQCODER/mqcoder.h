#ifndef MQCODER_H__06_10_2025__15_33
#define MQCODER_H__06_10_2025__15_33

#pragma once

#include "types.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

extern u32 MQcompress(byte* src, u32 srclen, byte* dst);
extern u32 MQcompressFast(byte* src, u32 srclen, byte* dst);
extern u32 MQdecompress(byte* src, u32 srclen, byte* dst, u32 dstlen);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // MQCODER_H__06_10_2025__15_33

