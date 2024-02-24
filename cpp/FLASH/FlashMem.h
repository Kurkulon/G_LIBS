#ifndef FLASHMEM_H__24_02_2024__16_34
#define FLASHMEM_H__24_02_2024__16_34

#pragma once

#include "types.h"
#include "MEM\mem.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class FlashMem
{
public:

	struct FLWB
	{
		u32		adr;
		u32 	dataLen;
		u32 	dataOffset;
		byte	data[0];
	};


	const u32		pageSize;
	const u32		flashStartAdr;

	virtual u32		GetSectorAdrLen(u32 sadr, u32 *radr)	= 0;
	virtual void	Update()								= 0;
	virtual u32		Read(u32 addr, byte *data, u32 size)	= 0;
	virtual void	InitFlashWrite()						= 0;
	virtual bool	Busy()									= 0;
	virtual bool	RequestWrite(Ptr<MB> &b)				= 0;

	FlashMem(u32 psz, u32 stAdr) : pageSize(psz), flashStartAdr(stAdr) {}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // FLASHMEM_H__24_02_2024__16_34
