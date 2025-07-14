#ifndef SAM4SA_H__27_08_2014__11_07
#define SAM4SA_H__27_08_2014__11_07

#ifndef CORETYPE_SAM4SA
#error  Must #include "core.h"
#endif 

#include "types.h"
#include "cm4.h"

//#define MCK 100000000


#ifndef WIN32
#define MK_PTR(n,p)  T_HW::S_##n * const n = ((T_HW::S_##n*)(p))
#else
extern byte core_sys_array[0x100000]; 
#define MK_PTR(n,p)  T_HW::S_##n * const n = ((T_HW::S_##n*)(core_sys_array-0x40000000+p))
#endif

#define MKPID(n,i) n##_M=(1UL<<(i&31)), n##_I=i

#define SUPC_IRQ		0	/**<  0 SAM4SA16B Supply Controller (SUPC) */
#define RSTC_IRQ		1	/**<  1 SAM4SA16B Reset Controller (RSTC) */
#define RTC_IRQ			2	/**<  2 SAM4SA16B Real Time Clock (RTC) */
#define RTT_IRQ			3	/**<  3 SAM4SA16B Real Time Timer (RTT) */
#define WDT_IRQ			4	/**<  4 SAM4SA16B Watchdog Timer (WDT) */
#define PMC_IRQ			5	/**<  5 SAM4SA16B Power Management Controller (PMC) */
#define EFC0_IRQ		6	/**<  6 SAM4SA16B Enhanced Embedded Flash Controller 0 (EFC0) */
#define UART0_IRQ		8	/**<  8 SAM4SA16B UART 0 (UART0) */
#define UART1_IRQ		9	/**<  9 SAM4SA16B UART 1 (UART1) */
#define PIOA_IRQ		11	/**< 11 SAM4SA16B Parallel I/O Controller A (PIOA) */
#define PIOB_IRQ		12	/**< 12 SAM4SA16B Parallel I/O Controller B (PIOB) */
#define USART0_IRQ		14	/**< 14 SAM4SA16B USART 0 (USART0) */
#define USART1_IRQ		15	/**< 15 SAM4SA16B USART 1 (USART1) */
#define HSMCI_IRQ		18	/**< 18 SAM4SA16B Multimedia Card Interface (HSMCI) */
#define TWI0_IRQ		19	/**< 19 SAM4SA16B Two Wire Interface 0 (TWI0) */
#define TWI1_IRQ		20	/**< 20 SAM4SA16B Two Wire Interface 1 (TWI1) */
#define SPI_IRQ			21	/**< 21 SAM4SA16B Serial Peripheral Interface (SPI) */
#define SSC_IRQ			22	/**< 22 SAM4SA16B Synchronous Serial Controller (SSC) */
#define TC0_IRQ			23	/**< 23 SAM4SA16B Timer/Counter 0 (TC0) */
#define TC1_IRQ			24	/**< 24 SAM4SA16B Timer/Counter 1 (TC1) */
#define TC2_IRQ			25	/**< 25 SAM4SA16B Timer/Counter 2 (TC2) */
#define ADC_IRQ			29	/**< 29 SAM4SA16B Analog To Digital Converter (ADC) */
#define DACC_IRQ		30	/**< 30 SAM4SA16B Digital To Analog Converter (DACC) */
#define PWM_IRQ			31	/**< 31 SAM4SA16B Pulse Width Modulation (PWM) */
#define CRCCU_IRQ		32	/**< 32 SAM4SA16B CRC Calculation Unit (CRCCU) */
#define ACC_IRQ			33	/**< 33 SAM4SA16B Analog Comparator (ACC) */
#define UDP_IRQ			34	/**< 34 SAM4SA16B USB Device Port (UDP) */

#pragma push
#pragma anon_unions
#pragma diag_suppress 368,381,826


namespace T_HW
{
	typedef volatile u32				AT91_REG;	// Hardware register definition
	typedef volatile __writeonly u32	AT91_WO;	// Hardware register definition
	typedef volatile const u32		AT91_RO;	// Hardware register definition
	typedef volatile void *			AT91_PTR;	// Hardware register definition

	typedef void(*AT91_IHP)() __irq;	// Interrupt handler pointer

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_SMC
	{
		AT91_REG	NFC_CFG;	
		AT91_REG	NFC_CTRL;	
		AT91_REG	NFC_SR;	
		AT91_REG	NFC_IER;	
		AT91_REG	NFC_IDR;	
		AT91_REG	NFC_IMR;	
		AT91_REG	NFC_ADDR;	
		AT91_REG	NFC_BANK;	
		AT91_REG	ECC_CTRL;	
		AT91_REG	ECC_MD;	
		AT91_REG	ECC_SR1;	
		AT91_REG	ECC_PR0;	
		AT91_REG	ECC_PR1;	
		AT91_REG	ECC_SR2;	
		AT91_REG	ECC_PR2;	
		AT91_REG	ECC_PR3;	
		AT91_REG	ECC_PR4;	
		AT91_REG	ECC_PR5;	
		AT91_REG	ECC_PR6;	
		AT91_REG	ECC_PR7;	
		AT91_REG	ECC_PR8;	
		AT91_REG	ECC_PR9;	
		AT91_REG	ECC_PR10;	
		AT91_REG	ECC_PR11;	
		AT91_REG	ECC_PR12;	
		AT91_REG	ECC_PR13;	
		AT91_REG	ECC_PR14;	
		AT91_REG	ECC_PR15;	

		struct S_CSR { AT91_REG	SETUP, PULSE, CYCLE, TIMINGS, MODE;	} CSR[8];

		AT91_REG	OCMS;	
		AT91_REG	KEY1;	
		AT91_REG	KEY2;	
		AT91_REG	WPCR;	
		AT91_REG	WPSR;	
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_PMC
	{
		AT91_REG	SCER;
		AT91_REG	SCDR;
		AT91_REG	SCSR;
		AT91_REG	z__reserved1;
		AT91_REG	PCER0;
		AT91_REG	PCDR0;
		AT91_REG	PCSR0;
		AT91_REG	z__reserved2;
		AT91_REG	MOR;
		AT91_REG	MCFR;
		AT91_REG	PLLAR;
		AT91_REG	PLLBR;
		AT91_REG	MCKR;
		AT91_REG	z__reserved4;
		AT91_REG	USBCR;
		AT91_REG	z__reserved5;
		AT91_REG	PCK[3];
		AT91_REG	z__reserved6[5];
		AT91_REG	IER;
		AT91_REG	IDR;
		AT91_REG	SR;
		AT91_REG	IMR;
		AT91_REG	FSMR;
		AT91_REG	FSPR;
		AT91_REG	FOCR;
		AT91_REG	z__reserved7[26];
		AT91_REG	WPMR;
		AT91_REG	WPSR;
		AT91_REG	z__reserved8[5];
		AT91_REG	PCER1;
		AT91_REG	PCDR1;
		AT91_REG	PCSR1;
		AT91_REG	z__reserved9;
		AT91_REG	OCR;

		void ClockEnable(byte pid)	{ if (pid < 32) PCER0 = (1UL<<pid); else PCER1 = (1UL<<(pid-32)); }
		void ClockDisable(byte pid)	{ if (pid < 32) PCDR0 = (1UL<<pid); else PCDR1 = (1UL<<(pid-32)); }
	};

	/* -------- CKGR_MOR : (PMC Offset: 0x0020) Main Oscillator Register -------- */

	#define PMC_MOSCXTEN			(0x1u << 0)				/**< \brief (CKGR_MOR) Main Crystal Oscillator Enable */
	#define PMC_MOSCXTBY			(0x1u << 1)				/**< \brief (CKGR_MOR) Main Crystal Oscillator Bypass */
	#define PMC_WAITMODE			(0x1u << 2)				/**< \brief (CKGR_MOR) Wait Mode Command */
	#define PMC_MOSCRCEN			(0x1u << 3)				/**< \brief (CKGR_MOR) Main On-Chip RC Oscillator Enable */
	#define	PMC_MOSCRCF_4_MHz		(0x0u << 4)				/**< \brief (CKGR_MOR) The Fast RC Oscillator Frequency is at 4 MHz (default) */
	#define	PMC_MOSCRCF_8_MHz		(0x1u << 4)				/**< \brief (CKGR_MOR) The Fast RC Oscillator Frequency is at 8 MHz */
	#define	PMC_MOSCRCF_12_MHz		(0x2u << 4)				/**< \brief (CKGR_MOR) The Fast RC Oscillator Frequency is at 12 MHz */
	#define PMC_MOSCXTST_MSK		(0xffu << 8)			/**< \brief (CKGR_MOR) Main Crystal Oscillator Start-up Time */
	#define PMC_MOSCXTST(value)		(((value)&0xFF) << 8)	/**< \brief (CKGR_MOR) Main Crystal Oscillator Start-up Time */
	#define	PMC_KEY_PASSWD			(0x37u << 16)			/**< \brief (CKGR_MOR) Writing any other value in this field aborts the write operation.Always reads as 0. */
	#define PMC_MOSCSEL				(0x1u << 24)			/**< \brief (CKGR_MOR) Main Oscillator Selection */
	#define PMC_CFDEN				(0x1u << 25)			/**< \brief (CKGR_MOR) Clock Failure Detector Enable */

	/* -------- PMC_SR : (PMC Offset: 0x0068) Status Register -------- */

	#define PMC_MOSCXTS		(0x1u << 0)		/**< \brief (PMC_SR) Main XTAL Oscillator Status */
	#define PMC_LOCKA		(0x1u << 1)		/**< \brief (PMC_SR) PLLA Lock Status */
	#define PMC_LOCKB		(0x1u << 2)		/**< \brief (PMC_SR) PLLB Lock Status */
	#define PMC_MCKRDY		(0x1u << 3)		/**< \brief (PMC_SR) Master Clock Status */
	#define PMC_OSCSELS		(0x1u << 7)		/**< \brief (PMC_SR) Slow Clock Oscillator Selection */
	#define PMC_PCKRDY0		(0x1u << 8)		/**< \brief (PMC_SR) Programmable Clock Ready Status */
	#define PMC_PCKRDY1		(0x1u << 9)		/**< \brief (PMC_SR) Programmable Clock Ready Status */
	#define PMC_PCKRDY2		(0x1u << 10)	/**< \brief (PMC_SR) Programmable Clock Ready Status */
	#define PMC_MOSCSELS	(0x1u << 16)	/**< \brief (PMC_SR) Main Oscillator Selection Status */
	#define PMC_MOSCRCS		(0x1u << 17)	/**< \brief (PMC_SR) Main On-Chip RC Oscillator Status */
	#define PMC_CFDEV		(0x1u << 18)	/**< \brief (PMC_SR) Clock Failure Detector Event */
	#define PMC_CFDS		(0x1u << 19)	/**< \brief (PMC_SR) Clock Failure Detector Status */
	#define PMC_FOS			(0x1u << 20)	/**< \brief (PMC_SR) Clock Failure Detector Fault Output Status */

	/* -------- CKGR_PLLAR : (PMC Offset: 0x0028) PLLA Register -------- */

	#define PMC_DIVA_MSK			(0xffu << 0)				/**< \brief (CKGR_PLLAR) PLLA Front_End Divider */
	#define PMC_DIVA(value)			(((value)&0xFF) << 0)		/**< \brief (CKGR_PLLAR) PLLA Front_End Divider */
	#define PMC_PLLACOUNT_MSK		(0x3fu << 8)				/**< \brief (CKGR_PLLAR) PLLA Counter */
	#define PMC_PLLACOUNT(value)	(((value)&0x3F) << 8)		/**< \brief (CKGR_PLLAR) PLLA Counter */
	#define PMC_MULA_MSK			(0x7ffu << 16)				/**< \brief (CKGR_PLLAR) PLLA Multiplier */
	#define PMC_MULA(value)			(((value)&0x7FF) << 16)		/**< \brief (CKGR_PLLAR) PLLA Multiplier */
	#define PMC_ONE					(0x1u << 29)				/**< \brief (CKGR_PLLAR) Must Be Set to 1 */

	/* -------- CKGR_PLLBR : (PMC Offset: 0x002C) PLLB Register -------- */
	
	#define PMC_DIVB_MSK			(0xffu << 0)			/**< \brief (CKGR_PLLBR) PLLB Front-End Divider */
	#define PMC_DIVB(value)			(((value)&0xFF) << 0)	/**< \brief (CKGR_PLLBR) PLLB Front-End Divider */
	#define PMC_PLLBCOUNT_MSK		(0x3fu << 8)			/**< \brief (CKGR_PLLBR) PLLB Counter */
	#define PMC_PLLBCOUNT(value)	(((value)&0x3F) << 8)	/**< \brief (CKGR_PLLBR) PLLB Counter */
	#define PMC_MULB_MSK			(0x7ffu << 16)			/**< \brief (CKGR_PLLBR) PLLB Multiplier */
	#define PMC_MULB(value)			(((value)&0x7FF) << 16)	/**< \brief (CKGR_PLLBR) PLLB Multiplier */

	/* -------- PMC_MCKR : (PMC Offset: 0x0030) Master Clock Register -------- */

	#define	MCKR_SLOW_CLK	(0x0u << 0)		/**< \brief (PMC_MCKR) Slow Clock is selected */
	#define	MCKR_MAIN_CLK	(0x1u << 0)		/**< \brief (PMC_MCKR) Main Clock is selected */
	#define	MCKR_PLLA_CLK	(0x2u << 0)		/**< \brief (PMC_MCKR) PLLA Clock is selected */
	#define	MCKR_PLLB_CLK	(0x3u << 0)		/**< \brief (PMC_MCKR) PLLBClock is selected */
	#define	MCKR_PRES_1		(0x0u << 4)		/**< \brief (PMC_MCKR) Selected clock */
	#define	MCKR_PRES_2		(0x1u << 4)		/**< \brief (PMC_MCKR) Selected clock divided by 2 */
	#define	MCKR_PRES_4		(0x2u << 4)		/**< \brief (PMC_MCKR) Selected clock divided by 4 */
	#define	MCKR_PRES_8		(0x3u << 4)		/**< \brief (PMC_MCKR) Selected clock divided by 8 */
	#define	MCKR_PRES_16	(0x4u << 4)		/**< \brief (PMC_MCKR) Selected clock divided by 16 */
	#define	MCKR_PRES_32	(0x5u << 4)		/**< \brief (PMC_MCKR) Selected clock divided by 32 */
	#define	MCKR_PRES_64	(0x6u << 4)		/**< \brief (PMC_MCKR) Selected clock divided by 64 */
	#define	MCKR_PRES_3		(0x7u << 4)		/**< \brief (PMC_MCKR) Selected clock divided by 3 */
	#define	MCKR_PLLADIV2	(0x1u << 12)	/**< \brief (PMC_MCKR) PLLA Divisor by 2 */
	#define	MCKR_PLLBDIV2	(0x1u << 13)	/**< \brief (PMC_MCKR) PLLB Divisor by 2 */

	/* -------- PMC_PCK[3] : (PMC Offset: 0x0040) Programmable Clock 0 Register -------- */

	#define   PMC_PCK_SLOW_CLK	(0x0u << 0) /**< \brief (PMC_PCK[3]) Slow Clock is selected */
	#define   PMC_PCK_MAIN_CLK	(0x1u << 0) /**< \brief (PMC_PCK[3]) Main Clock is selected */
	#define   PMC_PCK_PLLA_CLK	(0x2u << 0) /**< \brief (PMC_PCK[3]) PLLA Clock is selected */
	#define   PMC_PCK_PLLB_CLK	(0x3u << 0) /**< \brief (PMC_PCK[3]) PLLB Clock is selected */
	#define   PMC_PCK_MCK		(0x4u << 0) /**< \brief (PMC_PCK[3]) Master Clock is selected */
	#define   PMC_PCK_PRES_1	(0x0u << 4) /**< \brief (PMC_PCK[3]) Selected clock */
	#define   PMC_PCK_PRES_2	(0x1u << 4) /**< \brief (PMC_PCK[3]) Selected clock divided by 2 */
	#define   PMC_PCK_PRES_4	(0x2u << 4) /**< \brief (PMC_PCK[3]) Selected clock divided by 4 */
	#define   PMC_PCK_PRES_8	(0x3u << 4) /**< \brief (PMC_PCK[3]) Selected clock divided by 8 */
	#define   PMC_PCK_PRES_16	(0x4u << 4) /**< \brief (PMC_PCK[3]) Selected clock divided by 16 */
	#define   PMC_PCK_PRES_32	(0x5u << 4) /**< \brief (PMC_PCK[3]) Selected clock divided by 32 */
	#define   PMC_PCK_PRES_64	(0x6u << 4) /**< \brief (PMC_PCK[3]) Selected clock divided by 64 */

	/* -------- PMC_SCER : (PMC Offset: 0x0000) System Clock Enable Register -------- */

	#define PMC_UDP		(0x1u << 7)		/**< \brief (PMC_SCER) USB Device Port Clock Enable */
	#define PMC_PCK0	(0x1u << 8)		/**< \brief (PMC_SCER) Programmable Clock 0 Output Enable */
	#define PMC_PCK1	(0x1u << 9)		/**< \brief (PMC_SCER) Programmable Clock 1 Output Enable */
	#define PMC_PCK2	(0x1u << 10)	/**< \brief (PMC_SCER) Programmable Clock 2 Output Enable */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_PDC
	{
		AT91_PTR	RPR;	// Receive Pointer Register	
		AT91_REG	RCR;	// Receive Counter Register
		AT91_PTR	TPR;	// Transmit Pointer Register
		AT91_REG	TCR;	// Transmit Counter Register
		AT91_PTR	RNPR;	// Receive Next Pointer Register	
		AT91_REG	RNCR;	// Receive Next Pointer Register
		AT91_PTR	TNPR;	// Transmit Next Pointer Register
		AT91_REG	TNCR;	// Transmit Next Counter Register
		AT91_WO		PTCR;	// PDC Transfer Control Register	
		AT91_REG	PTSR;	// PDC Transfer Status Register
	};

	/* -------- PERIPH_PTCR : (PDC Offset: 0x20) Transfer Control Register -------- */

	#define PDC_RXTEN	(0x1u << 0) /**< \brief (PERIPH_PTCR) Receiver Transfer Enable */
	#define PDC_RXTDIS	(0x1u << 1) /**< \brief (PERIPH_PTCR) Receiver Transfer Disable */
	#define PDC_TXTEN	(0x1u << 8) /**< \brief (PERIPH_PTCR) Transmitter Transfer Enable */
	#define PDC_TXTDIS	(0x1u << 9) /**< \brief (PERIPH_PTCR) Transmitter Transfer Disable */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_PIO
	{
		AT91_WO	 PER;						/**< \brief (Pio Offset: 0x0000) PIO Enable Register */						
		AT91_WO	 PDR;						/**< \brief (Pio Offset: 0x0004) PIO Disable Register */
		AT91_RO	 PSR;						/**< \brief (Pio Offset: 0x0008) PIO Status Register */
		AT91_RO	 z__reserved1;				
		AT91_WO	 OER;						/**< \brief (Pio Offset: 0x0010) Output Enable Register */
		AT91_WO	 ODR;						/**< \brief (Pio Offset: 0x0014) Output Disable Register */
		AT91_RO	 OSR;						/**< \brief (Pio Offset: 0x0018) Output Status Register */
		AT91_RO	 z__reserved2;				
		AT91_WO	 IFER;						/**< \brief (Pio Offset: 0x0020) Glitch Input Filter Enable Register */
		AT91_WO	 IFDR;						/**< \brief (Pio Offset: 0x0024) Glitch Input Filter Disable Register */
		AT91_RO	 IFSR;						/**< \brief (Pio Offset: 0x0028) Glitch Input Filter Status Register */
		AT91_RO	 z__reserved3;				
		AT91_WO	 SODR;						/**< \brief (Pio Offset: 0x0030) Set Output Data Register */
		AT91_WO	 CODR;						/**< \brief (Pio Offset: 0x0034) Clear Output Data Register */
		AT91_REG ODSR;						/**< \brief (Pio Offset: 0x0038) Output Data Status Register */
		AT91_RO	 PDSR;						/**< \brief (Pio Offset: 0x003C) Pin Data Status Register */
		AT91_WO	 IER;						/**< \brief (Pio Offset: 0x0040) Interrupt Enable Register */
		AT91_WO	 IDR;						/**< \brief (Pio Offset: 0x0044) Interrupt Disable Register */
		AT91_RO	 IMR;						/**< \brief (Pio Offset: 0x0048) Interrupt Mask Register */
		AT91_RO	 ISR;						/**< \brief (Pio Offset: 0x004C) Interrupt Status Register */
		AT91_WO	 MDER;						/**< \brief (Pio Offset: 0x0050) Multi-driver Enable Register */
		AT91_WO	 MDDR;						/**< \brief (Pio Offset: 0x0054) Multi-driver Disable Register */
		AT91_RO	 MDSR;						/**< \brief (Pio Offset: 0x0058) Multi-driver Status Register */
		AT91_RO	 z__reserved4;				
		AT91_WO	 PUDR;						/**< \brief (Pio Offset: 0x0060) Pull-up Disable Register */
		AT91_WO	 PUER;						/**< \brief (Pio Offset: 0x0064) Pull-up Enable Register */
		AT91_RO	 PUSR;						/**< \brief (Pio Offset: 0x0068) Pad Pull-up Status Register */
		AT91_RO	 z__reserved5;				
		AT91_REG ABCDSR1;					/**< \brief (Pio Offset: 0x0070) Peripheral Select Register */
		AT91_REG ABCDSR2;					
		AT91_RO	 z__reserved6[2];						
		AT91_WO	 IFSCDR;					/**< \brief (Pio Offset: 0x0080) Input Filter Slow Clock Disable Register */		
		AT91_WO	 IFSCER;					/**< \brief (Pio Offset: 0x0084) Input Filter Slow Clock Enable Register */
		AT91_RO	 IFSCSR;					/**< \brief (Pio Offset: 0x0088) Input Filter Slow Clock Status Register */
		AT91_REG SCDR;						/**< \brief (Pio Offset: 0x008C) Slow Clock Divider Debouncing Register */
		AT91_WO	 PPDDR;						/**< \brief (Pio Offset: 0x0090) Pad Pull-down Disable Register */
		AT91_WO	 PPDER;						/**< \brief (Pio Offset: 0x0094) Pad Pull-down Enable Register */
		AT91_RO	 PPDSR;						/**< \brief (Pio Offset: 0x0098) Pad Pull-down Status Register */
		AT91_RO	 z__reserved7;				
		AT91_WO	 OWER;						/**< \brief (Pio Offset: 0x00A0) Output Write Enable */
		AT91_WO	 OWDR;						/**< \brief (Pio Offset: 0x00A4) Output Write Disable */
		AT91_RO	 OWSR;						/**< \brief (Pio Offset: 0x00A8) Output Write Status Register */
		AT91_RO	 z__reserved8;				
		AT91_WO	 AIMER;						/**< \brief (Pio Offset: 0x00B0) Additional Interrupt Modes Enable Register */
		AT91_WO	 AIMDR;						/**< \brief (Pio Offset: 0x00B4) Additional Interrupt Modes Disable Register */
		AT91_RO	 AIMMR;						/**< \brief (Pio Offset: 0x00B8) Additional Interrupt Modes Mask Register */
		AT91_RO	 z__reserved9;				
		AT91_WO	 ESR;						/**< \brief (Pio Offset: 0x00C0) Edge Select Register */
		AT91_WO	 LSR;						/**< \brief (Pio Offset: 0x00C4) Level Select Register */
		AT91_RO	 ELSR;						/**< \brief (Pio Offset: 0x00C8) Edge/Level Status Register */
		AT91_RO	 z__reserved10;				
		AT91_WO	 FELLSR;					/**< \brief (Pio Offset: 0x00D0) Falling Edge/Low-Level Select Register */
		AT91_WO	 REHLSR;					/**< \brief (Pio Offset: 0x00D4) Rising Edge/ High-Level Select Register */
		AT91_RO	 FRLHSR;					/**< \brief (Pio Offset: 0x00D8) Fall/Rise - Low/High Status Register */
		AT91_RO	 z__reserved11;				
		AT91_REG LOCKSR;					/**< \brief (Pio Offset: 0x00E0) Lock Status */
		AT91_REG WPMR;						/**< \brief (Pio Offset: 0x00E4) Write Protection Mode Register */
		AT91_RO	 WPSR;						/**< \brief (Pio Offset: 0x00E8) Write Protection Status Register */
		AT91_RO	 z__reserved12[5];			
		AT91_REG SCHMITT;					/**< \brief (Pio Offset: 0x0100) Schmitt Trigger Register */

		AT91_REG z__Reserved13[19];
		AT91_REG PCMR;						/**< \brief (Pio Offset: 0x150) Parallel Capture Mode Register */
		AT91_WO	 PCIER;						/**< \brief (Pio Offset: 0x154) Parallel Capture Interrupt Enable Register */
		AT91_WO	 PCIDR;						/**< \brief (Pio Offset: 0x158) Parallel Capture Interrupt Disable Register */
		AT91_RO	 PCIMR;						/**< \brief (Pio Offset: 0x15C) Parallel Capture Interrupt Mask Register */
		AT91_RO	 PCISR;						/**< \brief (Pio Offset: 0x160) Parallel Capture Interrupt Status Register */
		AT91_RO	 PCRHR;						/**< \brief (Pio Offset: 0x164) Parallel Capture Reception Holding Register */
		
		S_PDC PDC;

		__forceinline void 	SET(u32 m) 			{ SODR = m; }
		__forceinline void 	CLR(u32 m) 			{ CODR = m; }
		__forceinline void 	NOT(u32 m) 			{ /*OUTTGL = m;*/ }
		__forceinline void 	WBIT(u32 m, bool c) { if (c) SET(m); else CLR(m); }
		__forceinline void 	BSET(u16 b) 		{ SODR = 1 << b; }
		__forceinline void 	BCLR(u16 b) 		{ CODR = 1 << b; }
		__forceinline void 	BTGL(u16 b) 		{ /*OUTTGL = 1 << b;*/ }

		//__forceinline void		OUT8(byte v)		{ *((byte*)&OUT) = v; }

		__forceinline bool 	TBSET(u16 b) 		{ return PDSR & (1<<b); }
		__forceinline bool 	TBCLR(u16 b) 		{ return (PDSR & (1<<b)) == 0; }

	};										
											
	typedef S_PIO S_PIOA, S_PIOB, S_PIOC, S_PORT;	
			
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
	#define PA00 	(1UL<<0)
	#define PA01 	(1UL<<1)
	#define PA02 	(1UL<<2)
	#define PA03 	(1UL<<3)
	#define PA04 	(1UL<<4)
	#define PA05 	(1UL<<5)
	#define PA06 	(1UL<<6)
	#define PA07 	(1UL<<7)
	#define PA08 	(1UL<<8)
	#define PA09 	(1UL<<9)
	#define PA10 	(1UL<<10)
	#define PA11 	(1UL<<11)
	#define PA12 	(1UL<<12)
	#define PA13 	(1UL<<13)
	#define PA14 	(1UL<<14)
	#define PA15 	(1UL<<15)
	#define PA16 	(1UL<<16)
	#define PA17 	(1UL<<17)
	#define PA18 	(1UL<<18)
	#define PA19 	(1UL<<19)
	#define PA20 	(1UL<<20)
	#define PA21 	(1UL<<21)
	#define PA22 	(1UL<<22)
	#define PA23 	(1UL<<23)
	#define PA24 	(1UL<<24)
	#define PA25 	(1UL<<25)
	#define PA26 	(1UL<<26)
	#define PA27 	(1UL<<27)
	#define PA28 	(1UL<<28)
	#define PA29 	(1UL<<29)
	#define PA30 	(1UL<<30)
	#define PA31 	(1UL<<31)

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
	#define PB00 	(1UL<<0)
	#define PB01 	(1UL<<1)
	#define PB02 	(1UL<<2)
	#define PB03 	(1UL<<3)
	#define PB04 	(1UL<<4)
	#define PB05 	(1UL<<5)
	#define PB06 	(1UL<<6)
	#define PB07 	(1UL<<7)
	#define PB08 	(1UL<<8)
	#define PB09 	(1UL<<9)
	#define PB10 	(1UL<<10)
	#define PB11 	(1UL<<11)
	#define PB12 	(1UL<<12)
	#define PB13 	(1UL<<13)
	#define PB14 	(1UL<<14)
	#define PB15 	(1UL<<15)
	#define PB16 	(1UL<<16)
	#define PB17 	(1UL<<17)
	#define PB18 	(1UL<<18)
	#define PB19 	(1UL<<19)
	#define PB20 	(1UL<<20)
	#define PB21 	(1UL<<21)
	#define PB22 	(1UL<<22)
	#define PB23 	(1UL<<23)
	#define PB24 	(1UL<<24)
	#define PB25 	(1UL<<25)
	#define PB26 	(1UL<<26)
	#define PB27 	(1UL<<27)
	#define PB28 	(1UL<<28)
	#define PB29 	(1UL<<29)
	#define PB30 	(1UL<<30)
	#define PB31 	(1UL<<31)

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
	#define PC00 	(1UL<<0)
	#define PC01 	(1UL<<1)
	#define PC02 	(1UL<<2)
	#define PC03 	(1UL<<3)
	#define PC04 	(1UL<<4)
	#define PC05 	(1UL<<5)
	#define PC06 	(1UL<<6)
	#define PC07 	(1UL<<7)
	#define PC08 	(1UL<<8)
	#define PC09 	(1UL<<9)
	#define PC10 	(1UL<<10)
	#define PC11 	(1UL<<11)
	#define PC12 	(1UL<<12)
	#define PC13 	(1UL<<13)
	#define PC14 	(1UL<<14)
	#define PC15 	(1UL<<15)
	#define PC16 	(1UL<<16)
	#define PC17 	(1UL<<17)
	#define PC18 	(1UL<<18)
	#define PC19 	(1UL<<19)
	#define PC20 	(1UL<<20)
	#define PC21 	(1UL<<21)
	#define PC22 	(1UL<<22)
	#define PC23 	(1UL<<23)
	#define PC24 	(1UL<<24)
	#define PC25 	(1UL<<25)
	#define PC26 	(1UL<<26)
	#define PC27 	(1UL<<27)
	#define PC28 	(1UL<<28)
	#define PC29 	(1UL<<29)
	#define PC30 	(1UL<<30)
	#define PC31 	(1UL<<31)

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_MC
	{
		AT91_REG RCR;	
		AT91_REG ASR;	
		AT91_REG AASR;	
		AT91_REG _RESERVED;	
		AT91_REG PUIA0;	
		AT91_REG PUIA1;	
		AT91_REG PUIA2;	
		AT91_REG PUIA3;	
		AT91_REG PUIA4;	
		AT91_REG PUIA5;	
		AT91_REG PUIA6;	
		AT91_REG PUIA7;	
		AT91_REG PUIA8;	
		AT91_REG PUIA9;	
		AT91_REG PUIA10;	
		AT91_REG PUIA11;	
		AT91_REG PUIA12;	
		AT91_REG PUIA13;	
		AT91_REG PUIA14;	
		AT91_REG PUIA15;	
		AT91_REG PUP;	
		AT91_REG PUER;	
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_TC
	{
		struct 
		{
			AT91_WO	 CCR;
			AT91_REG CMR;
			AT91_REG SMMR;
			AT91_RO	 z__reserved1;
			AT91_RO	 CV;
			AT91_REG RA;
			AT91_REG RB;
			AT91_REG RC;
			AT91_RO	 SR;
			AT91_REG IER;
			AT91_REG IDR;
			AT91_RO	 IMR;
			AT91_REG z__reserved2[4];
		} C0, C1, C2;

		AT91_WO	 BCR;
		AT91_REG BMR;

		AT91_WO	 QIER;
		AT91_WO	 QIDR;
		AT91_RO	 QIMR;
		AT91_RO	 QISR;
		AT91_REG FMR;
		AT91_REG WPMR;
	};

	typedef S_TC S_TC0, S_TC1;

	/* -------- TC_CCR : (TC Offset: N/A) Channel Control Register -------- */

	#define TC_CLKEN	(0x1u << 0)		/**< \brief (TC_CCR) Counter Clock Enable Command */
	#define TC_CLKDIS	(0x1u << 1)		/**< \brief (TC_CCR) Counter Clock Disable Command */
	#define TC_SWTRG	(0x1u << 2)		/**< \brief (TC_CCR) Software Trigger Command */

	/* -------- TC_CMR : (TC Offset: N/A) Channel Mode Register -------- */

	#define	TC_TCCLKS_MCK2		(0x0u << 0)		/**< \brief (TC_CMR) Clock selected: internal TIMER_CLOCK1 clock signal (from PMC) */
	#define	TC_TCCLKS_MCK8		(0x1u << 0)		/**< \brief (TC_CMR) Clock selected: internal TIMER_CLOCK2 clock signal (from PMC) */
	#define	TC_TCCLKS_MCK32		(0x2u << 0)		/**< \brief (TC_CMR) Clock selected: internal TIMER_CLOCK3 clock signal (from PMC) */
	#define	TC_TCCLKS_MCK128	(0x3u << 0)		/**< \brief (TC_CMR) Clock selected: internal TIMER_CLOCK4 clock signal (from PMC) */
	#define	TC_TCCLKS_SLCK		(0x4u << 0)		/**< \brief (TC_CMR) Clock selected: internal TIMER_CLOCK5 clock signal (from PMC) */
	#define	TC_TCCLKS_XC0		(0x5u << 0)		/**< \brief (TC_CMR) Clock selected: XC0 */
	#define	TC_TCCLKS_XC1		(0x6u << 0)		/**< \brief (TC_CMR) Clock selected: XC1 */
	#define	TC_TCCLKS_XC2		(0x7u << 0)		/**< \brief (TC_CMR) Clock selected: XC2 */

	#define TC_CLKI				(0x1u << 3)		/**< \brief (TC_CMR) Clock Invert */
	#define	TC_BURST_NONE		(0x0u << 4)		/**< \brief (TC_CMR) The clock is not gated by an external signal. */
	#define	TC_BURST_XC0		(0x1u << 4)		/**< \brief (TC_CMR) XC0 is ANDed with the selected clock. */
	#define	TC_BURST_XC1		(0x2u << 4)		/**< \brief (TC_CMR) XC1 is ANDed with the selected clock. */
	#define	TC_BURST_XC2		(0x3u << 4)		/**< \brief (TC_CMR) XC2 is ANDed with the selected clock. */
	#define TC_LDBSTOP			(0x1u << 6)		/**< \brief (TC_CMR) Counter Clock Stopped with RB Loading */
	#define TC_LDBDIS			(0x1u << 7)		/**< \brief (TC_CMR) Counter Clock Disable with RB Loading */
	#define	TC_ETRGEDG_NONE		(0x0u << 8)		/**< \brief (TC_CMR) The clock is not gated by an external signal. */
	#define	TC_ETRGEDG_RISING	(0x1u << 8)		/**< \brief (TC_CMR) Rising edge */
	#define	TC_ETRGEDG_FALLING	(0x2u << 8)		/**< \brief (TC_CMR) Falling edge */
	#define	TC_ETRGEDG_EDGE		(0x3u << 8)		/**< \brief (TC_CMR) Each edge */
	#define TC_ABETRG			(0x1u << 10)	/**< \brief (TC_CMR) TIOA or TIOB External Trigger Selection */
	#define TC_CPCTRG			(0x1u << 14)	/**< \brief (TC_CMR) RC Compare Trigger Enable */
	#define TC_WAVE				(0x1u << 15)	/**< \brief (TC_CMR) Waveform Mode */
	#define	TC_LDRA_NONE		(0x0u << 16)	/**< \brief (TC_CMR) None */
	#define	TC_LDRA_RISING		(0x1u << 16)	/**< \brief (TC_CMR) Rising edge of TIOA */
	#define	TC_LDRA_FALLING		(0x2u << 16)	/**< \brief (TC_CMR) Falling edge of TIOA */
	#define	TC_LDRA_EDGE		(0x3u << 16)	/**< \brief (TC_CMR) Each edge of TIOA */
	#define	TC_LDRB_NONE		(0x0u << 18)	/**< \brief (TC_CMR) None */
	#define	TC_LDRB_RISING		(0x1u << 18)	/**< \brief (TC_CMR) Rising edge of TIOA */
	#define	TC_LDRB_FALLING		(0x2u << 18)	/**< \brief (TC_CMR) Falling edge of TIOA */
	#define	TC_LDRB_EDGE		(0x3u << 18)	/**< \brief (TC_CMR) Each edge of TIOA */

	#define TC_CPCSTOP			(0x1u << 6)		/**< \brief (TC_CMR) Counter Clock Stopped with RC Compare */
	#define TC_CPCDIS			(0x1u << 7)		/**< \brief (TC_CMR) Counter Clock Disable with RC Compare */
	#define	TC_EEVTEDG_NONE		(0x0u << 8)		/**< \brief (TC_CMR) None */
	#define	TC_EEVTEDG_RISING	(0x1u << 8)		/**< \brief (TC_CMR) Rising edge */
	#define	TC_EEVTEDG_FALLING	(0x2u << 8)		/**< \brief (TC_CMR) Falling edge */
	#define	TC_EEVTEDG_EDGE		(0x3u << 8)		/**< \brief (TC_CMR) Each edge */
	#define	TC_EEVT_TIOB		(0x0u << 10)	/**< \brief (TC_CMR) TIOB */
	#define	TC_EEVT_XC0			(0x1u << 10)	/**< \brief (TC_CMR) XC0 */
	#define	TC_EEVT_XC1			(0x2u << 10)	/**< \brief (TC_CMR) XC1 */
	#define	TC_EEVT_XC2			(0x3u << 10)	/**< \brief (TC_CMR) XC2 */
	#define TC_ENETRG			0x1u << 12)		/**< \brief (TC_CMR) External Event Trigger Enable */
	#define	TC_WAVSEL_UP		(0x0u << 13)	/**< \brief (TC_CMR) UP mode without automatic trigger on RC Compare */
	#define	TC_WAVSEL_UPDOWN	(0x1u << 13)	/**< \brief (TC_CMR) UPDOWN mode without automatic trigger on RC Compare */
	#define	TC_WAVSEL_UP_RC		(0x2u << 13)	/**< \brief (TC_CMR) UP mode with automatic trigger on RC Compare */
	#define	TC_WAVSEL_UPDOWN_RC (0x3u << 13)	/**< \brief (TC_CMR) UPDOWN mode with automatic trigger on RC Compare */
	#define	TC_ACPA_NONE		(0x0u << 16)	/**< \brief (TC_CMR) None */
	#define	TC_ACPA_SET			(0x1u << 16)	/**< \brief (TC_CMR) Set */
	#define	TC_ACPA_CLEAR		(0x2u << 16)	/**< \brief (TC_CMR) Clear */
	#define	TC_ACPA_TOGGLE		(0x3u << 16)	/**< \brief (TC_CMR) Toggle */
	#define	TC_ACPC_NONE		(0x0u << 18)	/**< \brief (TC_CMR) None */
	#define	TC_ACPC_SET			(0x1u << 18)	/**< \brief (TC_CMR) Set */
	#define	TC_ACPC_CLEAR		(0x2u << 18)	/**< \brief (TC_CMR) Clear */
	#define	TC_ACPC_TOGGLE		(0x3u << 18)	/**< \brief (TC_CMR) Toggle */
	#define	TC_AEEVT_NONE		(0x0u << 20)	/**< \brief (TC_CMR) None */
	#define	TC_AEEVT_SET		(0x1u << 20)	/**< \brief (TC_CMR) Set */
	#define	TC_AEEVT_CLEAR		(0x2u << 20)	/**< \brief (TC_CMR) Clear */
	#define	TC_AEEVT_TOGGLE		(0x3u << 20)	/**< \brief (TC_CMR) Toggle */
	#define	TC_ASWTRG_NONE		(0x0u << 22)	/**< \brief (TC_CMR) None */
	#define	TC_ASWTRG_SET		(0x1u << 22)	/**< \brief (TC_CMR) Set */
	#define	TC_ASWTRG_CLEAR		(0x2u << 22)	/**< \brief (TC_CMR) Clear */
	#define	TC_ASWTRG_TOGGLE	(0x3u << 22)	/**< \brief (TC_CMR) Toggle */
	#define	TC_BCPB_NONE		(0x0u << 24)	/**< \brief (TC_CMR) None */
	#define	TC_BCPB_SET			(0x1u << 24)	/**< \brief (TC_CMR) Set */
	#define	TC_BCPB_CLEAR		(0x2u << 24)	/**< \brief (TC_CMR) Clear */
	#define	TC_BCPB_TOGGLE		(0x3u << 24)	/**< \brief (TC_CMR) Toggle */
	#define	TC_BCPC_NONE		(0x0u << 26)	/**< \brief (TC_CMR) None */
	#define	TC_BCPC_SET			(0x1u << 26)	/**< \brief (TC_CMR) Set */
	#define	TC_BCPC_CLEAR		(0x2u << 26)	/**< \brief (TC_CMR) Clear */
	#define	TC_BCPC_TOGGLE		(0x3u << 26)	/**< \brief (TC_CMR) Toggle */
	#define	TC_BEEVT_NONE		(0x0u << 28)	/**< \brief (TC_CMR) None */
	#define	TC_BEEVT_SET		(0x1u << 28)	/**< \brief (TC_CMR) Set */
	#define	TC_BEEVT_CLEAR		(0x2u << 28)	/**< \brief (TC_CMR) Clear */
	#define	TC_BEEVT_TOGGLE		(0x3u << 28)	/**< \brief (TC_CMR) Toggle */
	#define	TC_BSWTRG_NONE		(0x0u << 30)	/**< \brief (TC_CMR) None */
	#define	TC_BSWTRG_SET		(0x1u << 30)	/**< \brief (TC_CMR) Set */
	#define	TC_BSWTRG_CLEAR		(0x2u << 30)	/**< \brief (TC_CMR) Clear */
	#define	TC_BSWTRG_TOGGLE	(0x3u << 30)	/**< \brief (TC_CMR) Toggle */

	/* -------- TC_SMMR : (TC Offset: N/A) Stepper Motor Mode Register -------- */

	#define TC_GCEN (0x1u << 0) /**< \brief (TC_SMMR) Gray Count Enable */
	#define TC_DOWN (0x1u << 1) /**< \brief (TC_SMMR) DOWN Count */

	/* -------- TC_SR : (TC Offset: N/A) Status Register -------- */

	#define TC_COVFS	(0x1u << 0)		/**< \brief (TC_SR) Counter Overflow Status */
	#define TC_LOVRS	(0x1u << 1)		/**< \brief (TC_SR) Load Overrun Status */
	#define TC_CPAS		(0x1u << 2)		/**< \brief (TC_SR) RA Compare Status */
	#define TC_CPBS		(0x1u << 3)		/**< \brief (TC_SR) RB Compare Status */
	#define TC_CPCS		(0x1u << 4)		/**< \brief (TC_SR) RC Compare Status */
	#define TC_LDRAS	(0x1u << 5)		/**< \brief (TC_SR) RA Loading Status */
	#define TC_LDRBS	(0x1u << 6)		/**< \brief (TC_SR) RB Loading Status */
	#define TC_ETRGS	(0x1u << 7)		/**< \brief (TC_SR) External Trigger Status */
	#define TC_CLKSTA	(0x1u << 16)	/**< \brief (TC_SR) Clock Enabling Status */
	#define TC_MTIOA	(0x1u << 17)	/**< \brief (TC_SR) TIOA Mirror */
	#define TC_MTIOB	(0x1u << 18)	/**< \brief (TC_SR) TIOB Mirror */

	/* -------- TC_BCR : (TC Offset: 0xC0) Block Control Register -------- */

	#define TC_SYNC (0x1u << 0) /**< \brief (TC_BCR) Synchro Command */
	
	/* -------- TC_BMR : (TC Offset: 0xC4) Block Mode Register -------- */

	#define	TC_TC0XC0S_TCLK0	(0x0u << 0)				/**< \brief (TC_BMR) Signal connected to XC0: TCLK0 */
	#define	TC_TC0XC0S_TIOA1	(0x2u << 0)				/**< \brief (TC_BMR) Signal connected to XC0: TIOA1 */
	#define	TC_TC0XC0S_TIOA2	(0x3u << 0)				/**< \brief (TC_BMR) Signal connected to XC0: TIOA2 */
	#define	TC_TC1XC1S_TCLK1	(0x0u << 2)				/**< \brief (TC_BMR) Signal connected to XC1: TCLK1 */
	#define	TC_TC1XC1S_TIOA0	(0x2u << 2)				/**< \brief (TC_BMR) Signal connected to XC1: TIOA0 */
	#define	TC_TC1XC1S_TIOA2	(0x3u << 2)				/**< \brief (TC_BMR) Signal connected to XC1: TIOA2 */
	#define	TC_TC2XC2S_TCLK2	(0x0u << 4)				/**< \brief (TC_BMR) Signal connected to XC2: TCLK2 */
	#define	TC_TC2XC2S_TIOA0	(0x2u << 4)				/**< \brief (TC_BMR) Signal connected to XC2: TIOA0 */
	#define	TC_TC2XC2S_TIOA1	(0x3u << 4)				/**< \brief (TC_BMR) Signal connected to XC2: TIOA1 */
	#define TC_QDEN				(0x1u << 8)				/**< \brief (TC_BMR) Quadrature Decoder ENabled */
	#define TC_POSEN			(0x1u << 9)				/**< \brief (TC_BMR) POSition ENabled */
	#define TC_SPEEDEN			(0x1u << 10)			/**< \brief (TC_BMR) SPEED ENabled */
	#define TC_QDTRANS			(0x1u << 11)			/**< \brief (TC_BMR) Quadrature Decoding TRANSparent */
	#define TC_EDGPHA			(0x1u << 12)			/**< \brief (TC_BMR) EDGe on PHA count mode */
	#define TC_INVA				(0x1u << 13)			/**< \brief (TC_BMR) INVerted phA */
	#define TC_INVB				(0x1u << 14)			/**< \brief (TC_BMR) INVerted phB */
	#define TC_INVIDX			(0x1u << 15)			/**< \brief (TC_BMR) INVerted InDeX */
	#define TC_SWAP				(0x1u << 16)			/**< \brief (TC_BMR) SWAP PHA and PHB */
	#define TC_IDXPHB			(0x1u << 17)			/**< \brief (TC_BMR) InDeX pin is PHB pin */
	#define TC_FILTER			(0x1u << 19)			/**< \brief (TC_BMR) Glitch Filter */
	#define TC_MAXFILT(value)	(((value)&0x3F) << 20)	/**< \brief (TC_BMR) MAXimum FILTer */

	/* -------- TC_QISR : (TC Offset: 0xD4) QDEC Interrupt Status Register -------- */

	#define TC_IDX		(0x1u << 0) /**< \brief (TC_QISR) InDeX */
	#define TC_DIRCHG	(0x1u << 1) /**< \brief (TC_QISR) DIRection CHanGe */
	#define TC_QERR		(0x1u << 2) /**< \brief (TC_QISR) Quadrature ERRor */
	#define TC_DIR		(0x1u << 8) /**< \brief (TC_QISR) DIRection */
	
	/* -------- TC_FMR : (TC Offset: 0xD8) Fault Mode Register -------- */

	#define TC_ENCF0	(0x1u << 0) /**< \brief (TC_FMR) ENable Compare Fault Channel 0 */
	#define TC_ENCF1	(0x1u << 1) /**< \brief (TC_FMR) ENable Compare Fault Channel 1 */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_UDP	// 0xFFFB0000
	{
		AT91_REG FRM_NUM;
		AT91_REG GLB_STAT;
		AT91_REG FADDR;
		AT91_REG z__reserved1;
		AT91_REG IER;
		AT91_REG IDR;
		AT91_REG IMR;
		AT91_REG ISR;
		AT91_REG ICR;
		AT91_REG z__reserved2;
		AT91_REG RST_EP;
		AT91_REG z__reserved3;
		AT91_REG CSR[8];
		AT91_REG FDR[8];
		AT91_REG z__reserved4;
		AT91_REG TXVC;
	};


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_USART
	{
		AT91_WO	 CR;				// Control Register 	
		AT91_REG MR;				// Mode Register
		AT91_WO	 IER;				// Interrupt Enable Register
		AT91_WO	 IDR;				// Interrupt Disable Register
		AT91_REG IMR;				// Interrupt Mask Register
		AT91_REG CSR;				// Channel Status Register
		AT91_REG RHR;				// Receiver Holding Register
		AT91_WO	 THR;				// Transmitter Holding Register	
		AT91_REG BRGR;				// Baud Rate Generator Register
		AT91_REG RTOR;				// Receiver Time-out Register
		AT91_REG TTGR;				// Transmitter Timeguard Register
		AT91_REG z__reserved1[5];	
		AT91_REG FIDI;				// FI DI Ratio Register
		AT91_REG NER;				// Number of Errors Register
		AT91_REG z__reserved2;	
		AT91_REG IF;				// IrDA Filter Register
		AT91_REG MAN;				// Manchester Encode Decode Register
		AT91_REG z__reserved3[43];	

		S_PDC PDC;
	};

	typedef S_USART S_USART0, S_USART1;

	/* -------- US_CR : (USART Offset: 0x0000) Control Register -------- */

	#define US_RSTRX	(0x1u << 2)		/**< \brief (US_CR) Reset Receiver */
	#define US_RSTTX	(0x1u << 3)		/**< \brief (US_CR) Reset Transmitter */
	#define US_RXEN		(0x1u << 4)		/**< \brief (US_CR) Receiver Enable */
	#define US_RXDIS	(0x1u << 5)		/**< \brief (US_CR) Receiver Disable */
	#define US_TXEN		(0x1u << 6)		/**< \brief (US_CR) Transmitter Enable */
	#define US_TXDIS	(0x1u << 7)		/**< \brief (US_CR) Transmitter Disable */
	#define US_RSTSTA	(0x1u << 8)		/**< \brief (US_CR) Reset Status Bits */
	#define US_STTBRK	(0x1u << 9)		/**< \brief (US_CR) Start Break */
	#define US_STPBRK	(0x1u << 10)	/**< \brief (US_CR) Stop Break */
	#define US_STTTO	(0x1u << 11)	/**< \brief (US_CR) Start Time-out */
	#define US_SENDA	(0x1u << 12)	/**< \brief (US_CR) Send Address */
	#define US_RSTIT	(0x1u << 13)	/**< \brief (US_CR) Reset Iterations */
	#define US_RSTNACK	(0x1u << 14)	/**< \brief (US_CR) Reset Non Acknowledge */
	#define US_RETTO	(0x1u << 15)	/**< \brief (US_CR) Rearm Time-out */
	#define US_DTREN	(0x1u << 16)	/**< \brief (US_CR) Data Terminal Ready Enable */
	#define US_DTRDIS	(0x1u << 17)	/**< \brief (US_CR) Data Terminal Ready Disable */
	#define US_RTSEN	(0x1u << 18)	/**< \brief (US_CR) Request to Send Enable */
	#define US_RTSDIS	(0x1u << 19)	/**< \brief (US_CR) Request to Send Disable */
	#define US_FCS		(0x1u << 18)	/**< \brief (US_CR) Force SPI Chip Select */
	#define US_RCS		(0x1u << 19)	/**< \brief (US_CR) Release SPI Chip Select */
	
	/* -------- US_MR : (USART Offset: 0x0004) Mode Register -------- */

	#define	US_MODE_NORMAL				(0x0u << 0)		/**< \brief (US_MR) Normal mode */
	#define	US_MODE_RS485				(0x1u << 0)		/**< \brief (US_MR) RS485 */
	#define	US_MODE_HW_HANDSHAKING		(0x2u << 0)		/**< \brief (US_MR) Hardware Handshaking */
	#define	US_MODE_MODEM				(0x3u << 0)		/**< \brief (US_MR) Modem */
	#define	US_MODE_IS07816_T_0			(0x4u << 0)		/**< \brief (US_MR) IS07816 Protocol: T = 0 */
	#define	US_MODE_IS07816_T_1			(0x6u << 0)		/**< \brief (US_MR) IS07816 Protocol: T = 1 */
	#define	US_MODE_IRDA				(0x8u << 0)		/**< \brief (US_MR) IrDA */
	#define	US_MODE_SPI_MASTER			(0xEu << 0)		/**< \brief (US_MR) SPI master */
	#define	US_MODE_SPI_SLAVE			(0xFu << 0)		/**< \brief (US_MR) SPI Slave */

	#define	US_USCLKS_MCK				(0x0u << 4)		/**< \brief (US_MR) master Clock MCK is selected */
	#define	US_USCLKS_DIV				(0x1u << 4)		/**< \brief (US_MR) Internal Clock Divided MCK/DIV (DIV=8) is selected */
	#define	US_USCLKS_SCK				(0x3u << 4)		/**< \brief (US_MR) Serial Clock SLK is selected */

	#define	US_CHRL_5_BIT				(0x0u << 6)		/**< \brief (US_MR) Character length is 5 bits */
	#define	US_CHRL_6_BIT				(0x1u << 6)		/**< \brief (US_MR) Character length is 6 bits */
	#define	US_CHRL_7_BIT				(0x2u << 6)		/**< \brief (US_MR) Character length is 7 bits */
	#define	US_CHRL_8_BIT				(0x3u << 6)		/**< \brief (US_MR) Character length is 8 bits */
	#define	US_SYNC						(0x1u << 8)		/**< \brief (US_MR) Synchronous Mode Select */

	#define	US_PAR_EVEN					(0x0u << 9)		/**< \brief (US_MR) Even parity */
	#define	US_PAR_ODD					(0x1u << 9)		/**< \brief (US_MR) Odd parity */
	#define	US_PAR_SPACE				(0x2u << 9)		/**< \brief (US_MR) Parity forced to 0 (Space) */
	#define	US_PAR_MARK					(0x3u << 9)		/**< \brief (US_MR) Parity forced to 1 (Mark) */
	#define	US_PAR_NO					(0x4u << 9)		/**< \brief (US_MR) No parity */
	#define	US_PAR_MULTIDROP			(0x6u << 9)		/**< \brief (US_MR) Multidrop mode */
	#define	US_NBSTOP_1_BIT				(0x0u << 12)	/**< \brief (US_MR) 1 stop bit */
	#define	US_NBSTOP_1_5_BIT			(0x1u << 12)	/**< \brief (US_MR) 1.5 stop bit (SYNC = 0) or reserved (SYNC = 1) */
	#define	US_NBSTOP_2_BIT				(0x2u << 12)	/**< \brief (US_MR) 2 stop bits */
	#define	US_CHMODE_NORMAL			(0x0u << 14)	/**< \brief (US_MR) Normal mode */
	#define	US_CHMODE_AUTOMATIC			(0x1u << 14)	/**< \brief (US_MR) Automatic Echo. Receiver input is connected to the TXD pin. */
	#define	US_CHMODE_LOCAL_LOOPBACK	(0x2u << 14)	/**< \brief (US_MR) Local Loopback. Transmitter output is connected to the Receiver Input. */
	#define	US_CHMODE_REMOTE_LOOPBACK	(0x3u << 14)	/**< \brief (US_MR) Remote Loopback. RXD pin is internally connected to the TXD pin. */

	#define US_MSBF						(0x1u << 16)	/**< \brief (US_MR) Bit Order */
	#define US_MODE9					(0x1u << 17)	/**< \brief (US_MR) 9-bit Character Length */
	#define US_CLKO						(0x1u << 18)	/**< \brief (US_MR) Clock Output Select */
	#define US_OVER						(0x1u << 19)	/**< \brief (US_MR) Oversampling Mode */
	#define US_INACK					(0x1u << 20)	/**< \brief (US_MR) Inhibit Non Acknowledge */
	#define US_DSNACK					(0x1u << 21)	/**< \brief (US_MR) Disable Successive NACK */
	#define US_VAR_SYNC					(0x1u << 22)	/**< \brief (US_MR) Variable Synchronization of Command/Data Sync Start Frame Delimiter */
	#define US_INVDATA					(0x1u << 23)	/**< \brief (US_MR) Inverted Data */
	#define US_MAX_ITERATION(value) (((value)&7) << 24)	/**< \brief (US_MR) Maximum Number of Automatic Iteration */

	#define US_FILTER					(0x1u << 28)	/**< \brief (US_MR) Infrared Receive Line Filter */
	#define US_MAN						(0x1u << 29)	/**< \brief (US_MR) Manchester Encoder/Decoder Enable */
	#define US_MODSYNC					(0x1u << 30)	/**< \brief (US_MR) Manchester Synchronization Mode */
	#define US_ONEBIT					(0x1u << 31)	/**< \brief (US_MR) Start Frame Delimiter Selector */
	#define US_CPHA						(0x1u << 8)		/**< \brief (US_MR) SPI Clock Phase */
	#define US_CPOL						(0x1u << 16)	/**< \brief (US_MR) SPI Clock Polarity */
	#define US_WRDBT					(0x1u << 20)	/**< \brief (US_MR) Wait Read Data Before Transfer */

	/* -------- US_IER : (USART Offset: 0x0008) Interrupt Enable Register -------- */

	#define US_RXRDY					(0x1u << 0)		/**< \brief (US_IER) RXRDY Interrupt Enable */
	#define US_TXRDY					(0x1u << 1)		/**< \brief (US_IER) TXRDY Interrupt Enable */
	#define US_RXBRK					(0x1u << 2)		/**< \brief (US_IER) Receiver Break Interrupt Enable */
	#define US_ENDRX					(0x1u << 3)		/**< \brief (US_IER) End of Receive Transfer Interrupt Enable (available in all USART modes of operation) */
	#define US_ENDTX					(0x1u << 4)		/**< \brief (US_IER) End of Transmit Interrupt Enable (available in all USART modes of operation) */
	#define US_OVRE						(0x1u << 5)		/**< \brief (US_IER) Overrun Error Interrupt Enable */
	#define US_FRAME					(0x1u << 6)		/**< \brief (US_IER) Framing Error Interrupt Enable */
	#define US_PARE						(0x1u << 7)		/**< \brief (US_IER) Parity Error Interrupt Enable */
	#define US_TIMEOUT					(0x1u << 8)		/**< \brief (US_IER) Time-out Interrupt Enable */
	#define US_TXEMPTY					(0x1u << 9)		/**< \brief (US_IER) TXEMPTY Interrupt Enable */
	#define US_ITER						(0x1u << 10)	/**< \brief (US_IER) Max number of Repetitions Reached Interrupt Enable */
	#define US_TXBUFE					(0x1u << 11)	/**< \brief (US_IER) Buffer Empty Interrupt Enable (available in all USART modes of operation) */
	#define US_RXBUFF					(0x1u << 12)	/**< \brief (US_IER) Buffer Full Interrupt Enable (available in all USART modes of operation) */
	#define US_NACK						(0x1u << 13)	/**< \brief (US_IER) Non AcknowledgeInterrupt Enable */
	#define US_RIIC						(0x1u << 16)	/**< \brief (US_IER) Ring Indicator Input Change Enable */
	#define US_DSRIC					(0x1u << 17)	/**< \brief (US_IER) Data Set Ready Input Change Enable */
	#define US_DCDIC					(0x1u << 18)	/**< \brief (US_IER) Data Carrier Detect Input Change Interrupt Enable */
	#define US_CTSIC					(0x1u << 19)	/**< \brief (US_IER) Clear to Send Input Change Interrupt Enable */
	#define US_MANE						(0x1u << 24)	/**< \brief (US_IER) Manchester Error Interrupt Enable */
	#define US_UNRE						(0x1u << 10)	/**< \brief (US_IER) SPI Underrun Error Interrupt Enable */


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_UART
	{
		AT91_WO	 CR;	
		AT91_REG MR;	
		AT91_WO	 IER;	
		AT91_WO	 IDR;	
		AT91_REG IMR;	
		AT91_REG SR;	
		AT91_REG RHR;	
		AT91_WO	 THR;	
		AT91_REG BRGR;	
		AT91_REG z__reserved[55];	

		S_PDC PDC;
	};

	typedef S_UART S_UART0, S_UART1;

	/* -------- UART_CR : (UART Offset: 0x0000) Control Register -------- */

	#define UART_RSTRX	(0x1u << 2)		/**< \brief (UART_CR) Reset Receiver */
	#define UART_RSTTX	(0x1u << 3)		/**< \brief (UART_CR) Reset Transmitter */
	#define UART_RXEN	(0x1u << 4)		/**< \brief (UART_CR) Receiver Enable */
	#define UART_RXDIS	(0x1u << 5)		/**< \brief (UART_CR) Receiver Disable */
	#define UART_TXEN	(0x1u << 6)		/**< \brief (UART_CR) Transmitter Enable */
	#define UART_TXDIS	(0x1u << 7)		/**< \brief (UART_CR) Transmitter Disable */
	#define UART_RSTSTA (0x1u << 8)		/**< \brief (UART_CR) Reset Status Bits */

	/* -------- UART_MR : (UART Offset: 0x0004) Mode Register -------- */

	#define   UART_PAR_EVEN					(0x0u << 9)		/**< \brief (UART_MR) Even Parity */
	#define   UART_PAR_ODD					(0x1u << 9)		/**< \brief (UART_MR) Odd Parity */
	#define   UART_PAR_SPACE				(0x2u << 9)		/**< \brief (UART_MR) Space: parity forced to 0 */
	#define   UART_PAR_MARK					(0x3u << 9)		/**< \brief (UART_MR) Mark: parity forced to 1 */
	#define   UART_PAR_NO					(0x4u << 9)		/**< \brief (UART_MR) No parity */
	#define   UART_CHMODE_NORMAL			(0x0u << 14)	/**< \brief (UART_MR) Normal mode */
	#define   UART_CHMODE_AUTOMATIC			(0x1u << 14)	/**< \brief (UART_MR) Automatic echo */
	#define   UART_CHMODE_LOCAL_LOOPBACK	(0x2u << 14)	/**< \brief (UART_MR) Local loopback */
	#define   UART_CHMODE_REMOTE_LOOPBACK	(0x3u << 14)	/**< \brief (UART_MR) Remote loopback */

	/* -------- UART_IER : (UART Offset: 0x0008) Interrupt Enable Register -------- */

	#define UART_RXRDY		(0x1u << 0) /**< \brief (UART_IER) Enable RXRDY Interrupt */
	#define UART_TXRDY		(0x1u << 1) /**< \brief (UART_IER) Enable TXRDY Interrupt */
	#define UART_ENDRX		(0x1u << 3) /**< \brief (UART_IER) Enable End of Receive Transfer Interrupt */
	#define UART_ENDTX		(0x1u << 4) /**< \brief (UART_IER) Enable End of Transmit Interrupt */
	#define UART_OVRE		(0x1u << 5) /**< \brief (UART_IER) Enable Overrun Error Interrupt */
	#define UART_FRAME		(0x1u << 6) /**< \brief (UART_IER) Enable Framing Error Interrupt */
	#define UART_PARE		(0x1u << 7) /**< \brief (UART_IER) Enable Parity Error Interrupt */
	#define UART_TXEMPTY	(0x1u << 9) /**< \brief (UART_IER) Enable TXEMPTY Interrupt */
	#define UART_TXBUFE		(0x1u << 11) /**< \brief (UART_IER) Enable Buffer Empty Interrupt */
	#define UART_RXBUFF		(0x1u << 12) /**< \brief (UART_IER) Enable Buffer Full Interrupt */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//struct S_PIT
	//{
	//	AT91_REG		MR;
	//	const AT91_REG	SR;
	//	const AT91_REG	PIVR;
	//	const AT91_REG	PIIR;
	//};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_RTT
	{
		AT91_REG	MR;
		AT91_REG	AR;
		AT91_RO		VR;
		AT91_RO		SR;
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_SPI
	{
		AT91_WO		CR;
		AT91_REG	MR;
		AT91_REG 	RDR;
		AT91_WO		TDR;
		AT91_REG	SR;
		AT91_WO		IER;
		AT91_WO		IDR;
		AT91_REG	IMR;
		AT91_REG	z__rsrvd1[4];
		AT91_REG	CSR[4];
		AT91_REG	z__rsrvd2[41];
		AT91_REG	WPMR;
		AT91_REG	WPSR;

		AT91_REG	z__rsrvd3[5];	

		S_PDC PDC;

	};

	/* -------- SPI_CR : (SPI Offset: 0x00) Control Register -------- */
	#define SPI_SPIEN				(0x1u << 0)				/**< \brief (SPI_CR) SPI Enable */
	#define SPI_SPIDIS				(0x1u << 1)				/**< \brief (SPI_CR) SPI Disable */
	#define SPI_SWRST				(0x1u << 7)				/**< \brief (SPI_CR) SPI Software Reset */
	#define SPI_LASTXFER			(0x1u << 24)			/**< \brief (SPI_CR) Last Transfer */

	/* -------- SPI_MR : (SPI Offset: 0x04) Mode Register -------- */
	#define SPI_MSTR				(0x1u << 0)				/**< \brief (SPI_MR) Master/Slave Mode */
	#define SPI_PS					(0x1u << 1)				/**< \brief (SPI_MR) Peripheral Select */
	#define SPI_PCSDEC				(0x1u << 2)				/**< \brief (SPI_MR) Chip Select Decode */
	#define SPI_MODFDIS				(0x1u << 4)				/**< \brief (SPI_MR) Mode Fault Detection */
	#define SPI_WDRBT				(0x1u << 5)				/**< \brief (SPI_MR) Wait Data Read Before Transfer */
	#define SPI_LLB					(0x1u << 7)				/**< \brief (SPI_MR) Local Loopback Enable */
	#define SPI_PCS(value)			(((value)&15) << 16)	/**< \brief (SPI_MR) Peripheral Chip Select */
	#define SPI_DLYBCS(value)		(((value)&0xFF) << 24)	/**< \brief (SPI_MR) Delay Between Chip Selects */

	/* -------- SPI_RDR : (SPI Offset: 0x08) Receive Data Register -------- */
	#define SPI_RDR_RD_Pos			0
	#define SPI_RDR_RD_Msk			(0xffffu << 0)			/**< \brief (SPI_RDR) Receive Data */
	#define SPI_RDR_PCS_Pos			16
	#define SPI_RDR_PCS_Msk			(0xfu << 16)			/**< \brief (SPI_RDR) Peripheral Chip Select */

	/* -------- SPI_TDR : (SPI Offset: 0x0C) Transmit Data Register -------- */
	#define SPI_TD(value)			(((value)&0xFFFF) << 0)	/**< \brief (SPI_TDR) Transmit Data */
	#define SPI_TDR_PCS(value)		(((value)&15) << 16)	/**< \brief (SPI_TDR) Peripheral Chip Select */
	#define SPI_LASTXFER			(0x1u << 24)			/**< \brief (SPI_TDR) Last Transfer */

	/* -------- SPI_SR : (SPI Offset: 0x10) Status Register -------- */
	#define SPI_RDRF				(0x1u << 0)				/**< \brief (SPI_SR) Receive Data Register Full */
	#define SPI_TDRE				(0x1u << 1)				/**< \brief (SPI_SR) Transmit Data Register Empty */
	#define SPI_MODF				(0x1u << 2)				/**< \brief (SPI_SR) Mode Fault Error */
	#define SPI_OVRES				(0x1u << 3)				/**< \brief (SPI_SR) Overrun Error Status */
	#define SPI_ENDRX				(0x1u << 4)				/**< \brief (SPI_SR) End of RX buffer */
	#define SPI_ENDTX				(0x1u << 5)				/**< \brief (SPI_SR) End of TX buffer */
	#define SPI_RXBUFF				(0x1u << 6)				/**< \brief (SPI_SR) RX Buffer Full */
	#define SPI_TXBUFE				(0x1u << 7)				/**< \brief (SPI_SR) TX Buffer Empty */
	#define SPI_NSSR				(0x1u << 8)				/**< \brief (SPI_SR) NSS Rising */
	#define SPI_TXEMPTY				(0x1u << 9)				/**< \brief (SPI_SR) Transmission Registers Empty */
	#define SPI_UNDES				(0x1u << 10)			/**< \brief (SPI_SR) Underrun Error Status (Slave mode Only) */
	#define SPI_SPIENS				(0x1u << 16)			/**< \brief (SPI_SR) SPI Enable Status */

	/* -------- SPI_CSR[4] : (SPI Offset: 0x30) Chip Select Register -------- */
	#define SPI_CPOL				(0x1u << 0)				/**< \brief (SPI_CSR[4]) Clock Polarity */
	#define SPI_NCPHA				(0x1u << 1)				/**< \brief (SPI_CSR[4]) Clock Phase */
	#define SPI_CSNAAT				(0x1u << 2)				/**< \brief (SPI_CSR[4]) Chip Select Not Active After Transfer (Ignored if CSAAT = 1) */
	#define SPI_CSAAT				(0x1u << 3)				/**< \brief (SPI_CSR[4]) Chip Select Active After Transfer */
	#define SPI_BITS(v)				((((v)-8)&0xfu) << 4)	/**< \brief (SPI_CSR[4]) Bits Per Transfer */
	#define	SPI_8BIT				(0x0u << 4)				/**< \brief (SPI_CSR[4]) 8 bits for transfer */
	#define	SPI_9BIT				(0x1u << 4)				/**< \brief (SPI_CSR[4]) 9 bits for transfer */
	#define	SPI_10BIT				(0x2u << 4)				/**< \brief (SPI_CSR[4]) 10 bits for transfer */
	#define	SPI_11BIT				(0x3u << 4)				/**< \brief (SPI_CSR[4]) 11 bits for transfer */
	#define	SPI_12BIT				(0x4u << 4)				/**< \brief (SPI_CSR[4]) 12 bits for transfer */
	#define	SPI_13BIT				(0x5u << 4)				/**< \brief (SPI_CSR[4]) 13 bits for transfer */
	#define	SPI_14BIT				(0x6u << 4)				/**< \brief (SPI_CSR[4]) 14 bits for transfer */
	#define	SPI_15BIT				(0x7u << 4)				/**< \brief (SPI_CSR[4]) 15 bits for transfer */
	#define	SPI_16BIT				(0x8u << 4)				/**< \brief (SPI_CSR[4]) 16 bits for transfer */
	#define SPI_SCBR(value)			(((value)&0xFF) << 8)	/**< \brief (SPI_CSR[4]) Serial Clock Baud Rate */
	#define SPI_DLYBS(value)		(((value)&0xFF) << 16)	/**< \brief (SPI_CSR[4]) Delay Before SPCK */
	#define SPI_DLYBCT(value)		(((value)&0xFF) << 24)	/**< \brief (SPI_CSR[4]) Delay Between Consecutive Transfers */
	 
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_PWM
	{
		AT91_REG		CLK;
		AT91_WO			ENA;
		AT91_WO			DIS;
		AT91_RO			SR;
		AT91_WO			IER1;
		AT91_WO			IDR1;
		AT91_RO			IMR1;
		AT91_RO			ISR1;
		AT91_REG		SCM;

		AT91_REG		zrsrv1;

		AT91_REG		SCUC;
		AT91_REG		SCUP;
		AT91_WO			SCUPUPD;
		AT91_WO			IER2;
		AT91_WO			IDR2;
		AT91_RO			IMR2;
		AT91_RO			ISR2;
		AT91_REG		OOV;
		AT91_REG		OS;
		AT91_WO			OSS;
		AT91_WO			OSC;
		AT91_WO			OSSUPD;
		AT91_WO			OSCUPD;
		AT91_REG		FMR;
		AT91_RO			FSR;
		AT91_WO			FCR;
		AT91_REG		FPV;
		AT91_REG		FPE;

		AT91_REG		zrsrv2[3];

		AT91_REG		ELMR0;
		AT91_REG		ELMR1;

		AT91_REG		zrsrv3[11];

		AT91_REG		SMMR;

		AT91_REG		zrsrv4[12];

		AT91_WO			WPCR;
		AT91_RO			WPSR;

		AT91_REG		zrsrv5[5];

		S_PDC PDC;

		AT91_REG		zrsrv6[2];

		struct { AT91_REG VR, VUR, MR, MUR; } CMP[13];

		struct { AT91_REG	MR, DTY, DTYUPD, PRD, PRDUPD, CNT, DT, DTUPD; } CH[4];
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/* -------- WDT_CR : (WDT Offset: 0x00) Control Register -------- */

	#define WDT_WDRSTT		(0x1u << 0)		/**< \brief (WDT_CR) Watchdog Restart */
	#define WDT_KEY_PASSWD	(0xA5u << 24)	/**< \brief (WDT_CR) Writing any other value in this field aborts the write operation. */

	/* -------- WDT_MR : (WDT Offset: 0x04) Mode Register -------- */

	#define WDT_WDV_MSK		(0xfffu << 0)			/**< \brief (WDT_MR) Watchdog Counter Value				*/
	#define WDT_WDV(value)	(((value)&0xFFF) << 0)	/**< \brief (WDT_MR) Watchdog Counter Value				*/
	#define WDT_WDFIEN		(0x1u << 12)			/**< \brief (WDT_MR) Watchdog Fault Interrupt Enable	*/
	#define WDT_WDRSTEN		(0x1u << 13)			/**< \brief (WDT_MR) Watchdog Reset Enable				*/
	#define WDT_WDRPROC		(0x1u << 14)			/**< \brief (WDT_MR) Watchdog Reset Processor			*/
	#define WDT_WDDIS		(0x1u << 15)			/**< \brief (WDT_MR) Watchdog Disable					*/
	#define WDT_WDD_MSK		(0xfffu << 16)			/**< \brief (WDT_MR) Watchdog Delta Value				*/
	#define WDT_WDD(value)	(((value)&0xFFF) << 16)	/**< \brief (WDT_MR) Watchdog Delta Value				*/
	#define WDT_WDDBGHLT	(0x1u << 28)			/**< \brief (WDT_MR) Watchdog Debug Halt				*/
	#define WDT_WDIDLEHLT	(0x1u << 29)			/**< \brief (WDT_MR) Watchdog Idle Halt					*/

	/* -------- WDT_SR : (WDT Offset: 0x08) Status Register -------- */

	#define WDT_WDUNF (0x1u << 0) /**< \brief (WDT_SR) Watchdog Underflow */
	#define WDT_WDERR (0x1u << 1) /**< \brief (WDT_SR) Watchdog Error */

	struct S_WDT
	{
		AT91_WO			CR;
		AT91_REG		MR;
		AT91_RO			SR;

		void Update()	{ CR = WDT_KEY_PASSWD|WDT_WDRSTT;	}
		void Reset()	{ CR = WDT_KEY_PASSWD|WDT_WDRSTT;	}
		void Disable()	{ MR = WDT_WDDIS;					}
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_RSTC
	{
		AT91_WO			CR;
		AT91_RO			SR;
		AT91_REG		MR;
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_ADC
	{
		AT91_WO			CR;
		AT91_REG		MR;

		AT91_REG		SEQR1;
		AT91_REG		SEQR2;

		AT91_WO			CHER;
		AT91_WO			CHDR;
		AT91_RO			CHSR;

		AT91_RO			zrsrv;

		AT91_RO			LCDR;

		AT91_WO			IER;
		AT91_WO			IDR;
		AT91_RO			IMR;
		AT91_RO			ISR;

		AT91_RO			zrsrv2[2];

		AT91_RO			OVER;
		AT91_REG		EMR;
		AT91_REG		CWR;
		AT91_REG		CGR;
		AT91_REG		COR;
		AT91_RO			CDR[16];

		AT91_RO			zrsrv3;

		AT91_REG		ACR;

		AT91_RO			zrsrv4[19];

		AT91_REG		WPMR;
		AT91_RO			WPSR;

		AT91_RO			zrsrv5[5];

		S_PDC			PDC;
	};

	/* -------- ADC_CR : (ADC Offset: 0x00) Control Register -------- */

	#define ADC_SWRST	(0x1u << 0) /**< \brief (ADC_CR) Software Reset */
	#define ADC_START	(0x1u << 1) /**< \brief (ADC_CR) Start Conversion */
	#define ADC_AUTOCAL (0x1u << 3) /**< \brief (ADC_CR) Automatic Calibration of ADC */

	/* -------- ADC_MR : (ADC Offset: 0x04) Mode Register -------- */

	#define ADC_TRGEN			(0x1u << 0) /**< \brief (ADC_MR) Trigger Enable */
	#define ADC_TRGSEL_MSK		(0x7u << 1) /**< \brief (ADC_MR) Trigger Selection */
	#define	ADC_TRGSEL_EXT		(0x0u << 1) /**< \brief (ADC_MR) External trigger */
	#define	ADC_TRGSEL_TIO0		(0x1u << 1) /**< \brief (ADC_MR) TIO Output of the Timer Counter Channel 0 */
	#define	ADC_TRGSEL_TIO1		(0x2u << 1) /**< \brief (ADC_MR) TIO Output of the Timer Counter Channel 1 */
	#define	ADC_TRGSEL_TIO2		(0x3u << 1) /**< \brief (ADC_MR) TIO Output of the Timer Counter Channel 2 */
	#define	ADC_TRGSEL_PWM0		(0x4u << 1) /**< \brief (ADC_MR) PWM Event Line 0 */
	#define	ADC_TRGSEL_PWM1		(0x5u << 1) /**< \brief (ADC_MR) PWM Event Line 1 */
	#define ADC_SLEEP			(0x1u << 5) /**< \brief (ADC_MR) Sleep Mode */
	#define ADC_FWUP			(0x1u << 6) /**< \brief (ADC_MR) Fast Wake Up */
	#define ADC_FREERUN			(0x1u << 7) /**< \brief (ADC_MR) Free Run Mode */
	#define ADC_PRESCAL_MSK		(0xFFu << 8) /**< \brief (ADC_MR) Prescaler Rate Selection */
	#define ADC_PRESCAL(value)	(((value)&0xFF) << 8)
	#define ADC_STARTUP_MSK		(0xFU << 16) /**< \brief (ADC_MR) Start Up Time */
	#define	ADC_STARTUP_0		(0x0u << 16) /**< \brief (ADC_MR) 0 periods of ADCClock */
	#define	ADC_STARTUP_8		(0x1u << 16) /**< \brief (ADC_MR) 8 periods of ADCClock */
	#define	ADC_STARTUP_16		(0x2u << 16) /**< \brief (ADC_MR) 16 periods of ADCClock */
	#define	ADC_STARTUP_24		(0x3u << 16) /**< \brief (ADC_MR) 24 periods of ADCClock */
	#define	ADC_STARTUP_64		(0x4u << 16) /**< \brief (ADC_MR) 64 periods of ADCClock */
	#define	ADC_STARTUP_80		(0x5u << 16) /**< \brief (ADC_MR) 80 periods of ADCClock */
	#define	ADC_STARTUP_96		(0x6u << 16) /**< \brief (ADC_MR) 96 periods of ADCClock */
	#define	ADC_STARTUP_112		(0x7u << 16) /**< \brief (ADC_MR) 112 periods of ADCClock */
	#define	ADC_STARTUP_512		(0x8u << 16) /**< \brief (ADC_MR) 512 periods of ADCClock */
	#define	ADC_STARTUP_576		(0x9u << 16) /**< \brief (ADC_MR) 576 periods of ADCClock */
	#define	ADC_STARTUP_640		(0xAu << 16) /**< \brief (ADC_MR) 640 periods of ADCClock */
	#define	ADC_STARTUP_704		(0xBu << 16) /**< \brief (ADC_MR) 704 periods of ADCClock */
	#define	ADC_STARTUP_768		(0xCu << 16) /**< \brief (ADC_MR) 768 periods of ADCClock */
	#define	ADC_STARTUP_832		(0xDu << 16) /**< \brief (ADC_MR) 832 periods of ADCClock */
	#define	ADC_STARTUP_896		(0xEu << 16) /**< \brief (ADC_MR) 896 periods of ADCClock */
	#define	ADC_STARTUP_960		(0xFu << 16) /**< \brief (ADC_MR) 960 periods of ADCClock */
	#define ADC_SETTLING_MSK	(0x3u << 20) /**< \brief (ADC_MR) Analog Settling Time */
	#define	ADC_SETTLING_AST3	(0x0u << 20) /**< \brief (ADC_MR) 3 periods of ADCClock */
	#define	ADC_SETTLING_AST5	(0x1u << 20) /**< \brief (ADC_MR) 5 periods of ADCClock */
	#define	ADC_SETTLING_AST9	(0x2u << 20) /**< \brief (ADC_MR) 9 periods of ADCClock */
	#define	ADC_SETTLING_AST17	(0x3u << 20) /**< \brief (ADC_MR) 17 periods of ADCClock */
	#define ADC_ANACH			(0x1u << 23) /**< \brief (ADC_MR) Analog Change */
	#define ADC_TRACKTIM_MSK	(0xfu << 24) /**< \brief (ADC_MR) Tracking Time */
	#define ADC_TRACKTIM(value)	(((value)&15) << 24)
	#define ADC_TRANSFER_MSK	(0x3u << 28) /**< \brief (ADC_MR) Transfer Period */
	#define ADC_TRANSFER(value)	(((value)&3) << 28)
	#define	ADC_USEQ			(0x1u << 31) /**< \brief (ADC_MR) Use Sequence Enable */
	#define	ADC_USEQ_NUM_ORDER	(0x0u << 31) /**< \brief (ADC_MR) Normal Mode: The controller converts channels in a simple numeric order depending only on the channel index. */
	#define	ADC_USEQ_REG_ORDER	(0x1u << 31) /**< \brief (ADC_MR) User Sequence Mode: The sequence respects what is defined in ADC_SEQR1 and ADC_SEQR2 registers and can be used to convert several times the same channel. */

	/* -------- ADC_CHER : (ADC Offset: 0x10) Channel Enable Register -------- */
	#define ADC_CH0		(0x1u << 0) /**< \brief (ADC_CHER) Channel 0 Enable */
	#define ADC_CH1		(0x1u << 1) /**< \brief (ADC_CHER) Channel 1 Enable */
	#define ADC_CH2		(0x1u << 2) /**< \brief (ADC_CHER) Channel 2 Enable */
	#define ADC_CH3		(0x1u << 3) /**< \brief (ADC_CHER) Channel 3 Enable */
	#define ADC_CH4		(0x1u << 4) /**< \brief (ADC_CHER) Channel 4 Enable */
	#define ADC_CH5		(0x1u << 5) /**< \brief (ADC_CHER) Channel 5 Enable */
	#define ADC_CH6		(0x1u << 6) /**< \brief (ADC_CHER) Channel 6 Enable */
	#define ADC_CH7		(0x1u << 7) /**< \brief (ADC_CHER) Channel 7 Enable */
	#define ADC_CH8		(0x1u << 8) /**< \brief (ADC_CHER) Channel 8 Enable */
	#define ADC_CH9		(0x1u << 9) /**< \brief (ADC_CHER) Channel 9 Enable */
	#define ADC_CH10	(0x1u << 10) /**< \brief (ADC_CHER) Channel 10 Enable */
	#define ADC_CH11	(0x1u << 11) /**< \brief (ADC_CHER) Channel 11 Enable */
	#define ADC_CH12	(0x1u << 12) /**< \brief (ADC_CHER) Channel 12 Enable */
	#define ADC_CH13	(0x1u << 13) /**< \brief (ADC_CHER) Channel 13 Enable */
	#define ADC_CH14	(0x1u << 14) /**< \brief (ADC_CHER) Channel 14 Enable */
	#define ADC_CH15	(0x1u << 15) /**< \brief (ADC_CHER) Channel 15 Enable */

	/* -------- ADC_IER : (ADC Offset: 0x24) Interrupt Enable Register -------- */
	#define ADC_EOC0	(0x1u << 0) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 0 */
	#define ADC_EOC1	(0x1u << 1) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 1 */
	#define ADC_EOC2	(0x1u << 2) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 2 */
	#define ADC_EOC3	(0x1u << 3) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 3 */
	#define ADC_EOC4	(0x1u << 4) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 4 */
	#define ADC_EOC5	(0x1u << 5) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 5 */
	#define ADC_EOC6	(0x1u << 6) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 6 */
	#define ADC_EOC7	(0x1u << 7) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 7 */
	#define ADC_EOC8	(0x1u << 8) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 8 */
	#define ADC_EOC9	(0x1u << 9) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 9 */
	#define ADC_EOC10	(0x1u << 10) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 10 */
	#define ADC_EOC11	(0x1u << 11) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 11 */
	#define ADC_EOC12	(0x1u << 12) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 12 */
	#define ADC_EOC13	(0x1u << 13) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 13 */
	#define ADC_EOC14	(0x1u << 14) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 14 */
	#define ADC_EOC15	(0x1u << 15) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 15 */
	#define ADC_EOCAL	(0x1u << 23) /**< \brief (ADC_IER) End of Calibration Sequence */
	#define ADC_DRDY	(0x1u << 24) /**< \brief (ADC_IER) Data Ready Interrupt Enable */
	#define ADC_GOVRE	(0x1u << 25) /**< \brief (ADC_IER) General Overrun Error Interrupt Enable */
	#define ADC_COMPE	(0x1u << 26) /**< \brief (ADC_IER) Comparison Event Interrupt Enable */
	#define ADC_ENDRX	(0x1u << 27) /**< \brief (ADC_IER) End of Receive Buffer Interrupt Enable */
	#define ADC_RXBUFF	(0x1u << 28) /**< \brief (ADC_IER) Receive Buffer Full Interrupt Enable */

	/* -------- ADC_OVER : (ADC Offset: 0x3C) Overrun Status Register -------- */
	#define ADC_OVRE0	(0x1u << 0) /**< \brief (ADC_OVER) Overrun Error 0 */
	#define ADC_OVRE1	(0x1u << 1) /**< \brief (ADC_OVER) Overrun Error 1 */
	#define ADC_OVRE2	(0x1u << 2) /**< \brief (ADC_OVER) Overrun Error 2 */
	#define ADC_OVRE3	(0x1u << 3) /**< \brief (ADC_OVER) Overrun Error 3 */
	#define ADC_OVRE4	(0x1u << 4) /**< \brief (ADC_OVER) Overrun Error 4 */
	#define ADC_OVRE5	(0x1u << 5) /**< \brief (ADC_OVER) Overrun Error 5 */
	#define ADC_OVRE6	(0x1u << 6) /**< \brief (ADC_OVER) Overrun Error 6 */
	#define ADC_OVRE7	(0x1u << 7) /**< \brief (ADC_OVER) Overrun Error 7 */
	#define ADC_OVRE8	(0x1u << 8) /**< \brief (ADC_OVER) Overrun Error 8 */
	#define ADC_OVRE9	(0x1u << 9) /**< \brief (ADC_OVER) Overrun Error 9 */
	#define ADC_OVRE10	(0x1u << 10) /**< \brief (ADC_OVER) Overrun Error 10 */
	#define ADC_OVRE11	(0x1u << 11) /**< \brief (ADC_OVER) Overrun Error 11 */
	#define ADC_OVRE12	(0x1u << 12) /**< \brief (ADC_OVER) Overrun Error 12 */
	#define ADC_OVRE13	(0x1u << 13) /**< \brief (ADC_OVER) Overrun Error 13 */
	#define ADC_OVRE14	(0x1u << 14) /**< \brief (ADC_OVER) Overrun Error 14 */
	#define ADC_OVRE15	(0x1u << 15) /**< \brief (ADC_OVER) Overrun Error 15 */

	/* -------- ADC_ACR : (ADC Offset: 0x94) Analog Control Register -------- */
	#define ADC_TSON			(0x1u << 4) /**< \brief (ADC_ACR) Temperature Sensor On */
	#define ADC_IBCTL_MSK		(0x3u << 8) /**< \brief (ADC_ACR) ADC Bias Current Control */
	#define ADC_IBCTL(value)	(((value)&3) << 8)

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_RTC
	{
		AT91_REG		CR;
		AT91_REG		MR;
		AT91_REG		TIMR;
		AT91_REG		CALR;
		AT91_REG		TIMALR;
		AT91_REG		CALALR;
		AT91_RO			SR;
		AT91_WO			SCCR;
		AT91_WO			IER;
		AT91_WO			IDR;
		AT91_RO			IMR;
		AT91_RO			VER;
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_MATRIX
	{
		AT91_REG		MCFG[16];
		AT91_REG		SCFG[16];
		struct { AT91_REG A; AT91_REG B; } PRAS[18];
		AT91_RO			zreserve;
		AT91_REG		SYSIO;
		AT91_RO			zreserve1;
		AT91_REG		SMCNFCS;
	};

	/* -------- CCFG_SYSIO : (MATRIX Offset: 0x0114) System I/O Configuration register -------- */
	#define MATRIX_PB4	(0x1u << 4)		/**< \brief (CCFG_SYSIO) PB4 or TDI Assignment */
	#define MATRIX_PB5	(0x1u << 5)		/**< \brief (CCFG_SYSIO) PB5 or TDO/TRACESWO Assignment */
	#define MATRIX_PB6	(0x1u << 6)		/**< \brief (CCFG_SYSIO) PB6 or TMS/SWDIO Assignment */
	#define MATRIX_PB7	(0x1u << 7)		/**< \brief (CCFG_SYSIO) PB7 or TCK/SWCLK Assignment */
	#define MATRIX_PB10	(0x1u << 10)	/**< \brief (CCFG_SYSIO) PB10 or DDM Assignment */
	#define MATRIX_PB11	(0x1u << 11)	/**< \brief (CCFG_SYSIO) PB11 or DDP Assignment */
	#define MATRIX_PB12	(0x1u << 12)	/**< \brief (CCFG_SYSIO) PB12 or ERASE Assignment */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_EFC
	{
		AT91_REG		FMR;
		AT91_WO			FCR;
		AT91_RO			FSR;
		AT91_RO			FRR;
	};

	typedef S_EFC S_EFC0, S_EFC1;

	/* -------- EEFC_FMR : (EFC Offset: 0x00) EEFC Flash Mode Register -------- */
	#define EFC_FRDY		(0x1u << 0) /**< \brief (EEFC_FMR) Ready Interrupt Enable */
	#define EFC_FWS_MSK		(0xfu << 8) /**< \brief (EEFC_FMR) Flash Wait State */
	#define EFC_FWS(value)	(((value)&15) << 8)
	#define EFC_SCOD		(0x1u << 16) /**< \brief (EEFC_FMR) Sequential Code Optimization Disable */
	#define EFC_FAM			(0x1u << 24) /**< \brief (EEFC_FMR) Flash Access Mode */
	#define EFC_CLOE		(0x1u << 26) /**< \brief (EEFC_FMR) Code Loop Optimization Enable */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_SUPC
	{
		AT91_WO			CR;
		AT91_REG		SMMR;
		AT91_REG		MR;
		AT91_REG		WUMR;
		AT91_REG		WUIR;
		AT91_RO			SR;
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//struct S_DMAC
	//{
	//	AT91_REG		GCFG;
	//	AT91_REG		EN;
	//	AT91_REG		SREQ;
	//	AT91_REG		CREQ;
	//	AT91_REG		LAST;
	//	AT91_REG		z__rsrvd1;
	//	AT91_REG		EBCIER;
	//	AT91_REG		EBCIDR;
	//	AT91_REG		EBCIMR;
	//	AT91_REG		EBCISR;
	//	AT91_REG		CHER;
	//	AT91_REG		CHDR;
	//	AT91_REG		CHSR;
	//	AT91_REG		z__rsrvd2[2];

	//	struct { AT91_PTR SADDR, DADDR, DSCR; AT91_REG CTRLA, CTRLB, CFG, z__rsrvd[4]; } CH[6];

	//	AT91_REG		z__rsrvd3[46];

	//	AT91_REG		WPMR;
	//	AT91_REG		WPSR;
	//};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_CMCC
	{
		AT91_RO			TYPE;
		AT91_REG		CFG;
		AT91_WO			CTRL;
		AT91_RO			SR;
		AT91_RO			z__rsrvd[4];
		AT91_WO			MAINT0;
		AT91_WO			MAINT1;
		AT91_REG		MCFG;
		AT91_REG		MEN;
		AT91_WO			MCTRL;
		AT91_RO			MSR;
	};

	/* -------- CMCC_CFG : (CMCC Offset: 0x04) Cache Configuration Register -------- */

	#define CMCC_GCLKDIS (0x1u << 0) /**< \brief (CMCC_CFG) Disable Clock Gating */

	/* -------- CMCC_CTRL : (CMCC Offset: 0x08) Cache Control Register -------- */

	#define CMCC_CEN (0x1u << 0) /**< \brief (CMCC_CTRL) Cache Controller Enable */

	/* -------- CMCC_SR : (CMCC Offset: 0x0C) Cache Status Register -------- */

	#define CMCC_CSTS (0x1u << 0) /**< \brief (CMCC_SR) Cache Controller Status */

	/* -------- CMCC_MAINT0 : (CMCC Offset: 0x20) Cache Maintenance Register 0 -------- */

	#define CMCC_INVALL (0x1u << 0) /**< \brief (CMCC_MAINT0) Cache Controller Invalidate All */

	/* -------- CMCC_MAINT1 : (CMCC Offset: 0x24) Cache Maintenance Register 1 -------- */

	#define CMCC_INDEX(value)	(((value)&0x1F) << 4)	/**< \brief (CMCC_MAINT1) Invalidate Index */
	#define	CMCC_WAY0			(0x0u << 30) /**< \brief (CMCC_MAINT1) Way 0 is selection for index invalidation */
	#define	CMCC_WAY1			(0x1u << 30) /**< \brief (CMCC_MAINT1) Way 1 is selection for index invalidation */
	#define	CMCC_WAY2			(0x2u << 30) /**< \brief (CMCC_MAINT1) Way 2 is selection for index invalidation */
	#define	CMCC_WAY3			(0x3u << 30) /**< \brief (CMCC_MAINT1) Way 3 is selection for index invalidation */

	/* -------- CMCC_MCFG : (CMCC Offset: 0x28) Cache Monitor Configuration Register -------- */

	#define   CMCC_MODE_CYCLE_COUNT	(0x0u << 0) /**< \brief (CMCC_MCFG) Cycle counter */
	#define   CMCC_MODE_IHIT_COUNT	(0x1u << 0) /**< \brief (CMCC_MCFG) Instruction hit counter */
	#define   CMCC_MODE_DHIT_COUNT	(0x2u << 0) /**< \brief (CMCC_MCFG) Data hit counter */

	/* -------- CMCC_MEN : (CMCC Offset: 0x2C) Cache Monitor Enable Register -------- */

	#define CMCC_MENABLE (0x1u << 0) /**< \brief (CMCC_MEN) Cache Controller Monitor Enable */
	
	/* -------- CMCC_MCTRL : (CMCC Offset: 0x30) Cache Monitor Control Register -------- */

	#define CMCC_SWRST (0x1u << 0) /**< \brief (CMCC_MCTRL) Monitor */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_TWI
	{
		AT91_WO			CR;
		AT91_REG		MMR;
		AT91_REG		SMR;
		AT91_REG		IADR;
		AT91_REG		CWGR;
		AT91_REG		z__reserved1[3];
		AT91_REG		SR;
		AT91_WO			IER;
		AT91_WO			IDR;
		AT91_REG		IMR;
		AT91_REG		RHR;
		AT91_WO			THR;
		AT91_REG		z__reserved2[50];
		S_PDC			PDC;
	};

	typedef S_TWI S_TWI0, S_TWI1;

	/* -------- TWI_CR : (TWI Offset: 0x00) Control Register -------- */
	#define TWI_START				(0x1u << 0)					/**< \brief (TWI_CR) Send a START Condition */
	#define TWI_STOP				(0x1u << 1)					/**< \brief (TWI_CR) Send a STOP Condition */
	#define TWI_MSEN				(0x1u << 2)					/**< \brief (TWI_CR) TWI Master Mode Enabled */
	#define TWI_MSDIS				(0x1u << 3)					/**< \brief (TWI_CR) TWI Master Mode Disabled */
	#define TWI_SVEN				(0x1u << 4)					/**< \brief (TWI_CR) TWI Slave Mode Enabled */
	#define TWI_SVDIS				(0x1u << 5)					/**< \brief (TWI_CR) TWI Slave Mode Disabled */
	#define TWI_QUICK				(0x1u << 6)					/**< \brief (TWI_CR) SMBUS Quick Command */
	#define TWI_SWRST				(0x1u << 7)					/**< \brief (TWI_CR) Software Reset */
	/* -------- TWI_MMR : (TWI Offset: 0x04) Master Mode Register -------- */
	#define TWI_IADRSZ(v)			(((v)&3) << 8)				/**< \brief (TWI_MMR) Internal Device Address Size */
	#define	TWI_IADRSZ_NONE			(0x0u << 8)					/**< \brief (TWI_MMR) No internal device address */
	#define	TWI_IADRSZ_1_BYTE		(0x1u << 8)					/**< \brief (TWI_MMR) One-byte internal device address */
	#define	TWI_IADRSZ_2_BYTE		(0x2u << 8)					/**< \brief (TWI_MMR) Two-byte internal device address */
	#define	TWI_IADRSZ_3_BYTE		(0x3u << 8)					/**< \brief (TWI_MMR) Three-byte internal device address */
	#define TWI_MREAD				(0x1u << 12)				/**< \brief (TWI_MMR) Master Read Direction */
	#define TWI_DADR(value)			(((value)&0x7F) << 16)		/**< \brief (TWI_MMR) Device Address */
	/* -------- TWI_SMR : (TWI Offset: 0x08) Slave Mode Register -------- */
	#define TWI_SADR(value)			(((value)&0x7F) << 16)		/**< \brief (TWI_SMR) Slave Address */
	/* -------- TWI_IADR : (TWI Offset: 0x0C) Internal Address Register -------- */
	#define TWI_IADR(value)			(((value)&0XFFFFFF) << 0)	/**< \brief (TWI_IADR) Internal Address */
	/* -------- TWI_CWGR : (TWI Offset: 0x10) Clock Waveform Generator Register -------- */
	#define TWI_CWGR_CLDIV(value)	(((value)&0xFF) << 0)		/**< \brief (TWI_CWGR) Clock Low Divider */
	#define TWI_CWGR_CHDIV(value)	(((value)&0xFF) << 8)		/**< \brief (TWI_CWGR) Clock High Divider */
	#define TWI_CWGR_CKDIV(value)	(((value)&7) << 16)			/**< \brief (TWI_CWGR) Clock Divider */
	/* -------- TWI_SR : (TWI Offset: 0x20) Status Register -------- */
	#define TWI_TXCOMP				(0x1u << 0)					/**< \brief (TWI_SR) Transmission Completed (automatically set / reset) */
	#define TWI_RXRDY				(0x1u << 1)					/**< \brief (TWI_SR) Receive Holding Register Ready (automatically set / reset) */
	#define TWI_TXRDY				(0x1u << 2)					/**< \brief (TWI_SR) Transmit Holding Register Ready (automatically set / reset) */
	#define TWI_SVREAD				(0x1u << 3)					/**< \brief (TWI_SR) Slave Read (automatically set / reset) */
	#define TWI_SVACC				(0x1u << 4)					/**< \brief (TWI_SR) Slave Access (automatically set / reset) */
	#define TWI_GACC				(0x1u << 5)					/**< \brief (TWI_SR) General Call Access (clear on read) */
	#define TWI_OVRE				(0x1u << 6)					/**< \brief (TWI_SR) Overrun Error (clear on read) */
	#define TWI_NACK				(0x1u << 8)					/**< \brief (TWI_SR) Not Acknowledged (clear on read) */
	#define TWI_ARBLST				(0x1u << 9)					/**< \brief (TWI_SR) Arbitration Lost (clear on read) */
	#define TWI_SCLWS				(0x1u << 10)				/**< \brief (TWI_SR) Clock Wait State (automatically set / reset) */
	#define TWI_EOSACC				(0x1u << 11)				/**< \brief (TWI_SR) End Of Slave Access (clear on read) */
	#define TWI_ENDRX				(0x1u << 12)				/**< \brief (TWI_SR) End of RX buffer */
	#define TWI_ENDTX				(0x1u << 13)				/**< \brief (TWI_SR) End of TX buffer */
	#define TWI_RXBUFF				(0x1u << 14)				/**< \brief (TWI_SR) RX Buffer Full */
	#define TWI_TXBUFE				(0x1u << 15)				/**< \brief (TWI_SR) TX Buffer Empty */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	union USIC
	{
		S_USART		usart;
		S_UART		uart;
		S_SPI		spi;
		S_TWI		i2c;

		//USIC()				: usart(0)	{}
		//USIC(S_USART *p)	: usart(p)	{}
		//USIC(S_UART *p)		: uart(p)	{}
		//USIC(S_SPI *p)		: spi(p)	{}
		//USIC(S_TWI *p)		: i2c(p)	{}
	};

	typedef USIC S_USIC0, S_USIC1, S_USIC2, S_USIC3, S_USIC4, S_USIC5, S_USIC6;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_CRCCU
	{
		AT91_PTR DSCR;		/**< \brief (Crccu Offset: 0x000) CRCCU Descriptor Base Register */
		AT91_RO				z__Reserved1[1];
		AT91_WO	 DMA_EN;	/**< \brief (Crccu Offset: 0x008) CRCCU DMA Enable Register */
		AT91_WO	 DMA_DIS;	/**< \brief (Crccu Offset: 0x00C) CRCCU DMA Disable Register */
		AT91_RO	 DMA_SR;	/**< \brief (Crccu Offset: 0x010) CRCCU DMA Status Register */
		AT91_WO	 DMA_IER;	/**< \brief (Crccu Offset: 0x014) CRCCU DMA Interrupt Enable Register */
		AT91_WO	 DMA_IDR;	/**< \brief (Crccu Offset: 0x018) CRCCU DMA Interrupt Disable Register */
		AT91_RO	 DMA_IMR;	/**< \brief (Crccu Offset: 0x001C) CRCCU DMA Interrupt Mask Register */
		AT91_RO	 DMA_ISR;	/**< \brief (Crccu Offset: 0x020) CRCCU DMA Interrupt Status Register */
		AT91_RO				z__Reserved2[4];
		AT91_WO	 CR;		/**< \brief (Crccu Offset: 0x034) CRCCU Control Register */
		AT91_REG MR;		/**< \brief (Crccu Offset: 0x038) CRCCU Mode Register */
		AT91_RO	 SR;		/**< \brief (Crccu Offset: 0x03C) CRCCU Status Register */
		AT91_WO	 IER;		/**< \brief (Crccu Offset: 0x040) CRCCU Interrupt Enable Register */
		AT91_WO	 IDR;		/**< \brief (Crccu Offset: 0x044) CRCCU Interrupt Disable Register */
		AT91_RO	 IMR;		/**< \brief (Crccu Offset: 0x048) CRCCU Interrupt Mask Register */
		AT91_RO	 ISR;		/**< \brief (Crccu Offset: 0x004C) CRCCU Interrupt Status Register */

		struct TRDSCR
		{
			const void*		ADDR;
			u32				CTRL;
			u32				z__Reserved[2];
			u32				CRC;
		};
	};

	#define CRCCU_DMAEN		(0x1u << 0)		/**< \brief (CRCCU_DMA_EN)	DMA Enable Register */
	#define CRCCU_DMADIS	(0x1u << 0)		/**< \brief (CRCCU_DMA_DIS) DMA Disable Register */
	#define CRCCU_DMASR		(0x1u << 0)		/**< \brief (CRCCU_DMA_SR)	DMA Status Register */
	#define CRCCU_DMAIER	(0x1u << 0)		/**< \brief (CRCCU_DMA_IER) Interrupt Enable register */
	#define CRCCU_DMAIDR	(0x1u << 0)		/**< \brief (CRCCU_DMA_IDR) Interrupt Disable register */
	#define CRCCU_DMAIMR	(0x1u << 0)		/**< \brief (CRCCU_DMA_IMR) Interrupt Mask Register */
	#define CRCCU_DMAISR	(0x1u << 0)		/**< \brief (CRCCU_DMA_ISR) Interrupt Status register */
	#define CRCCU_RESET		(0x1u << 0)		/**< \brief (CRCCU_CR) CRC	Computation Reset */

	/* -------- CRCCU_MR : (CRCCU Offset: 0x038) CRCCU Mode Register -------- */
	#define CRCCU_ENABLE			(0x1u << 0) /**< \brief (CRCCU_MR) CRC Enable */
	#define CRCCU_COMPARE			(0x1u << 1) /**< \brief (CRCCU_MR) CRC Compare */
	#define	CRCCU_CCITT8023			(0x0u << 2) /**< \brief (CRCCU_MR) Polynom 0x04C11DB7 */
	#define	CRCCU_CASTAGNOLI		(0x1u << 2) /**< \brief (CRCCU_MR) Polynom 0x1EDC6F41 */
	#define	CRCCU_CCITT16			(0x2u << 2) /**< \brief (CRCCU_MR) Polynom 0x1021 */
	#define CRCCU_MR_DIVIDER_MSK	(0xfu << 4) /**< \brief (CRCCU_MR) Request Divider */
	#define CRCCU_MR_DIVIDER(value) (((value)&15) << 4)

	#define CRCCU_ERRIER			(0x1u << 0) /**< \brief (CRCCU_IER) CRC Error Interrupt Enable */
	#define CRCCU_ERRIDR			(0x1u << 0) /**< \brief (CRCCU_IDR) CRC Error Interrupt Disable */
	#define CRCCU_ERRIMR			(0x1u << 0) /**< \brief (CRCCU_IMR) CRC Error Interrupt Mask */
	#define CRCCU_ERRISR			(0x1u << 0) /**< \brief (CRCCU_ISR) CRC Error Interrupt Status */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_SSC
	{
		AT91_WO		CR;		/**< \brief (Ssc Offset: 0x0) Control Register */
		AT91_REG	CMR;	/**< \brief (Ssc Offset: 0x4) Clock Mode Register */
		AT91_RO				z__Reserved1[2];
		AT91_REG	RCMR;	/**< \brief (Ssc Offset: 0x10) Receive Clock Mode Register */
		AT91_REG	RFMR;	/**< \brief (Ssc Offset: 0x14) Receive Frame Mode Register */
		AT91_REG	TCMR;	/**< \brief (Ssc Offset: 0x18) Transmit Clock Mode Register */
		AT91_REG	TFMR;	/**< \brief (Ssc Offset: 0x1C) Transmit Frame Mode Register */
		AT91_RO		RHR;	/**< \brief (Ssc Offset: 0x20) Receive Holding Register */
		AT91_WO		THR;	/**< \brief (Ssc Offset: 0x24) Transmit Holding Register */
		AT91_RO				z__Reserved2[2];
		AT91_RO		RSHR;	/**< \brief (Ssc Offset: 0x30) Receive Sync. Holding Register */
		AT91_REG	TSHR;	/**< \brief (Ssc Offset: 0x34) Transmit Sync. Holding Register */
		AT91_REG	RC0R;	/**< \brief (Ssc Offset: 0x38) Receive Compare 0 Register */
		AT91_REG	RC1R;	/**< \brief (Ssc Offset: 0x3C) Receive Compare 1 Register */
		AT91_RO		SR;		/**< \brief (Ssc Offset: 0x40) Status Register */
		AT91_WO		IER;	/**< \brief (Ssc Offset: 0x44) Interrupt Enable Register */
		AT91_WO		IDR;	/**< \brief (Ssc Offset: 0x48) Interrupt Disable Register */
		AT91_RO		IMR;	/**< \brief (Ssc Offset: 0x4C) Interrupt Mask Register */
		AT91_RO				z__Reserved3[37];
		AT91_REG	WPMR;	/**< \brief (Ssc Offset: 0xE4) Write Protect Mode Register */
		AT91_RO		WPSR;	/**< \brief (Ssc Offset: 0xE8) Write Protect Status Register */
		AT91_RO				z__Reserved4[5];

		S_PDC		PDC;
	};

	/* -------- SSC_CR : (SSC Offset: 0x0) Control Register -------- */
	#define SSC_RXEN	(0x1u << 0)		/**< \brief (SSC_CR) Receive Enable */
	#define SSC_RXDIS	(0x1u << 1)		/**< \brief (SSC_CR) Receive Disable */
	#define SSC_TXEN	(0x1u << 8)		/**< \brief (SSC_CR) Transmit Enable */
	#define SSC_TXDIS	(0x1u << 9)		/**< \brief (SSC_CR) Transmit Disable */
	#define SSC_SWRST	(0x1u << 15)	/**< \brief (SSC_CR) Software Reset */
	
	/* -------- SSC_CMR : (SSC Offset: 0x4) Clock Mode Register -------- */
	#define SSC_DIV(v) ((v)&0xFFF) /**< \brief (SSC_CMR) Clock Divider */

	/* -------- SSC_RCMR : (SSC Offset: 0x10) Receive Clock Mode Register -------- */
	#define	SSC_RCKS_MCK			(0x0u << 0)			/**< \brief (SSC_RCMR) Divided Clock */
	#define	SSC_RCKS_TK				(0x1u << 0)			/**< \brief (SSC_RCMR) TK Clock signal */
	#define	SSC_RCKS_RK				(0x2u << 0)			/**< \brief (SSC_RCMR) RK pin */
	#define	SSC_RCKO_NONE			(0x0u << 2)			/**< \brief (SSC_RCMR) None, RK pin is an input */
	#define	SSC_RCKO_CONTINUOUS		(0x1u << 2)			/**< \brief (SSC_RCMR) Continuous Receive Clock, RK pin is an output */
	#define	SSC_RCKO_TRANSFER		(0x2u << 2)			/**< \brief (SSC_RCMR) Receive Clock only during data transfers, RK pin is an output */
	#define	SSC_RCKI				(0x1u << 5)			/**< \brief (SSC_RCMR) Receive Clock Inversion */
	#define	SSC_RCKG_CONTINUOUS		(0x0u << 6)			/**< \brief (SSC_RCMR) None */
	#define	SSC_RCKG_EN_RF_LOW		(0x1u << 6)			/**< \brief (SSC_RCMR) Receive Clock enabled only if RF Low */
	#define	SSC_RCKG_EN_RF_HIGH		(0x2u << 6)			/**< \brief (SSC_RCMR) Receive Clock enabled only if RF High */
	#define	SSC_RSTART_CONTINUOUS	(0x0u << 8)			/**< \brief (SSC_RCMR) Continuous, as soon as the receiver is enabled, and immediately after the end of transfer of the previous data. */
	#define	SSC_RSTART_TRANSMIT		(0x1u << 8)			/**< \brief (SSC_RCMR) Transmit start */
	#define	SSC_RSTART_RF_LOW		(0x2u << 8)			/**< \brief (SSC_RCMR) Detection of a low level on RF signal */
	#define	SSC_RSTART_RF_HIGH		(0x3u << 8)			/**< \brief (SSC_RCMR) Detection of a high level on RF signal */
	#define	SSC_RSTART_RF_FALLING	(0x4u << 8)			/**< \brief (SSC_RCMR) Detection of a falling edge on RF signal */
	#define	SSC_RSTART_RF_RISING	(0x5u << 8)			/**< \brief (SSC_RCMR) Detection of a rising edge on RF signal */
	#define	SSC_RSTART_RF_LEVEL		(0x6u << 8)			/**< \brief (SSC_RCMR) Detection of any level change on RF signal */
	#define	SSC_RSTART_RF_EDGE		(0x7u << 8)			/**< \brief (SSC_RCMR) Detection of any edge on RF signal */
	#define	SSC_RSTART_CMP_0		(0x8u << 8)			/**< \brief (SSC_RCMR) Compare 0 */
	#define SSC_RSTOP				(0x1u << 12)		/**< \brief (SSC_RCMR) Receive Stop Selection */
	#define SSC_RSTTDLY(v)			(((v)&0xFF) << 16)	/**< \brief (SSC_RCMR) Receive Start Delay */
	#define SSC_RPERIOD(v)			(((v)&0xFF) << 24)	/**< \brief (SSC_RCMR) Receive Period Divider Selection */

	/* -------- SSC_RFMR : (SSC Offset: 0x14) Receive Frame Mode Register -------- */
	#define SSC_RDATLEN(v)			((((v)-1)&0x1F) << 0)	/**< \brief (SSC_RFMR) Data Length */
	#define SSC_RLOOP				(0x1u << 5)				/**< \brief (SSC_RFMR) Loop Mode */
	#define SSC_RMSBF				(0x1u << 7)				/**< \brief (SSC_RFMR) Most Significant Bit First */
	#define SSC_RDATNB(v)			(((v)&0xF) << 8)		/**< \brief (SSC_RFMR) Data Number per Frame */
	#define SSC_RFSLEN(v)			(((v)&0xF) << 16)		/**< \brief (SSC_RFMR) Receive Frame Sync Length */
	#define	SSC_RFSOS_NONE			(0x0u << 20)			/**< \brief (SSC_RFMR) None, RF pin is an input */
	#define	SSC_RFSOS_NEGATIVE		(0x1u << 20)			/**< \brief (SSC_RFMR) Negative Pulse, RF pin is an output */
	#define	SSC_RFSOS_POSITIVE		(0x2u << 20)			/**< \brief (SSC_RFMR) Positive Pulse, RF pin is an output */
	#define	SSC_RFSOS_LOW			(0x3u << 20)			/**< \brief (SSC_RFMR) Driven Low during data transfer, RF pin is an output */
	#define	SSC_RFSOS_HIGH			(0x4u << 20)			/**< \brief (SSC_RFMR) Driven High during data transfer, RF pin is an output */
	#define	SSC_RFSOS_TOGGLING		(0x5u << 20)			/**< \brief (SSC_RFMR) Toggling at each start of data transfer, RF pin is an output */
	#define	SSC_RFSEDGE_POSITIVE	(0x0u << 24)			/**< \brief (SSC_RFMR) Positive Edge Detection */
	#define	SSC_RFSEDGE_NEGATIVE	(0x1u << 24)			/**< \brief (SSC_RFMR) Negative Edge Detection */
	#define SSC_RFSLEN_EXT(v)		(((v)&0xF) << 28)		/**< \brief (SSC_RFMR) FSLEN Field Extension */

	/* -------- SSC_TCMR : (SSC Offset: 0x18) Transmit Clock Mode Register -------- */
	#define	SSC_TCKS_MCK			(0x0u << 0)				/**< \brief (SSC_TCMR) Divided Clock */
	#define	SSC_TCKS_RK				(0x1u << 0)				/**< \brief (SSC_TCMR) RK Clock signal */
	#define	SSC_TCKS_TK				(0x2u << 0)				/**< \brief (SSC_TCMR) TK pin */
	#define	SSC_TCKO_NONE			(0x0u << 2)				/**< \brief (SSC_TCMR) None, TK pin is an input */
	#define	SSC_TCKO_CONTINUOUS		(0x1u << 2)				/**< \brief (SSC_TCMR) Continuous Transmit Clock, TK pin is an output */
	#define	SSC_TCKO_TRANSFER		(0x2u << 2)				/**< \brief (SSC_TCMR) Transmit Clock only during data transfers, TK pin is an output */
	#define SSC_TCKI				(0x1u << 5)				/**< \brief (SSC_TCMR) Transmit Clock Inversion */
	#define	SSC_TCKG_CONTINUOUS		(0x0u << 6)				/**< \brief (SSC_TCMR) None */
	#define	SSC_TCKG_EN_TF_LOW		(0x1u << 6)				/**< \brief (SSC_TCMR) Transmit Clock enabled only if TF Low */
	#define	SSC_TCKG_EN_TF_HIGH		(0x2u << 6)				/**< \brief (SSC_TCMR) Transmit Clock enabled only if TF High */
	#define	SSC_TSTART_CONTINUOUS	(0x0u << 8)				/**< \brief (SSC_TCMR) Continuous, as soon as a word is written in the SSC_THR Register (if Transmit is enabled), and immediately after the end of transfer of the previous data */
	#define	SSC_TSTART_RECEIVE		(0x1u << 8)				/**< \brief (SSC_TCMR) Receive start */
	#define	SSC_TSTART_TF_LOW		(0x2u << 8)				/**< \brief (SSC_TCMR) Detection of a low level on TF signal */
	#define	SSC_TSTART_TF_HIGH		(0x3u << 8)				/**< \brief (SSC_TCMR) Detection of a high level on TF signal */
	#define	SSC_TSTART_TF_FALLING	(0x4u << 8)				/**< \brief (SSC_TCMR) Detection of a falling edge on TF signal */
	#define	SSC_TSTART_TF_RISING	(0x5u << 8)				/**< \brief (SSC_TCMR) Detection of a rising edge on TF signal */
	#define	SSC_TSTART_TF_LEVEL		(0x6u << 8)				/**< \brief (SSC_TCMR) Detection of any level change on TF signal */
	#define	SSC_TSTART_TF_EDGE		(0x7u << 8)				/**< \brief (SSC_TCMR) Detection of any edge on TF signal */
	#define SSC_TSTTDLY(v)			(((v)&0xFF) << 16)		/**< \brief (SSC_TCMR) Transmit Start Delay */
	#define SSC_TPERIOD(v)			(((v)&0xFF) << 24)		/**< \brief (SSC_TCMR) Transmit Period Divider Selection */
	
	/* -------- SSC_TFMR : (SSC Offset: 0x1C) Transmit Frame Mode Register -------- */
	#define SSC_TDATLEN(v)			((((v)-1)&0x1F) << 0)	/**< \brief (SSC_TFMR) Data Length */
	#define SSC_TDATDEF				(0x1u << 5)				/**< \brief (SSC_TFMR) Data Default Value */
	#define SSC_TMSBF				(0x1u << 7)				/**< \brief (SSC_TFMR) Most Significant Bit First */
	#define SSC_TDATNB(v)			(((v)&0xF) << 8)		/**< \brief (SSC_TFMR) Data Number per frame */
	#define SSC_TFSLEN(v)			(((v)&0xF) << 16)		/**< \brief (SSC_TFMR) Transmit Frame Sync Length */
	#define	SSC_TFSOS_NONE			(0x0u << 20)			/**< \brief (SSC_TFMR) None, RF pin is an input */
	#define	SSC_TFSOS_NEGATIVE		(0x1u << 20)			/**< \brief (SSC_TFMR) Negative Pulse, RF pin is an output */
	#define	SSC_TFSOS_POSITIVE		(0x2u << 20)			/**< \brief (SSC_TFMR) Positive Pulse, RF pin is an output */
	#define	SSC_TFSOS_LOW			(0x3u << 20)			/**< \brief (SSC_TFMR) Driven Low during data transfer */
	#define	SSC_TFSOS_HIGH			(0x4u << 20)			/**< \brief (SSC_TFMR) Driven High during data transfer */
	#define	SSC_TFSOS_TOGGLING		(0x5u << 20)			/**< \brief (SSC_TFMR) Toggling at each start of data transfer */
	#define SSC_TFSDEN				(0x1u << 23)			/**< \brief (SSC_TFMR) Frame Sync Data Enable */
	#define SSC_TFSEDGE				(0x1u << 24)			/**< \brief (SSC_TFMR) Frame Sync Edge Detection */
	#define	SSC_TFSEDGE_POSITIVE	(0x0u << 24)			/**< \brief (SSC_TFMR) Positive Edge Detection */
	#define	SSC_TFSEDGE_NEGATIVE	(0x1u << 24)			/**< \brief (SSC_TFMR) Negative Edge Detection */
	#define SSC_TFSLEN_EXT(v)		(((v)&0xF) << 28)		/**< \brief (SSC_TFMR) FSLEN Field Extension */
	
	/* -------- SSC_SR : (SSC Offset: 0x40) Status Register -------- */
	#define SSC_TXRDY				(0x1u << 0)				/**< \brief (SSC_SR) Transmit Ready */
	#define SSC_TXEMPTY				(0x1u << 1)				/**< \brief (SSC_SR) Transmit Empty */
	#define SSC_ENDTX				(0x1u << 2)				/**< \brief (SSC_SR) End of Transmission */
	#define SSC_TXBUFE				(0x1u << 3)				/**< \brief (SSC_SR)  */
	#define SSC_RXRDY				(0x1u << 4)				/**< \brief (SSC_SR) Receive Ready */
	#define SSC_OVRUN				(0x1u << 5)				/**< \brief (SSC_SR) Receive Overrun */
	#define SSC_ENDRX				(0x1u << 6)				/**< \brief (SSC_SR) End of Reception */
	#define SSC_RXBUFF				(0x1u << 7)				/**< \brief (SSC_SR)  */
	#define SSC_CP0					(0x1u << 8)				/**< \brief (SSC_SR) Compare 0 */
	#define SSC_CP1					(0x1u << 9)				/**< \brief (SSC_SR) Compare 1 */
	#define SSC_TXSYN				(0x1u << 10)			/**< \brief (SSC_SR) Transmit Sync */
	#define SSC_RXSYN				(0x1u << 11)			/**< \brief (SSC_SR) Receive Sync */
	#define SSC_SR_TXEN				(0x1u << 16)			/**< \brief (SSC_SR) Transmit Enable */
	#define SSC_SR_RXEN				(0x1u << 17)			/**< \brief (SSC_SR) Receive Enable */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

}


namespace HW
{
	namespace PID
	{
		enum {	MKPID(SUPC, 0),		MKPID(RSTC, 1),		MKPID(RTC, 2),		MKPID(RTT, 3),		MKPID(WDT, 4),		MKPID(PMC, 5),		MKPID(EEFC0, 6),	MKPID(EEFC1, 7),
				MKPID(UART0, 8),	MKPID(UART1, 9),	MKPID(SMC, 10),		MKPID(PIOA, 11),	MKPID(PIOB, 12),	MKPID(PIOC, 13),	MKPID(USART0, 14),	MKPID(USART1, 15),
				MKPID(HSMCI, 18),	MKPID(TWI0, 19),	MKPID(TWI1, 20),	MKPID(SPI, 21),		MKPID(SSC, 22),		MKPID(TC0, 23),		MKPID(TC1, 24),		MKPID(TC2, 25),		MKPID(TC3, 26),		MKPID(TC4, 27), 
				MKPID(TC5, 28),		MKPID(ADC, 29),		MKPID(DACC, 30),	MKPID(PWM, 31),		MKPID(CRCCU, 32),	MKPID(ACC, 33),		MKPID(UDP, 34) };
	}

//	MK_PTR(SMC, 	0x400E0000);

	MK_PTR(MATRIX,	0x400E0200);
	MK_PTR(PMC,		0x400E0400);
	MK_PTR(UART0,	0x400E0600);
	MK_PTR(UART1,	0x400E0800);

	MK_PTR(EFC0,	0x400E0A00);
	MK_PTR(EFC1,	0x400E0C00);
	MK_PTR(PIOA,	0x400E0E00);
	MK_PTR(PIOB,	0x400E1000);
	MK_PTR(PIOC,	0x400E1200);

	MK_PTR(RSTC,	0X400E1400);
	MK_PTR(SUPC,	0X400E1410);
	MK_PTR(RTT,		0X400E1430);
	MK_PTR(WDT,		0X400E1450);
	MK_PTR(RTC,		0X400E1460);

	MK_PTR(SPI,		0X40008000);

	MK_PTR(TC0,		0X40010000);
	MK_PTR(TC1,		0X40014000);

//	MK_PTR(PWM,		0X40094000);

	MK_PTR(USART0,	0x40024000);
	MK_PTR(USART1,	0x40028000);

	MK_PTR(ADC,		0X40038000);

	MK_PTR(CMCC,	0X4007C000);

	MK_PTR(TWI0,	0x40018000);
	MK_PTR(TWI1,	0x4001C000);

	MK_PTR(CRCCU,	0x40044000);

	MK_PTR(SSC,		0x40004000);


	MK_PTR(USIC0,	SPI);
	MK_PTR(USIC1,	TWI0);
	MK_PTR(USIC2,	TWI1);
	MK_PTR(USIC3,	USART0);
	MK_PTR(USIC4,	USART1);
	MK_PTR(USIC5,	UART0);
	MK_PTR(USIC6,	UART1);


	inline void ResetWDT() { WDT->Reset(); }

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	__forceinline u32 ReadMem32(volatile const void *p) 
	{
		u32 r0;

		__asm { LDR r0, [p] }

		return r0;
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	inline bool RamCheck(const void *ptr)
	{
		u32 v = (u32)ptr;

		return (v >= 0x20000000 && v < 0x20028000);

	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	inline bool RomCheck(const void *ptr)
	{
		u32 v = (u32)ptr;

		return (v >= 0x400000 && v < 0x500000);

	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

} // namespace HW

extern T_HW::AT91_IHP VectorTableInt[16];
extern T_HW::AT91_IHP VectorTableExt[35];

#undef MK_PTR
#undef MKPID

#pragma pop

#endif // SAM4SA_H__27_08_2014__11_07
