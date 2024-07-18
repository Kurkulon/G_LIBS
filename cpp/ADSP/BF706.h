#ifndef BF706_H__16_07_2024__15_40
#define BF706_H__16_07_2024__15_40

#ifndef CORETYPE_BF706
//#error  Must #include "core.h"
#endif 

#include "blackfin.h"
#include <bfrom.h>
//#include <sys\exception.h>
#include <sysreg.h>

#include "types.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//extern void InitIVG(u32 IVG, u32 PID, void (*EVT)());
//
//#pragma always_inline
//inline void SIC_EnableIRQ(byte pid) { *pSIC_IMASK |= 1UL<<pid; }
//inline void SIC_DisableIRQ(byte pid) { *pSIC_IMASK &= ~(1UL<<pid); }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//#pragma always_inline
//inline void ResetWDT()		{ *pWDOG_STAT = 0;		}
//#pragma always_inline
//inline void DisableWDT()	{ *pWDOG_CTL = WDDIS;	}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma always_inline
inline i16 addsat16(i16 x, i16 y)
{
	i16 res;

	__asm volatile ("%0 = %1 + %2 (S); \n" : "=H"(res) : "H"(x),"H"(y)	:	); 

	return res; 
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma always_inline
inline i16 subsat16(i16 x, i16 y)
{
	i16 res;

	__asm volatile ("%0 = %1 - %2 (S); \n" : "=H"(res) : "H"(x),"H"(y)	:	); 

	return res; 
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma always_inline
inline i32 addsat32(i32 x, i32 y)
{
	i32 res;

	__asm volatile ("%0 = %1 + %2 (S); \n" : "=d"(res) : "d"(x),"d"(y)	:	); 

	return res; 
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma always_inline
inline i32 subsat32(i32 x, i32 y)
{
	i32 res;

	__asm volatile ("%0 = %1 - %2 (S); \n" : "=d"(res) : "d"(x),"d"(y)	:	); 

	return res; 
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline u64 GetCycles64()
{
	u64 res;

	__asm volatile ("CLI r0;       \n"  
                    "r2 = CYCLES;  \n"  
                    "r1 = CYCLES2; \n"  
                    "STI r0;       \n"  
                    "[%0]   = r2;  \n"  
                    "[%0+4] = r1;  \n"  
                    : : "p" (&res) 
                    : "r0", "r1", "r2" ); 

	return res;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma always_inline
inline u32 GetCycles32()
{
	//u32 res;

	//__asm volatile ("%0 = CYCLES;  \n"	: "=d" (res)	:	:	); 

	return sysreg_read(reg_CYCLES); 
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
	typedef volatile u16	BF_R16;		// Hardware register definition
	typedef volatile u32	BF_R32;		// Hardware register definition
	typedef volatile void	*BF_PTR;	// Hardware register definition


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//struct S_SIC	// System Interrupt Controller 	(0xFFC00100 - 0xFFC001FF)
	//{
	//	BF_R32	Imask;	
	//	BF_R32	Iar[4];
	//	BF_R32	Isr;
	//	BF_R32	Iwr;

	//	inline void EnableIRQ(byte pid)		{ Imask |= 1UL<<pid; }
	//	inline void DisableIRQ(byte pid)	{ Imask &= ~(1UL<<pid); }

	//	inline void IntAssign(byte pid, byte ivg) {	if (ivg > 6) { ivg -= 7; byte n = pid/8; byte i = (pid&7)*4; Iar[n] = (Iar[n] & ~(0xF<<i)) | (ivg<<i); EnableIRQ(pid); }; }
	//};

	//#define SIC_BASE_ADR	SIC_IMASK0

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	//struct S_EIC	// Event/Interrupt Controller Registers   (0xFFE02000 - 0xFFE02110)
	//{
	//	BF_PTR	EVT[16];	
	//	BF_R32	z__Reserved[49];
	//	BF_R32	Imask;
	//	BF_R32	Ipend;
	//	BF_R32	Ilat;
	//	BF_R32	Iprio;

	//	inline void EnableIRQ(byte ivg)		{ Imask |= 1UL<<ivg; }
	//	inline void DisableIRQ(byte ivg)	{ Imask &= ~(1UL<<ivg); }

	//	inline void InitIVG(byte ivg, void (*evt)()) { if (ivg < ArraySize(EVT)) { EVT[ivg] = (void*)evt; Imask |= 1UL<<ivg; }; }; 

	//};

	//#define EIC_BASE_ADR	EVT0

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_SPI
	{
							BF_R32                  z__RESERVED0;
		BF_R32 CTL;                           /*!< Control Register */
		BF_R32 RXCTL;                         /*!< Receive Control Register */
		BF_R32 TXCTL;                         /*!< Transmit Control Register */
		BF_R32 CLK;                           /*!< Clock Rate Register */
		BF_R32 DLY;                           /*!< Delay Register */
		BF_R32 SLVSEL;                        /*!< Slave Select Register */
		BF_R32 RWC;                           /*!< Received Word Count Register */
		BF_R32 RWCR;                          /*!< Received Word Count Reload Register */
		BF_R32 TWC;                           /*!< Transmitted Word Count Register */
		BF_R32 TWCR;                          /*!< Transmitted Word Count Reload Register */
							BF_R32                  z__RESERVED1;
		BF_R32 IMSK;                          /*!< Interrupt Mask Register */
		BF_R32 IMSK_CLR;                      /*!< Interrupt Mask Clear Register */
		BF_R32 IMSK_SET;                      /*!< Interrupt Mask Set Register */
							BF_R32                  z__RESERVED2;
		BF_R32 STAT;                       /*!< Status Register */
		BF_R32 _ILAT;                      /*!< Masked Interrupt Condition Register (_ILAT to avoid conflict with legacy macro) */
		BF_R32 ILAT_CLR;                   /*!< Masked Interrupt Clear Register */
							BF_R32                  z__RESERVED3;
		BF_R32 RFIFO;                      /*!< Receive FIFO Data Register */
							BF_R32                  z__RESERVED4;
		BF_R32 TFIFO;                      /*!< Transmit FIFO Data Register */
							BF_R32                  z__RESERVED5;
		BF_R32 MMRDH;                         /*!< Memory Mapped Read Header */
		BF_R32 MMTOP;                         /*!< SPI Memory Top Address */ 	
	};

	typedef S_SPI S_SPI0, S_SPI1, S_SPI2;

	#define SPI_MMSE    (1UL<<31)       /* Memory-Mapped SPI Enable */
	#define SPI_MMWEM   (1UL<<30)       /* Memory Mapped Write Error Mask */
	#define SPI_SOSI    (1UL<<22)       /* Start on MOSI */
	#define SPI_MIOM    (1UL<<20)       /* Multiple I/O Mode */
	#define SPI_FMODE   (1UL<<18)       /* Fast-Mode Enable */
	#define SPI_FCWM    (1UL<<16)       /* Flow Control Watermark */
	#define SPI_FCPL    (1UL<<15)       /* Flow Control Polarity */
	#define SPI_FCCH    (1UL<<14)       /* Flow Control Channel Selection */
	#define SPI_FCEN    (1UL<<13)       /* Flow Control Enable */
	#define SPI_LSBF    (1UL<<12)       /* Least Significant Bit First */
	#define SPI_SIZE    (1UL<< 9)       /* Word Transfer Size */
	#define SPI_EMISO   (1UL<< 8)       /* Enable MISO */
	#define SPI_SELST   (1UL<< 7)       /* Slave Select Polarity Between Transfers */
	#define SPI_ASSEL   (1UL<< 6)       /* Slave Select Pin Control */
	#define SPI_CPOL    (1UL<< 5)       /* Clock Polarity */
	#define SPI_CPHA    (1UL<< 4)       /* Clock Phase */
	#define SPI_ODM     (1UL<<3)        /* Open Drain Mode */
	#define SPI_PSSE    (1UL<<2)        /* Protected Slave Select Enable */
	#define SPI_MSTR    (1UL<<1)        /* Master/Slave */
	#define SPI_EN      (1UL<<0)        /* Enable */ 
												
	#define STAT_TFF		(1UL<<23)       /* SPI_TFIFO Full */
	#define STAT_RFE		(1UL<<22)       /* SPI_RFIFO Empty */
	#define STAT_FCS		(1UL<<20)       /* Flow Control Stall Indication */
	#define STAT_TFS		(1UL<<16)       /* SPI_TFIFO Status */
	#define STAT_RFS		(1UL<<12)       /* SPI_RFIFO Status */
	#define STAT_TF 		(1UL<<11)       /* Transmit Finish Indication */
	#define STAT_RF 		(1UL<<10)       /* Receive Finish Indication */
	#define STAT_TS  	(1UL<<9)		/* Transmit Start */
	#define STAT_RS  	(1UL<<8)        /* Receive Start */
	#define STAT_MF  	(1UL<<7)        /* Mode Fault Indication */
	#define STAT_TC  	(1UL<<6)        /* Transmit Collision Indication */
	#define STAT_TUR 	(1UL<<5)        /* Transmit Underrun Indication */
	#define STAT_ROR 	(1UL<<4)        /* Receive Overrun Indication */
	#define STAT_TUWM	(1UL<<2)        /* Transmit Urgent Watermark Breached */
	#define STAT_RUWM	(1UL<<1)        /* Receive Urgent Watermark Breached */
	#define STAT_SPIF	(1UL<<0)        /* SPI Finished */ 

	#define RXCTL_RUWM  	(1UL<<16)   	/* Receive FIFO Urgent Watermark */
	#define RXCTL_RRWM  	(1UL<<12)   	/* Receive FIFO Regular Watermark */
	#define RXCTL_RDO   	(1UL<< 8)   	/* Receive Data Overrun */
	#define RXCTL_RDR   	(1UL<< 4)   	/* Receive Data Request */
	#define RXCTL_RWCEN 	(1UL<< 3)   	/* Receive Word Counter Enable */
	#define RXCTL_RTI   	(1UL<< 2)   	/* Receive Transfer Initiate */
	#define RXCTL_REN   	(1UL<< 0)   	/* Receive Enable */ 
	#define TXCTL_TUWM  	(1UL<<16)   	/* FIFO Urgent Watermark */
	#define TXCTL_TRWM  	(1UL<<12)   	/* FIFO Regular Watermark */
	#define TXCTL_TDU   	(1UL<< 8)   	/* Transmit Data Under-run */
	#define TXCTL_TDR   	(1UL<< 4)   	/* Transmit Data Request */
	#define TXCTL_TWCEN 	(1UL<< 3)   	/* Transmit Word Counter Enable */
	#define TXCTL_TTI   	(1UL<< 2)   	/* Transmit Transfer Initiate */
	#define TXCTL_TEN   	(1UL<< 0)   	/* Transmit Enable */ 

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//struct S_TIMERx
	//{
	//	BF_R16	Config;				//	Timer x Configuration Register					
	//	BF_R16			z__Reserved1;
	//	BF_R32	Counter;			//	Timer x Counter Register						
	//	BF_R32	Period;				//	Timer x Period Register							
	//	BF_R32	Width;				//	Timer x Width Register		
	//};

	//typedef S_TIMERx S_TIMER0, S_TIMER1, S_TIMER2;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//struct S_TIMER
	//{
	//	BF_R16	Enable;					//	Timer Enable Register	
	//	BF_R16			z__Reserved1;
	//	BF_R16	Disable;				//	Timer Disable Register	
	//	BF_R16			z__Reserved2;
	//	BF_R16	Status;					//	Timer Status Register	
	//};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//struct S_PIO
	//{
	//	BF_R16	PinState;			//	Port I/O Pin State Specify Register				
	//	BF_R16			z__Reserved1;
	//	BF_R16	Clear;				//	Port I/O Peripheral Interrupt Clear Register	
	//	BF_R16			z__Reserved2;
	//	BF_R16	Set;				//	Port I/O Peripheral Interrupt Set Register		
	//	BF_R16			z__Reserved3;
	//	BF_R16	Toggle;				//	Port I/O Pin State Toggle Register				
	//	BF_R16			z__Reserved4;
	//	BF_R16	MaskA;				//	Port I/O Mask State Specify Interrupt A Register
	//	BF_R16			z__Reserved5;
	//	BF_R16	MaskA_Clr;			//	Port I/O Mask Disable Interrupt A Register		
	//	BF_R16			z__Reserved6;
	//	BF_R16	MaskA_Set;			//	Port I/O Mask Enable Interrupt A Register		
	//	BF_R16			z__Reserved7;
	//	BF_R16	MaskA_Tgl;			//	Port I/O Mask Toggle Enable Interrupt A Register
	//	BF_R16			z__Reserved8;
	//	BF_R16	MaskB;				//	Port I/O Mask State Specify Interrupt B Register
	//	BF_R16			z__Reserved9;
	//	BF_R16	MaskB_Clr;			//	Port I/O Mask Disable Interrupt B Register		
	//	BF_R16			z__Reserved10;
	//	BF_R16	MaskB_Set;			//	Port I/O Mask Enable Interrupt B Register		
	//	BF_R16			z__Reserved11;
	//	BF_R16	MaskB_Tgl;			//	Port I/O Mask Toggle Enable Interrupt B Register
	//	BF_R16			z__Reserved12;
	//	BF_R16	Dir;				//	Port I/O Direction Register						
	//	BF_R16			z__Reserved13;
	//	BF_R16	Polar;				//	Port I/O Source Polarity Register				
	//	BF_R16			z__Reserved14;
	//	BF_R16	Edge;				//	Port I/O Source Sensitivity Register			
	//	BF_R16			z__Reserved51;
	//	BF_R16	Both;				//	Port I/O Set on BOTH Edges Register				
	//	BF_R16			z__Reserved16;
	//	BF_R16	Inen;				//	Port I/O Input Enable Register 		

	//	inline void 	SET(u16 m) 			{ Set = m; }
	//	inline void 	CLR(u16 m) 			{ Clear = m; }
	//	inline void 	NOT(u16 m) 			{ Toggle = m; }
	//	inline void 	WBIT(u16 m, bool c) { if (c) SET(m); else CLR(m); }
	//	inline void 	BSET(u16 b) 		{ Set = 1UL<< b; }
	//	inline void 	BCLR(u16 b) 		{ Clear = 1UL << b; }
	//	inline void 	BTGL(u16 b) 		{ Toggle = 1UL << b; }

	//	inline bool 	TBSET(u16 b) 		{ return PinState & (1<<b); }
	//	inline bool 	TBCLR(u16 b) 		{ return (PinState & (1<<b)) == 0; }

	//	inline void 	DirSet(u16 m) 		{ Dir |= m; }
	//	inline void 	DirClr(u16 m) 		{ Dir &= ~m; }

	//	inline void 	SetMaskA(u32 m) 	{ MaskA_Set = m; }
	//	inline void 	SetMaskB(u32 m) 	{ MaskB_Set = m; }
	//	inline void 	ClrMaskA(u32 m) 	{ MaskA_Clr = m; }
	//	inline void 	ClrMaskB(u32 m) 	{ MaskB_Clr = m; }

	//	inline void		SetFER(u16 m)		{ if ((&PinState) == (pPORTFIO)) *pPORTF_FER |= m; else *pPORTG_FER |= m;  }
	//	inline void		SetMUX(u16 m)		{ if ((&PinState) == (pPORTFIO)) *pPORTF_MUX |= m; else *pPORTG_MUX |= m;  }
	//	inline void		ClrFER(u16 m)		{ if ((&PinState) == (pPORTFIO)) *pPORTF_FER &= ~m; else *pPORTG_FER &= ~m;  }
	//	inline void		ClrMUX(u16 m)		{ if ((&PinState) == (pPORTFIO)) *pPORTF_MUX &= ~m; else *pPORTG_MUX &= ~m;  }

	//	inline void		WrFER(u16 m)		{ if ((&PinState) == (pPORTFIO)) *pPORTF_FER = m; else *pPORTG_FER = m;  }
	//	inline void		WrMUX(u16 m)		{ if ((&PinState) == (pPORTFIO)) *pPORTF_MUX = m; else *pPORTG_MUX = m;  }

	//	inline void		EnableIRQA_Low(u16 m)	{ ClrFER(m); DirClr(m); Inen |= m; Edge &= ~m;	Polar |= m;					CLR(m); MaskA_Set = m; }
	//	inline void		EnableIRQA_High(u16 m) 	{ ClrFER(m); DirClr(m); Inen |= m; Edge &= ~m;	Polar &= ~m;				CLR(m); MaskA_Set = m; }
	//	inline void		EnableIRQA_Rise(u16 m) 	{ ClrFER(m); DirClr(m); Inen |= m; Edge |= m;	Polar &= ~m;	Both &= ~m; CLR(m); MaskA_Set = m; }
	//	inline void		EnableIRQA_Fall(u16 m) 	{ ClrFER(m); DirClr(m); Inen |= m; Edge |= m;	Polar |= m;		Both &= ~m; CLR(m); MaskA_Set = m; }
	//	inline void		EnableIRQA_Both(u16 m) 	{ ClrFER(m); DirClr(m); Inen |= m; Edge |= m;					Both |= m;	CLR(m); MaskA_Set = m; }

	//	inline void		EnableIRQB_Low(u16 m)	{ ClrFER(m); DirClr(m); Inen |= m; Edge &= ~m;	Polar |= m;					CLR(m); MaskB_Set = m; }
	//	inline void		EnableIRQB_High(u16 m) 	{ ClrFER(m); DirClr(m); Inen |= m; Edge &= ~m;	Polar &= ~m;				CLR(m); MaskB_Set = m; }
	//	inline void		EnableIRQB_Rise(u16 m) 	{ ClrFER(m); DirClr(m); Inen |= m; Edge |= m;	Polar &= ~m;	Both &= ~m; CLR(m); MaskB_Set = m; }
	//	inline void		EnableIRQB_Fall(u16 m) 	{ ClrFER(m); DirClr(m); Inen |= m; Edge |= m;	Polar |= m;		Both &= ~m; CLR(m); MaskB_Set = m; }
	//	inline void		EnableIRQB_Both(u16 m) 	{ ClrFER(m); DirClr(m); Inen |= m; Edge |= m;					Both |= m;	CLR(m); MaskB_Set = m; }

	//	inline void		ClearTriggerIRQ(u16 m)	{ Clear = m; }
	//};

	//typedef S_PIO S_PIOF, S_PIOG;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_PORT
	{
		BF_R32	FER;                           /*!< Port x Function Enable Register */
		BF_R32	FER_SET;                       /*!< Port x Function Enable Set Register */
		BF_R32	FER_CLR;                       /*!< Port x Function Enable Clear Register */
		BF_R32	DATA;                          /*!< Port x GPIO Data Register */
		BF_R32	DATA_SET;                      /*!< Port x GPIO Data Set Register */
		BF_R32	DATA_CLR;                      /*!< Port x GPIO Data Clear Register */
		BF_R32	DIR;                           /*!< Port x GPIO Direction Register */
		BF_R32	DIR_SET;                       /*!< Port x GPIO Direction Set Register */
		BF_R32	DIR_CLR;                       /*!< Port x GPIO Direction Clear Register */
		BF_R32	INEN;                          /*!< Port x GPIO Input Enable Register */
		BF_R32	INEN_SET;                      /*!< Port x GPIO Input Enable Set Register */
		BF_R32	INEN_CLR;                      /*!< Port x GPIO Input Enable Clear Register */
		BF_R32	MUX;                           /*!< Port x Multiplexer Control Register */
		BF_R32	DATA_TGL;                      /*!< Port x GPIO Input Enable Toggle Register */
		BF_R32	POL;                           /*!< Port x GPIO Polarity Invert Register */
		BF_R32	POL_SET;                       /*!< Port x GPIO Polarity Invert Set Register */
		BF_R32	POL_CLR;                       /*!< Port x GPIO Polarity Invert Clear Register */
		BF_R32	LOCK;                          /*!< Port x GPIO Lock Register */
		BF_R32	TRIG_TGL;                      /*!< Port x GPIO Trigger Toggle Register */ 	

		inline void 	SET(u16 m) 			{ DATA_SET = m;					}
		inline void 	CLR(u16 m) 			{ DATA_CLR = m;					}
		inline void 	NOT(u16 m) 			{ DATA_TGL = m;					}
		inline void 	WBIT(u16 m, bool c) { if (c) SET(m); else CLR(m);	}
		inline void 	BSET(u16 b) 		{ SET(1UL<< b);					}
		inline void 	BCLR(u16 b) 		{ CLR(1UL << b);				}
		inline void 	BTGL(u16 b) 		{ NOT(1UL << b);				}

		inline bool 	TBSET(u16 b) 		{ return DATA & (1<<b); }
		inline bool 	TBCLR(u16 b) 		{ return (DATA & (1<<b)) == 0; }

		inline void 	DirSet(u16 m) 		{ DIR_SET = m; }
		inline void 	DirClr(u16 m) 		{ DIR_CLR = m; }
		
		inline void		SetMUX(byte pin, byte v) { MUX = (MUX & ~(3UL<<pin)) | ((v&3)<<pin); }

	};

	typedef S_PORT S_PORTA, S_PORTB, S_PORTC, S_PIOA, S_PIOB, S_PIOC;

	#define PA0 	(1UL<<0)
	#define PA1 	(1UL<<1)
	#define PA2 	(1UL<<2)
	#define PA3 	(1UL<<3)
	#define PA4 	(1UL<<4)
	#define PA5 	(1UL<<5)
	#define PA6 	(1UL<<6)
	#define PA7 	(1UL<<7)
	#define PA8 	(1UL<<8)
	#define PA9 	(1UL<<9)
	#define PA10 	(1UL<<10)
	#define PA11 	(1UL<<11)
	#define PA12 	(1UL<<12)
	#define PA13 	(1UL<<13)
	#define PA14 	(1UL<<14)
	#define PA15 	(1UL<<15)

	#define PB0 	(1UL<<0)
	#define PB1 	(1UL<<1)
	#define PB2 	(1UL<<2)
	#define PB3 	(1UL<<3)
	#define PB4 	(1UL<<4)
	#define PB5 	(1UL<<5)
	#define PB6 	(1UL<<6)
	#define PB7 	(1UL<<7)
	#define PB8 	(1UL<<8)
	#define PB9 	(1UL<<9)
	#define PB10 	(1UL<<10)
	#define PB11 	(1UL<<11)
	#define PB12 	(1UL<<12)
	#define PB13 	(1UL<<13)
	#define PB14 	(1UL<<14)
	#define PB15 	(1UL<<15)

	#define PC0 	(1UL<<0)
	#define PC1 	(1UL<<1)
	#define PC2 	(1UL<<2)
	#define PC3 	(1UL<<3)
	#define PC4 	(1UL<<4)
	#define PC5 	(1UL<<5)
	#define PC6 	(1UL<<6)
	#define PC7 	(1UL<<7)
	#define PC8 	(1UL<<8)
	#define PC9 	(1UL<<9)
	#define PC10 	(1UL<<10)
	#define PC11 	(1UL<<11)
	#define PC12 	(1UL<<12)
	#define PC13 	(1UL<<13)
	#define PC14 	(1UL<<14)
	#define PC15 	(1UL<<15)


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//struct S_SPORT
	//{
	//	BF_R16	TCR1;						//	SPORT Transmit Configuration 1 Register			
	//	BF_R16			z__Reserved1;
	//	BF_R16	TCR2;						//	SPORT Transmit Configuration 2 Register			
	//	BF_R16			z__Reserved2;
	//	BF_R16	TCLKDIV;					//	SPORT Transmit Clock Divider					
	//	BF_R16			z__Reserved3;
	//	BF_R16	TFSDIV;						//	SPORT Transmit Frame Sync Divider				
	//	BF_R16			z__Reserved4;
	//	BF_R32	TX;							//	SPORT TX Data Register							
	//	BF_R32			z__Reserved5;
	//	BF_R32	RX;							//	SPORT RX Data Register							
	//	BF_R32			z__Reserved51;
	//	BF_R16	RCR1;						//	SPORT Transmit Configuration 1 Register			
	//	BF_R16			z__Reserved6;
	//	BF_R16	RCR2;						//	SPORT Transmit Configuration 2 Register			
	//	BF_R16			z__Reserved7;
	//	BF_R16	RCLKDIV;					//	SPORT Receive Clock Divider						
	//	BF_R16			z__Reserved8;
	//	BF_R16	RFSDIV;						//	SPORT Receive Frame Sync Divider				
	//	BF_R16			z__Reserved9;
	//	BF_R16	STAT;						//	SPORT Status Register							
	//	BF_R16			z__Reserved10;
	//	BF_R16	CHNL;						//	SPORT Current Channel Register					
	//	BF_R16			z__Reserved11;
	//	BF_R16	MCMC1;						//	SPORT Multi-Channel Configuration Register 1	
	//	BF_R16			z__Reserved12;
	//	BF_R16	MCMC2;						//	SPORT Multi-Channel Configuration Register 2	
	//	BF_R16			z__Reserved13;
	//	BF_R32	MTCS0;						//	SPORT Multi-Channel Transmit Select Register 0	
	//	BF_R32	MTCS1;						//	SPORT Multi-Channel Transmit Select Register 1	
	//	BF_R32	MTCS2;						//	SPORT Multi-Channel Transmit Select Register 2	
	//	BF_R32	MTCS3;						//	SPORT Multi-Channel Transmit Select Register 3	
	//	BF_R32	MRCS0;						//	SPORT Multi-Channel Receive Select Register 0	
	//	BF_R32	MRCS1;						//	SPORT Multi-Channel Receive Select Register 1	
	//	BF_R32	MRCS2;						//	SPORT Multi-Channel Receive Select Register 2	
	//	BF_R32	MRCS3;						//	SPORT Multi-Channel Receive Select Register 3	
	//};

	//typedef S_SPORT S_SPORT0, S_SPORT1;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	//struct DMADSC_AM
	//{
	//	union { u32 SA;	struct { u16 SAL; u16 SAH; }; };
	//	u16 DMACFG;
	//	u16 XCNT;
	//	u16 XMOD;
	//	u16 YCNT;
	//	u16 YMOD;
	//};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	//struct DMADSC_SLM
	//{
	//	u16 NDPL;
	//	u16 SAL; 
	//	u16 SAH; 
	//	u16 DMACFG;
	//	u16 XCNT;
	//	u16 XMOD;
	//	u16 YCNT;
	//	u16 YMOD;

	//	inline void SA(u32 v) { SAL = v; SAH = v>>16; }
	//};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//struct DMADSC_LLM
	//{
	//	union { void* NDP; struct { u16 NDPL; u16 NDPH; }; };
	//	union { void* SA;	struct { u16 SAL; u16 SAH; }; };
	//	u16 DMACFG;
	//	u16 XCNT;
	//	u16 XMOD;
	//	u16 YCNT;
	//	u16 YMOD;
	//};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	//struct S_DMACH
	//{
	//	BF_PTR	NEXT_DESC_PTR;									//	DMA Channel x Next Descriptor Pointer Register		
	//	BF_PTR	START_ADDR;										//	DMA Channel x Start Address Register				
	//	BF_R16	CONFIG;											//	DMA Channel x Configuration Register				
	//							BF_R16	z__Reserved1[3];							
	//	BF_R16	X_COUNT;										//	DMA Channel x X Count Register						
	//							BF_R16	z__Reserved2;							
	//	BF_R16	X_MODIFY;										//	DMA Channel x X Modify Register						
	//							BF_R16	z__Reserved3;									
	//	BF_R16	Y_COUNT;										//	DMA Channel x Y Count Register						
	//							BF_R16	z__Reserved4;									
	//	BF_R16	Y_MODIFY;										//	DMA Channel x Y Modify Register						
	//							BF_R16	z__Reserved5;									
	//	BF_PTR	CURR_DESC_PTR;									//	DMA Channel x Current Descriptor Pointer Register	
	//	BF_PTR	CURR_ADDR;										//	DMA Channel x Current Address Register				
	//	BF_R16	IRQ_STATUS;										//	DMA Channel x Interrupt/Status Register				
	//							BF_R16	z__Reserved6;									
	//	BF_R16	PERIPHERAL_MAP;									//	DMA Channel x Peripheral Map Register				
	//							BF_R16	z__Reserved7;									
	//	BF_R16	CURR_X_COUNT;									//	DMA Channel x Current X Count Register				
	//							BF_R16	z__Reserved8[3];									
	//	BF_R16	CURR_Y_COUNT;									//	DMA Channel x Current Y Count Register				
	//							BF_R16	z__Reserved9[3];
	//};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//struct S_DMA
	//{
	//	BF_R16	TC_PER;						// 0xFFC00B0C	/* Traffic Control Periods Register						
	//						BF_R16	z__Reserved1;
	//	BF_R16	TC_CNT;						// 0xFFC00B10	/* Traffic Control Current Counts Register				
	//						BF_R16	z__Reserved2;
	//						BF_R32	z__Reserved3[59];
	//	S_DMACH	CH[9];
	//						BF_R32	z__Reserved4[49];
	//	S_DMACH	 MDMA_D0;
	//	S_DMACH	 MDMA_S0;
	//	S_DMACH	 MDMA_D1;
	//	S_DMACH	 MDMA_S1;
	//};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//struct S_PPI
	//{
	//	BF_R32	Control;			//	PPI Control Register		
	//	BF_R32	Status;				//	PPI Status Register			
	//	BF_R32	Count;				//	PPI Transfer Count Register	
	//	BF_R32	Delay;				//	PPI Delay Count Register	
	//	BF_R32	Frame;				//	PPI Frame Length Register	
	//};
	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//struct S_TWI
	//{
	//	BF_R32	CLKDIV;				//	Serial Clock Divider Register			
	//	BF_R32	CONTROL;			//	TWI Control Register						
	//	BF_R32	SLAVE_CTL;			//	Slave Mode Control Register				
	//	BF_R32	SLAVE_STAT;			//	Slave Mode Status Register				
	//	BF_R32	SLAVE_ADDR;			//	Slave Mode Address Register				
	//	BF_R32	MASTER_CTL;			//	Master Mode Control Register				
	//	BF_R32	MASTER_STAT;		//	Master Mode Status Register				
	//	BF_R32	MASTER_ADDR;		//	Master Mode Address Register				
	//	BF_R32	INT_STAT;			//	TWI Interrupt Status Register			
	//	BF_R32	INT_MASK;			//	TWI Master Interrupt Mask Register		
	//	BF_R32	FIFO_CTL;			//	FIFO Control Register					
	//	BF_R32	FIFO_STAT;			//	FIFO Status Register						
	//	BF_R32	XMT_DATA8;			//	FIFO Transmit Data Single Byte Register	
	//	BF_R32	XMT_DATA16;			//	FIFO Transmit Data Double Byte Register	
	//	BF_R32	RCV_DATA8;			//	FIFO Receive Data Single Byte Register	
	//	BF_R32	RCV_DATA16;			//	FIFO Receive Data Double Byte Register	
	//};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//struct S_WDT
	//{
	//	BF_R16	CTL;		// Watchdog Control Register
	//						BF_R16	z__Reserved1;
	//	BF_R32	CNT;		// Watchdog Count Register							
	//	BF_R32	STAT;		// Watchdog Status Register							

	//	inline void Update()	{ STAT = 0; }
	//	inline void Reset()		{ STAT = 0; }
	//	inline void Disable()	{ CTL = WDDIS; }
	//};

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

#pragma diag(push)
#pragma diag(suppress: 1967)

	//MK_PTR(SIC,		SIC_BASE_ADR);
	//MK_PTR(EIC,		EIC_BASE_ADR);
	
	MK_PTR(SPI0,	ADI_SPI0_BASE);
	MK_PTR(SPI1,	ADI_SPI1_BASE);
	MK_PTR(SPI2,	ADI_SPI2_BASE);

	//MK_PTR(TIMER0, 	TIMER0_CONFIG);
	//MK_PTR(TIMER1, 	TIMER1_CONFIG);
	//MK_PTR(TIMER2, 	TIMER2_CONFIG);
	//MK_PTR(TIMER,	TIMER_ENABLE);
	//MK_PTR(PIOF,	PORTFIO);
	//MK_PTR(PIOG,	PORTGIO);

	MK_PTR(PORTA,	ADI_PORTA_BASE);
	MK_PTR(PORTB,	ADI_PORTB_BASE);
	MK_PTR(PORTC,	ADI_PORTC_BASE);

	MK_PTR(PIOA,	ADI_PORTA_BASE);
	MK_PTR(PIOB,	ADI_PORTB_BASE);
	MK_PTR(PIOC,	ADI_PORTC_BASE);

	//MK_PTR(SPORT0,	SPORT0_TCR1);
	//MK_PTR(SPORT1,	SPORT1_TCR1);

	//MK_PTR(DMA,		DMA_TC_PER);

	//MK_PTR(PPI,		PPI_CONTROL);

	//MK_PTR(TWI,		TWI_CLKDIV);
	//MK_PTR(WDT,		WDOG_CTL);


#pragma diag(pop)


	//inline void ResetWDT()		{ *pWDOG_STAT = 0; }
	//inline void DisableWDT()	{ *pWDOG_CTL = WDDIS; }


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


} // namespace HW

//extern T_HW::BF_IHP VectorTableInt[16];
//extern T_HW::BF_IHP VectorTableExt[45];

#define PID_Watchdog_Timer				(31)
#define PID_DMA_14_15_Mem_DMA_Stream_1	(30)
#define PID_DMA_12_13_Mem_DMA_Stream_0	(29)
#define PID_TWI							(24)
#define PID_Port_G_Interrupt_B			(23)
#define PID_Port_G_Interrupt_A			(22)
#define PID_GP_Timer_2					(21)
#define PID_GP_Timer_1					(20)
#define PID_GP_Timer_0					(19)
#define PID_Port_F_Interrupt_B			(18)
#define PID_Port_F_Interrupt_A			(17)
#define PID_DMA8_UART0_TX				(16)
#define PID_DMA7_UART0_RX				(15)
#define PID_DMA6_SPI1_RX_TX				(14)
#define PID_DMA5_SPI0_RX_TX				(13)
#define PID_DMA4_SPORT1_TX				(12)
#define PID_DMA3_SPORT1_RX				(11)
#define PID_DMA2_SPORT0_TX				(10)
#define PID_DMA1_SPORT0_RX				(9)
#define PID_DMA0_PPI					(8)
#define PID_UART0_Status				(7)
#define PID_SPI1_Status					(6) 
#define PID_SPI0_Status					(5) 
#define PID_SPORT1_Status				(4) 
#define PID_SPORT0_Status				(3) 
#define PID_PPI_Status					(2) 
#define PID_DMA_Error_generic			(1) 
#define PID_PLL_Wakeup					(0) 


#undef MK_PTR
#undef MKPID

#endif // BF706_H__16_07_2024__15_40
