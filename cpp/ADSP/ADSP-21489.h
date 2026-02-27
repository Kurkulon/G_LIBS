#ifndef ADSP21489_H__27_02_2026__12_11
#define ADSP21489_H__27_02_2026__12_11

#pragma once

#ifndef CORETYPE_21489
#error  Must #include "core.h"
#endif 

#ifndef __ADSP2148x__
#error  Must #include "core.h"
#endif

#include "types.h"

#include <sys\platform.h> 
#include <interrupt.h> 


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define IVG_EMULATION		0
#define IVG_RESET			1
#define IVG_NMI				2
#define IVG_EXEPTIONS		3
#define IVG_HW_ERROR		5
#define IVG_CORETIMER		6

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

extern void Core0_InitIVG(u32 IVG, void (*EVT)());
extern void Core1_InitIVG(u32 IVG, void (*EVT)());

#define InitIVG Core0_InitIVG

extern void InitSEC(u32 PID, void (*EVT)(), byte coren, byte prio = 0);

extern u32 Get_CCLK();
extern u32 Get_SCLK();
extern u32 Get_SCLK0();
extern u32 Get_SCLK1();
extern u32 Get_CCLK_MHz();	
extern u32 Get_SCLK_MHz();	
extern u32 Get_SCLK0_MHz();	
extern u32 Get_SCLK1_MHz();	

//extern void* Alloc_L1_DataA(u32 size);
//extern void* Alloc_L1_DataB(u32 size);
//extern void* Alloc_L2_NoCache(u32 size);
//extern void* Alloc_UnCached(u32 size);
//extern void* Alloc_L2_CacheWT(u32 size);
//
//extern bool IsCodeCacheEnabled();
//extern bool IsDataCacheEnabled();
//extern bool IsDataCacheA_Enabled();
//extern bool IsDataCacheB_Enabled();

//#pragma always_inline
//inline void SIC_EnableIRQ(byte pid) { *pSIC_IMASK |= 1UL<<pid; }
//inline void SIC_DisableIRQ(byte pid) { *pSIC_IMASK &= ~(1UL<<pid); }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//#pragma always_inline
//inline void ResetWDT()		{ *pWDOG_STAT = 0;		}
//#pragma always_inline
//inline void DisableWDT()	{ *pWDOG_CTL = WDDIS;	}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline u64 GetCycles64()
{
	return __builtin_emuclk();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma always_inline
inline u32 GetCycles32()
{
	u32 res;

	__asm volatile ("%0 = emuclk;  \n"	: "=d" (res)	:	:	); 

	return res; 
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef WIN32
#define MK_PTR(n,p)  T_HW::S_##n * const n = ((T_HW::S_##n*)(p))
#else
extern byte core_sys_array[0x100000]; 
#define MK_PTR(n,p)  T_HW::S_##n * const n = ((T_HW::S_##n*)(core_sys_array-0x40000000+p))
#endif

#define MKPID(n,i) n##_M=(1UL<<(i&31)), n##_I=i

namespace T_HW
{


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
}


namespace HW
{
	//namespace PID
	//{
	//	enum {	MKPID(SUPC, 0),		MKPID(RSTC, 1),		MKPID(RTC, 2),		MKPID(RTT, 3),		MKPID(WDT, 4),		MKPID(PMC, 5),		MKPID(EEFC0, 6),	MKPID(EEFC1, 7),
	//			MKPID(UART, 8),		MKPID(SMC, 9),		MKPID(SDRAM, 10),	MKPID(PIOA, 11),	MKPID(PIOB, 12),	MKPID(PIOC, 13),	MKPID(PIOD, 14),	MKPID(PIOE, 15),
	//			MKPID(PIOF, 16),	MKPID(USART0, 17),	MKPID(USART1, 18),	MKPID(USART2, 19),	MKPID(USART3, 20),	MKPID(HSMCI, 21),	MKPID(TWI0, 22),	MKPID(TWI1, 23), 
	//			MKPID(SPI0, 24),	MKPID(SPI1, 25), 	MKPID(SSC, 26),		MKPID(TC0, 27),		MKPID(TC1, 28),		MKPID(TC2, 29),		MKPID(TC3, 30),		MKPID(TC4, 31), 
	//			MKPID(TC5, 32),		MKPID(TC6, 33),		MKPID(TC7, 34),		MKPID(TC8, 35), 	MKPID(PWM, 36),		MKPID(ADC, 37),		MKPID(DACC, 38),	MKPID(DMAC, 39), 
	//			MKPID(UOTGHS, 40),	MKPID(TRNG, 41),	MKPID(EMAC, 42),	MKPID(CAN0, 43),	MKPID(CAN1, 44) };
	//};



	//inline void ResetWDT()		{ WDOG->Reset();	}
	//inline void DisableWDT()	{ WDOG->Disable();	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	__forceinline bool RamCheck(const void *ptr)
	{
		u32 v = (u32)ptr;

		return true; //((v&~0x107FFF) == 0x11800000 || (v&~0xFFFFF) == 0x8000000);

	};

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	inline void SystemReset()
	{
		//CM4::SCB->AIRCR = 0x05FA0000|SCB_AIRCR_SYSRESETREQ_Msk;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


} // namespace HW





#undef MK_PTR
#undef MKPID

#endif // ADSP21489_H__27_02_2026__12_11
