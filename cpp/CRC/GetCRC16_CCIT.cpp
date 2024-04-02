#include "types.h"
#include "CRC16_CCIT.h"

#ifdef __CC_ARM
#pragma O3
#pragma Otime
#elif defined(_ADI_COMPILER)
#pragma optimize_for_speed
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

word GetCRC16_CCIT(const void *data, u32 len, word init, bool revres)
{
	DataCRC CRC = { (revres) ? ReverseWord(init) : init };

	const byte *s = (const byte*)data;

	for ( ; len > 0; len--)
	{
		CRC.w = tableCRC_CCIT[CRC.b[0] ^ *(s++)] ^ CRC.b[1];
	};

	return (revres) ? ReverseWord(CRC.w) : CRC.w;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

word GetCRC16_CCIT_refl(const void *data, u32 len, word init, bool revres)
{
	DataCRC CRC = { (revres) ? init : ReverseWord(init) };

	const byte *s = (const byte*)data;

	for ( ; len > 0; len--)
	{
		CRC.w = tableCRC_CCIT[CRC.b[1] ^ *(s++)] ^ (CRC.w<<8);
	};

	return (revres) ? CRC.w : ReverseWord(CRC.w);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
