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
	typedef volatile u32			AD_RW32, AD_WO32;	// Hardware register definition
	typedef volatile const u32	AD_RO32;			// Hardware register definition
	typedef volatile void*			AD_PTR;				// Hardware register definition

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_SPI
	{
		AD_RW32 CTL;							//	0x1000	/* SPI Control Register */
		AD_RW32 FLG;							//	0x1001	/* SPI Flag register */
		AD_RW32 STAT;							//	0x1002	/* SPI Status register */
		AD_RW32 TX;								//	0x1003	/* SPI transmit data register */
		AD_RW32 RX;								//	0x1004	/* SPI receive data register */
		AD_RW32 BAUD;							//	0x1005	/* SPI baud setup register */
		AD_RW32 RX_SHADOW;						//	0x1006	/* SPI receive data shadow register */
								AD_RO32			z__RESERVED0[0x80-0x7];
		AD_RW32 II;								//	0x1080	/* Internal memory DMA address */
		AD_RW32 IM;								//	0x1081	/* Internal memory DMA access modifier */
		AD_RW32 C;								//	0x1082	/* Contains number of DMA transfers remaining */
		AD_RW32 CP;								//	0x1083	/* Points to next DMA parameters */
		AD_RW32 DMAC;							//	0x1084	/* SPI DMA control register */
	};

	typedef S_SPI S_SPIB;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_UART
	{
		union
		{
			AD_RW32 THR;				//	0x3c00	/* Transmit Holding Register */
			AD_RW32 RBR;				//	0x3c00	/* Receive Buffer Register */
			AD_RW32 DLL;				//	0x3c00	/* Divisor Latch Low Byte */
		};

		union
		{
			AD_RW32 IER;				//	0x3c01	/* Interrupt Enable Register */
			AD_RW32 DLH;				//	0x3c01	/* Divisor Latch High Byte */
		};

		AD_RW32 IIR;					//	0x3c02	/* Interrupt Identification Register */
		AD_RW32 LCR;					//	0x3c03	/* Line Control Register */
		AD_RW32 MODE;					//	0x3c04	/* Mode Register */
		AD_RW32 LSR;					//	0x3c05	/* Line Status Register */
		AD_RW32 SCR;					//	0x3c07	/* Scratch Register */
		AD_RW32 RBRSH;					//	0x3c08	/* Read Buffer Shadow Register */
		AD_RW32 IIRSH;					//	0x3c09	/* Interrupt Identification Shadow Register */
		AD_RW32 LSRSH;					//	0x3c0a	/* Line Status Shadow Register */
								AD_RO32			z__RESERVED0[0xE00-0xC0B];
		AD_RW32 IIRX;					//	0x3e00	/* Internal Memory address for DMA access with UART Receiver */
		AD_RW32 IMRX;					//	0x3e01	/* Internal Memory modifier for DMA access with UART Receiver */
		AD_RW32 CRX;					//	0x3e02	/* Word Count for DMA access with UART Receiver */
		AD_RW32 CPRX;					//	0x3e03	/* Chain Point for DMA access with UART Receiver */
		AD_RW32 RXCTL;					//	0x3e04	/* UART Receiver control register */
		AD_RW32 RXSTAT;					//	0x3e05	/* UART Receiver status register */
								AD_RO32			z__RESERVED1[0xF00-0xE06];
		AD_RW32 IITX;					//	0x3f00	/* Internal Memory address for DMA access with UART Transmitter */
		AD_RW32 IMTX;					//	0x3f01	/* Internal Memory modifier for DMA access with UART Transmitter */
		AD_RW32 CTX;					//	0x3f02	/* Word Count for DMA access with UART Transmitter */
		AD_RW32 CPTX;					//	0x3f03	/* Chain Point for DMA access with UART Transmitter */
		AD_RW32 TXCTL;					//	0x3f04	/* UART Transmitter control register */
		AD_RW32 TXSTAT;					//	0x3f05	/* UART Transmitter status register */
	};

	typedef S_SPI S_SPIB;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_PORT
	{
		__forceinline void 	SET(u32 m) 			{ sysreg_bit_set(sysreg_FLAGS, 0); }
		__forceinline void 	CLR(u32 m) 			{ sysreg_bit_clr(sysreg_FLAGS, 0);	}
		__forceinline void 	NOT(u32 m) 			{ sysreg_bit_tgl(sysreg_FLAGS, 0);	}
		__forceinline void 	WBIT(u32 m, bool c)	{ if (c) SET(m); else CLR(m);	}
		__forceinline void 	BSET(byte b) 		{ SET(1UL<< b);					}
		__forceinline void 	BCLR(byte b) 		{ CLR(1UL<< b);					}
		__forceinline void 	BTGL(byte b) 		{ NOT(1UL<< b);					}

		__forceinline bool 	TBSET(byte b) 		{ return sysreg_read(sysreg_FLAGS) & (1<<b); }
		__forceinline bool 	TBCLR(byte b) 		{ return (sysreg_read(sysreg_FLAGS) & (1<<b)) == 0; }

		__forceinline void 	DirSet(u32 m) 		{ sysreg_bit_set(sysreg_FLAGS, 0); }
		__forceinline void 	DirClr(u32 m) 		{ sysreg_bit_clr(sysreg_FLAGS, 0); }

		__forceinline void		SetFER(u32 m)		{  }
		__forceinline void		ClrFER(u32 m)		{  }

		__forceinline void		SetMUX(byte pin, byte v) { }

		__forceinline void		ClearTriggerIRQ(u32 m)	{ /*((S_PINT*)(((u32)this & ~0x1FF)|(((u32)this & 0x1FF)<<1)|0x1000))->LATCH = m;*/ }
	};

	typedef S_PORT S_PIO;
	
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

	MK_PTR(SPI,		SPICTL		);
	MK_PTR(SPIB,	SPICTLB		);
	MK_PTR(PORT,	0			);
	MK_PTR(PIO,		0			);


	inline void ResetWDT()		{ *pWDTUNLOCK = 0xAD21AD21;  *pWDTCURCNT = 1; *pWDTUNLOCK = 0;	}
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
