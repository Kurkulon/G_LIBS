#ifndef WAVEPACK_H__12_03_2024__12_16
#define WAVEPACK_H__12_03_2024__12_16

#pragma once

#include "types.h"

enum PackType { PACK_NO = 0, PACK_ULAW12, PACK_ULAW16, PACK_ADPCMIMA, PACK_DCT0, PACK_DCT1, PACK_DCT2, PACK_DCT3, PACK_SIZE};

extern u16 WavePack_uLaw_12Bit(i16* src, byte* dst, u16 len);
extern u16 WavePack_uLaw_16Bit(i16* src, byte* dst, u16 len);
extern u16 WavePack_ADPCMIMA(i16* src, byte* dst, u16 len);
extern u16 WavePack_FDCT(u16 packType, i16* src, byte* dst, u16 len);
extern u16 WavePack(u16 packType, i16* src, byte* dst, u16 len, u16 maxlen);
extern void WavePack_Init();

#endif //WAVEPACK_H__12_03_2024__12_16
