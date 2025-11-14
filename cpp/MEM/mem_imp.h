#ifndef MEM_IMP_H__12_10_2022__17_05
#define MEM_IMP_H__12_10_2022__17_05

#include "mem.h"

#ifdef CPU_BF607

#define SHARED_CODE _Pragma("section (\"shared_code\")")
#define SHARED_DATA _Pragma("section (\"shared_data\")")

#else

#define SHARED_CODE /**/
#define SHARED_DATA /**/

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

template <int L> List< MEMB<L> > MEMB<L>::_freeList;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define MEMBS MEMB<SMALL_BUF_LEN>
#define MEMBM MEMB<MEDIUM_BUF_LEN>
#define MEMBH MEMB<HUGE_BUF_LEN>

SHARED_DATA MEMBS	small_buffer[NUM_SMALL_BUF];
SHARED_DATA MEMBM	medium_buffer[NUM_MEDIUM_BUF];
SHARED_DATA MEMBH	huge_buffer[NUM_HUGE_BUF];

#ifdef _ADI_COMPILER
#pragma instantiate MEMBS
#pragma instantiate MEMBM
#pragma instantiate MEMBH
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SHARED_CODE
MB* AllocSmallBuffer()
{
	return MEMBS::Create();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SHARED_CODE
MB* AllocMediumBuffer()
{
	return MEMBM::Create();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SHARED_CODE
MB* AllocHugeBuffer()
{
	return MEMBH::Create();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

SHARED_CODE
MB* AllocMemBuffer(u32 minLen)
{
	if (minLen > MEDIUM_BUF_LEN)
	{
		return (minLen <= HUGE_BUF_LEN) ? AllocHugeBuffer() : 0;
	}
	else if (minLen > SMALL_BUF_LEN)
	{
		MB *p = AllocMediumBuffer();

		if (p == 0) p = AllocHugeBuffer();

		return p;
	}
	else
	{
		MB *p = AllocSmallBuffer();

		if (p == 0) p = AllocMediumBuffer();

		if (p == 0) p = AllocHugeBuffer();

		return p;
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // MEM_IMP_H__12_10_2022__17_05
