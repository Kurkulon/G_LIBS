#ifndef WAVEPACK_H__12_03_2024__12_16
#define WAVEPACK_H__12_03_2024__12_16

#pragma once

#include "types.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

extern void WavePack_Init();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef WAVEPACK_V2 // version 2

struct PackDCT
{
	u16		len		: 12;
	u16		scale	: 4;
	byte	data[];
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct PackInfoDCT
{
	byte 	log2n;
	byte	log2ovrlap;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

enum PackType { PACK_NO = 0, PACK_ULAW12, PACK_ULAW16, PACK_ADPCMIMA, PACK_DCT0, PACK_DCT1, PACK_DCT2, PACK_DCT3, PACK_DCT128, PACK_DCT256, PACK_DCT512, PACK_DCT1024, PACK_SIZE};

extern u16 WavePack(u16 packType, i16* src, byte* dst, u16 len, u16 maxlen, u16 *sampleLen);

#else	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct PackDCT
{
	byte	len;
	byte	scale;
	byte	data[16];
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

enum PackType { PACK_NO = 0, PACK_ULAW12, PACK_ULAW16, PACK_ADPCMIMA, PACK_DCT0, PACK_DCT1, PACK_DCT2, PACK_DCT3, PACK_SIZE};

extern u16 WavePack_uLaw_12Bit(i16* src, byte* dst, u16 len);
extern u16 WavePack_uLaw_16Bit(i16* src, byte* dst, u16 len);
extern u16 WavePack_ADPCMIMA(i16* src, byte* dst, u16 len);
extern u16 WavePack_FDCT(u16 packType, i16* src, byte* dst, u16 len);
extern u16 WavePack(u16 packType, i16* src, byte* dst, u16 len, u16 maxlen);

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



#endif //WAVEPACK_H__12_03_2024__12_16
