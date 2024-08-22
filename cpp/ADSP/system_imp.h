#ifndef SYSTEM_IMP_H__14_11_2022__11_53
#define SYSTEM_IMP_H__14_11_2022__11_53

#include "core.h"

#ifdef __ADSPBF59x__ //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void InitIVG(u32 IVG, u32 PID, void (*EVT)())
{
	if (IVG <= 15)
	{
		*(pEVT0 + IVG) = (void*)EVT;
		*pIMASK |= 1<<IVG; 

		if (IVG > 6)
		{
			IVG -= 7;

			byte n = PID/8;
			byte i = (PID&7)*4;

			pSIC_IAR0[n] = (pSIC_IAR0[n] & ~(0xF<<i)) | (IVG<<i);

			*pSIC_IMASK |= 1<<PID;
		};
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void Init_PLL()
{
	u32 SIC_IWR1_reg;                /* backup SIC_IWR1 register */

	/* use Blackfin ROM SysControl() to change the PLL */
    ADI_SYSCTRL_VALUES sysctrl = {	VRCTL_VALUE,
									PLLCTL_VALUE,		/* (25MHz CLKIN x (MSEL=16))::CCLK = 400MHz */
									PLLDIV_VALUE,		/* (400MHz/(SSEL=4))::SCLK = 100MHz */
									PLLLOCKCNT_VALUE,
									PLLSTAT_VALUE };

	/* use the ROM function */
	bfrom_SysControl( SYSCTRL_WRITE | SYSCTRL_PLLCTL | SYSCTRL_PLLDIV, &sysctrl, 0);

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void LowLevelInit()
{
	Init_PLL();
																				
	*pPORTF_MUX			= INIT_PORTF_MUX		;	
	*pPORTG_MUX			= INIT_PORTG_MUX		;	
						  
	*pPORTF_FER 		= INIT_PORTF_FER 		;	
	*pPORTG_FER 		= INIT_PORTG_FER 		;	
						  
	*pPORTFIO_DIR 		= INIT_PORTFIO_DIR 		;		
	*pPORTGIO_DIR 		= INIT_PORTGIO_DIR 		;		
						  
	*pPORTFIO_INEN 		= INIT_PORTFIO_INEN 	;	
	*pPORTGIO_INEN 		= INIT_PORTGIO_INEN 	;	
						  
	*pPORTGIO 			= INIT_PORTGIO 			;
	*pPORTFIO 			= INIT_PORTFIO 			;
						  
	*pPORTFIO_POLAR		= INIT_PORTFIO_POLAR	;
	*pPORTFIO_EDGE		= INIT_PORTFIO_EDGE 	;
	*pPORTFIO_BOTH		= INIT_PORTFIO_BOTH 	;
	*pPORTFIO_MASKA 	= INIT_PORTFIO_MASKA	;
	*pPORTFIO_MASKB 	= INIT_PORTFIO_MASKB	;
						  
	*pPORTGIO_POLAR		= INIT_PORTGIO_POLAR	;
	*pPORTGIO_EDGE 		= INIT_PORTGIO_EDGE 	;
	*pPORTGIO_BOTH 		= INIT_PORTGIO_BOTH 	;
	*pPORTGIO_MASKA 	= INIT_PORTGIO_MASKA	;
	*pPORTGIO_MASKB 	= INIT_PORTGIO_MASKB	;

	*pWDOG_CNT 			= INIT_WDOG_CNT;
	*pWDOG_CTL 			= INIT_WDOG_CTL;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#elif defined(__ADSPBF70x__) //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32 Get_CCLK_MHz()	{ return CCLK_MHz;	}
u32 Get_SCLK_MHz()	{ return SCLK_MHz;	}
u32 Get_SCLK0_MHz()	{ return SCLK0_MHz; }
u32 Get_SCLK1_MHz()	{ return SCLK1_MHz; }

u32 Get_CCLK()	{ return CCLK;	}
u32 Get_SCLK()	{ return SCLK;	}
u32 Get_SCLK0()	{ return SCLK0; }
u32 Get_SCLK1()	{ return SCLK1; }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void InitIVG(u32 IVG, void (*EVT)())
{
	if (IVG <= 15)
	{
		HW::ICU->Evt[IVG]	 = (void*)EVT;		// *(pEVT0 + IVG) = (void*)EVT;
		HW::ICU->IMask		|= 1<<IVG;			// *pIMASK |= 1<<IVG; 
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

extern "C" T_HW::S_SEC::EVT SEC_VecTable[PARAM_SEC0_SCOUNT];

void InitSEC(u32 PID, void (*EVT)(), byte prio)
{
	if (PID < PARAM_SEC0_SCOUNT)
	{
		SEC_VecTable[PID]		= EVT;
		HW::SEC->SSI[PID].SCTL	= SEC_IEN|SEC_SEN|SEC_PRIO(prio);
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void Init_PLL()
{
	u32	ctl			= CGUCTL_VALUE;
	u32 curr_ctl	= HW::CGU->CTL & (CGU_CTL_MSEL(~0)|CGU_CTL_DF);

	if (curr_ctl == ctl)
	{
      
		while(HW::CGU->STAT & CGU_STAT_CLKSALGN); // Verify that the CGU_STAT.CLKSALGN bit=0 (clocks aligned).

		HW::CGU->DIV = CGU_DIV_UPDT | CGU_DIV_OSEL(OCLK_DIV) | CGU_DIV_DSEL(DCLK_DIV) | CGU_DIV_S1SEL(SCLK1_DIV) | CGU_DIV_S0SEL(SCLK0_DIV) | CGU_DIV_SYSSEL(SCLK_DIV) | CGU_DIV_CSEL(CCLK_DIV);
		
		while(HW::CGU->STAT & CGU_STAT_CLKSALGN); // Poll the CGU_STAT.CLKSALGN bit till it is 1 when clocks are aligned indicating the end of the update sequence.

	}
	else
	{
		while((HW::CGU->STAT & (CGU_STAT_PLLEN|CGU_STAT_PLOCK|CGU_STAT_CLKSALGN)) != (CGU_STAT_PLLEN|CGU_STAT_PLOCK));

		HW::CGU->DIV = CGU_DIV_OSEL(OCLK_DIV) | CGU_DIV_DSEL(DCLK_DIV) | CGU_DIV_S1SEL(SCLK1_DIV) | CGU_DIV_S0SEL(SCLK0_DIV) | CGU_DIV_SYSSEL(SCLK_DIV) | CGU_DIV_CSEL(CCLK_DIV);

		HW::CGU->CTL = ctl;

		while((HW::CGU->STAT & (CGU_STAT_PLOCK|CGU_STAT_PLLBP|CGU_STAT_CLKSALGN)) != CGU_STAT_PLOCK);
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void Init_SEC()
{
	HW::SEC->SCI.CCTL = SEC_RESET;

	HW::SEC->SCI.CSTAT = ~0; //SEC_NMI;

	HW::SEC->GCTL = SEC_RESET;

	HW::SEC->FCTL = SEC_RESET;

	HW::SEC->GCTL = SEC_EN;

	//HW::SEC->SCI.CCTL = SEC_NMIEN;

	HW::SEC->SCI.CCTL = SEC_EN;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

section ("L1_code")

static void Init_Cache()
{
	int l1_code_cache;
	int l1_data_cache_a;
	int l1_data_cache_b;

	asm (	".extern ___l1_code_cache;\n	.type ___l1_code_cache, STT_OBJECT;\n	%0 = ___l1_code_cache (Z);"		: "=d" (l1_code_cache)		: : );						
	asm (	".extern ___l1_data_cache_a;\n	.type ___l1_data_cache_a, STT_OBJECT;\n	%0 = ___l1_data_cache_a (Z);"	: "=d" (l1_data_cache_a)	: : );
	asm (	".extern ___l1_data_cache_b;\n	.type ___l1_data_cache_b, STT_OBJECT;\n	%0 = ___l1_data_cache_b (Z);"	: "=d" (l1_data_cache_b)	: : );

	if (l1_code_cache != 0)
	{
		ssync();

		HW::L1IM->Ictl = L1IM_CFG;
		HW::L1IM->Icplb_dflt = L1IM_L1UREAD|L1IM_L1EOM|L1IM_SYSUREAD|L1IM_SYSEOM|L1IM_DFLT_NOCACHE;

		byte n = 0;
		HW::L1IM->Icplb_addr[n] = 0x11A00000; HW::L1IM->Icplb_data[n++] = L1IM_PSIZE_16KB |L1IM_NOCACHE|L1IM_CPRIO_LO|L1IM_UREAD|L1IM_LOCK|L1IM_VALID;	// L1 Code SRAM (1024 KB)
		HW::L1IM->Icplb_addr[n] = 0x11A04000; HW::L1IM->Icplb_data[n++] = L1IM_PSIZE_16KB |L1IM_NOCACHE|L1IM_CPRIO_LO|L1IM_UREAD|L1IM_LOCK|L1IM_VALID;	// L1 Code SRAM (1024 KB)
		HW::L1IM->Icplb_addr[n] = 0x11A08000; HW::L1IM->Icplb_data[n++] = L1IM_PSIZE_16KB |L1IM_NOCACHE|L1IM_CPRIO_LO|L1IM_UREAD|L1IM_LOCK|L1IM_VALID;	// L1 Code SRAM (1024 KB)
		HW::L1IM->Icplb_addr[n] = 0x04000000; HW::L1IM->Icplb_data[n++] = L1IM_PSIZE_256KB|L1IM_L1CACHE|L1IM_CPRIO_LO|L1IM_UREAD|L1IM_LOCK|L1IM_VALID;	// L2 ROM
		HW::L1IM->Icplb_addr[n] = 0x04040000; HW::L1IM->Icplb_data[n++] = L1IM_PSIZE_256KB|L1IM_L1CACHE|L1IM_CPRIO_LO|L1IM_UREAD|L1IM_LOCK|L1IM_VALID;	// L2 ROM
		HW::L1IM->Icplb_addr[n] = 0x08000000; HW::L1IM->Icplb_data[n++] = L1IM_PSIZE_1MB  |L1IM_L1CACHE|L1IM_CPRIO_HI|L1IM_UREAD|L1IM_LOCK|L1IM_VALID;	// L2 SRAM (1024 KB)

		for ( ; n < 16; n++) HW::L1IM->Icplb_data[n] = 0;

		csync();

		HW::L1IM->Ictl |= L1IM_ENCPLB;

		ssync();
	}
	else
	{
		ssync();

		HW::L1IM->Ictl = L1IM_CBYPASS;
		HW::L1IM->Icplb_dflt = L1IM_DFLT_NOCACHE;

		byte n = 0;
		HW::L1IM->Icplb_addr[n] = 0x11A00000; HW::L1IM->Icplb_data[n++] = L1IM_PSIZE_64KB |L1IM_NOCACHE|L1IM_CPRIO_LO|L1IM_UREAD|L1IM_LOCK|L1IM_VALID; // L1 Code SRAM (64 KB)
		HW::L1IM->Icplb_addr[n] = 0x04000000; HW::L1IM->Icplb_data[n++] = L1IM_PSIZE_256KB|L1IM_NOCACHE|L1IM_CPRIO_LO|L1IM_UREAD|L1IM_LOCK|L1IM_VALID; // L2 ROM
		HW::L1IM->Icplb_addr[n] = 0x04040000; HW::L1IM->Icplb_data[n++] = L1IM_PSIZE_256KB|L1IM_NOCACHE|L1IM_CPRIO_LO|L1IM_UREAD|L1IM_LOCK|L1IM_VALID; // L2 ROM
		HW::L1IM->Icplb_addr[n] = 0x08000000; HW::L1IM->Icplb_data[n++] = L1IM_PSIZE_1MB  |L1IM_NOCACHE|L1IM_CPRIO_HI|L1IM_UREAD|L1IM_LOCK|L1IM_VALID; // L2 SRAM (1024 KB)

		for ( ; n < 16; n++) HW::L1IM->Icplb_data[n] = 0;

		csync();

		HW::L1IM->Ictl |= L1IM_ENCPLB;

		ssync();
	};

	if (l1_data_cache_a != 0 || l1_data_cache_b != 0)
	{
		ssync();

		HW::L1DM->DCTL			= (l1_data_cache_b) ? (L1DM_ACACHE_BCACHE) : ((l1_data_cache_a) ? L1DM_ACACHE_BSRAM : L1DM_SRAM_AB);
		HW::L1DM->DCPLB_DFLT	= L1DM_DFLT_L1EOM|L1DM_DFLT_SYSEOM|L1DM_DFLT_NOCACHE;

		const u32 DCPLB_L2_ROM		= L1DM_PSIZE_256KB	| L1DM_WB_L1	| L1DM_DIRTY |								L1DM_UREAD | L1DM_LOCK | L1DM_VALID;
		const u32 DCPLB_L2_SRAM	= L1DM_PSIZE_1MB	| L1DM_WB_L1	| L1DM_DIRTY | L1DM_SWRITE | L1DM_UWRITE |	L1DM_UREAD | L1DM_LOCK | L1DM_VALID;
		const u32 DCPLB_L1_SRAM	= L1DM_PSIZE_16KB	| L1DM_NOCACHE	| L1DM_DIRTY | L1DM_SWRITE | L1DM_UWRITE |	L1DM_UREAD | L1DM_LOCK | L1DM_VALID;
		const u32 DCPLB_MMR		= L1DM_PSIZE_16MB	| L1DM_IO		| L1DM_DIRTY | L1DM_SWRITE | L1DM_UWRITE |	L1DM_UREAD | L1DM_LOCK | L1DM_VALID;
		const u32 DCPLB_OTP		= L1DM_PSIZE_1KB	| L1DM_NOCACHE	| L1DM_DIRTY |								L1DM_UREAD | L1DM_LOCK | L1DM_VALID;
		const u32 DCPLB_SPI2		= L1DM_PSIZE_64MB	| L1DM_WB_L1	| L1DM_DIRTY |								L1DM_UREAD | L1DM_LOCK | L1DM_VALID;

		byte n = 0;

									HW::L1DM->DCPLB_ADDR[n] = 0x04000000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_L2_ROM;	// L2 ROM
									HW::L1DM->DCPLB_ADDR[n] = 0x04040000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_L2_ROM;	// L2 ROM
									HW::L1DM->DCPLB_ADDR[n] = 0x08000000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_L2_SRAM;	// L2 SRAM (1024 KB)
									HW::L1DM->DCPLB_ADDR[n] = 0x11800000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_L1_SRAM;	// L1 Data Block A SRAM (16 KB)
		if (l1_data_cache_a == 0)	HW::L1DM->DCPLB_ADDR[n] = 0x11804000, HW::L1DM->DCPLB_DATA[n++] = DCPLB_L1_SRAM;	// L1 Data Block A SRAM/Cache (16 KB)
									HW::L1DM->DCPLB_ADDR[n] = 0x11900000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_L1_SRAM;	// L1 Data Block B SRAM (16 KB)
		if (l1_data_cache_b == 0)	HW::L1DM->DCPLB_ADDR[n] = 0x11904000, HW::L1DM->DCPLB_DATA[n++] = DCPLB_L1_SRAM;	// L1 Data Block B SRAM/Cache (16 KB)
									HW::L1DM->DCPLB_ADDR[n] = 0x11B00000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_L1_SRAM;	// L1 Data Block C SRAM (8 KB)
									HW::L1DM->DCPLB_ADDR[n] = 0x1FC00000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_MMR;		// Core MMR (4MB), System MMR (3MB), STM Memory (4 KB)
									HW::L1DM->DCPLB_ADDR[n] = 0x38000000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_OTP;		// OTP Memory (1 KB)
									HW::L1DM->DCPLB_ADDR[n] = 0x40000000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_SPI2;		// SPI2 Memory (64 MB)
									HW::L1DM->DCPLB_ADDR[n] = 0x44000000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_SPI2;		// SPI2 Memory (64 MB)


		for ( ; n < 16; n++) HW::L1DM->DCPLB_DATA[n] = 0;

		csync();

		HW::L1DM->DCTL |= L1DM_ENCPLB;

		ssync();
	}
	else
	{
		ssync();

		HW::L1DM->DCTL			= L1DM_SRAM_AB | L1DM_CBYPASS;
		HW::L1DM->DCPLB_DFLT	= L1DM_DFLT_NOCACHE;

		const u32 DCPLB_L2_ROM		= L1DM_PSIZE_256KB	| L1DM_WB_L1	| L1DM_DIRTY |								L1DM_UREAD | L1DM_LOCK | L1DM_VALID;
		const u32 DCPLB_L2_SRAM	= L1DM_PSIZE_1MB	| L1DM_WB_L1	| L1DM_DIRTY | L1DM_SWRITE | L1DM_UWRITE |	L1DM_UREAD | L1DM_LOCK | L1DM_VALID;
		const u32 DCPLB_L1_SRAM	= L1DM_PSIZE_16KB	| L1DM_NOCACHE	| L1DM_DIRTY | L1DM_SWRITE | L1DM_UWRITE |	L1DM_UREAD | L1DM_LOCK | L1DM_VALID;
		const u32 DCPLB_L1C_SRAM	= L1DM_PSIZE_4KB	| L1DM_NOCACHE	| L1DM_DIRTY | L1DM_SWRITE | L1DM_UWRITE |	L1DM_UREAD | L1DM_LOCK | L1DM_VALID;
		const u32 DCPLB_MMR		= L1DM_PSIZE_16MB	| L1DM_IO		| L1DM_DIRTY | L1DM_SWRITE | L1DM_UWRITE |	L1DM_UREAD | L1DM_LOCK | L1DM_VALID;
		const u32 DCPLB_OTP		= L1DM_PSIZE_1KB	| L1DM_NOCACHE	| L1DM_DIRTY |								L1DM_UREAD | L1DM_LOCK | L1DM_VALID;
		const u32 DCPLB_SPI2		= L1DM_PSIZE_64MB	| L1DM_WB_L1	| L1DM_DIRTY |								L1DM_UREAD | L1DM_LOCK | L1DM_VALID;

		byte n = 0;

		HW::L1DM->DCPLB_ADDR[n] = 0x04000000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_L2_ROM;	// L2 ROM
		HW::L1DM->DCPLB_ADDR[n] = 0x04040000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_L2_ROM;	// L2 ROM
		HW::L1DM->DCPLB_ADDR[n] = 0x08000000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_L2_SRAM;	// L2 SRAM (1024 KB)
		HW::L1DM->DCPLB_ADDR[n] = 0x11800000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_L1_SRAM;	// L1 Data Block A SRAM (16 KB)
		HW::L1DM->DCPLB_ADDR[n] = 0x11804000, HW::L1DM->DCPLB_DATA[n++] = DCPLB_L1_SRAM;	// L1 Data Block A SRAM/Cache (16 KB)
		HW::L1DM->DCPLB_ADDR[n] = 0x11900000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_L1_SRAM;	// L1 Data Block B SRAM (16 KB)
		HW::L1DM->DCPLB_ADDR[n] = 0x11904000, HW::L1DM->DCPLB_DATA[n++] = DCPLB_L1_SRAM;	// L1 Data Block B SRAM/Cache (16 KB)
		HW::L1DM->DCPLB_ADDR[n] = 0x11B00000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_L1C_SRAM;	// L1 Data Block C SRAM (8 KB)
		HW::L1DM->DCPLB_ADDR[n] = 0x11B01000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_L1C_SRAM;	// L1 Data Block C SRAM (8 KB)
		HW::L1DM->DCPLB_ADDR[n] = 0x1FC00000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_MMR;		// Core MMR (4MB), System MMR (3MB), STM Memory (4 KB)
		HW::L1DM->DCPLB_ADDR[n] = 0x38000000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_OTP;		// OTP Memory (1 KB)
		HW::L1DM->DCPLB_ADDR[n] = 0x40000000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_SPI2;		// SPI2 Memory (64 MB)
		HW::L1DM->DCPLB_ADDR[n] = 0x44000000; HW::L1DM->DCPLB_DATA[n++] = DCPLB_SPI2;		// SPI2 Memory (64 MB)

		for ( ; n < 16; n++) HW::L1DM->DCPLB_DATA[n] = 0;

		csync();

		HW::L1DM->DCTL |= L1DM_ENCPLB;

		ssync();
	};

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

extern "C" void SystemInit()
{
	Init_PLL();
			
	HW::PIOA->MUX		= INIT_PORTA_MUX;	
	HW::PIOB->MUX		= INIT_PORTB_MUX;	
	HW::PIOC->MUX		= INIT_PORTC_MUX;	
						  
	HW::PIOA->FER 		= INIT_PORTA_FER;	
	HW::PIOB->FER 		= INIT_PORTB_FER;	
	HW::PIOC->FER 		= INIT_PORTC_FER;	
						  
	HW::PIOA->DIR 		= INIT_PORTA_DIR;		
	HW::PIOB->DIR 		= INIT_PORTB_DIR;		
	HW::PIOC->DIR 		= INIT_PORTC_DIR;		
						  
	HW::PIOA->INEN 		= INIT_PORTA_INEN;	
	HW::PIOB->INEN 		= INIT_PORTB_INEN;	
	HW::PIOC->INEN 		= INIT_PORTC_INEN;	
		
	HW::PIOA->DATA		= INIT_PORTA_DATA;
	HW::PIOB->DATA		= INIT_PORTB_DATA;
	HW::PIOC->DATA		= INIT_PORTC_DATA;

	HW::PINT0->MSK_CLR	= ~0;
	HW::PINT1->MSK_CLR	= ~0;
	HW::PINT2->MSK_CLR	= ~0;
	HW::PADS->PCFG0		= 0;

	HW::WDOG->CNT 		= INIT_WDOG_CNT;
	HW::WDOG->CTL 		= INIT_WDOG_CTL;

	Init_Cache();

	Init_SEC();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



#endif //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // SYSTEM_IMP_H__11_10_2022__18_02
