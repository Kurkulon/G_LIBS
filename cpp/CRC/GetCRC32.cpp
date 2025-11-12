#include "types.h"
#include "CRC32.h"

#ifdef __CC_ARM
#pragma O3
#pragma Otime
#elif defined(_ADI_COMPILER)
#pragma optimize_for_speed
#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef __ADSPBF6xx__
#pragma section ("shared_code")
#endif

u32 GetCRC32(const void *data, u16 len)
{
	union
	{
		u32 r;
		byte b[4];
	};

	byte *s = (byte*)data;

	r = 0xFFFFFFFF;

	for ( ; len > 0; len--)
	{
		byte t = b[0];
		r >>= 8;
		r ^= tableCRC32[t ^ *(s++)];
	};

	return r^0xFFFFFFFF;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef __ADSPBF6xx__
#pragma section ("shared_code")
#endif

u32 GetCRC32(const void *data, u16 len, u32 init, u32 xorOut)
{
	union {	u32 r;	byte b[4];	};

	byte *s = (byte*)data;

	r = init;

	for ( ; len > 0; len--)
	{
		byte t = b[0];
		r >>= 8;
		r ^= tableCRC32[t ^ *(s++)];
	};

	return r ^ xorOut;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
