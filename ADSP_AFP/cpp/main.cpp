#define REAL_TIME_CLOCK_DISABLE
#define SYSTEM_TICK_TIMER_DISABLE

#include "spi.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define FLASHSPI_IMP_V2

#define BAUD_RATE_DIVISOR 	3
#define FLASH_START_ADR		0x00000000
#define BUFFER_SIZE			0x2000
#define BAUD_RATE			10000000

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma retain_name
char build_date[] __attribute__ ((used)) = "\n" "ADSP_AFP" "\n" __DATE__ "\n" __TIME__ "\n";

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef __ADSPBF59x__

	#define PIO_MAINLOOP	HW::PIOF
	#define PIO_TST			HW::PIOG

	#define PIN_MAINLOOP	4
	#define PIN_TST			5

	#define MAINLOOP	(1UL<<PIN_MAINLOOP)
	#define TST			(1UL<<PIN_TST)		

	#ifndef SCLK_MHz
	#define SCLK_MHz	100
	#endif

	#ifndef CCLK_MHz
	#define CCLK_MHz	400
	#endif

#elif defined(__ADSPBF70x__)

	#define PIO_MAINLOOP	HW::PIOA
	#define PIO_TST			HW::PIOB

	#define PIN_MAINLOOP	13
	#define PIN_TST			5

	#define MAINLOOP	(1UL<<PIN_MAINLOOP)
	#define TST			(1UL<<PIN_TST)		

	#ifndef SCLK_MHz
	#define SCLK_MHz	100
	#endif

	#ifndef CCLK_MHz
	#define CCLK_MHz	400
	#endif

#elif defined(__ADSPBF60x__) //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	#define PIO_MAINLOOP	HW::PIOA
	#define PIO_TST			HW::PIOB

	#define PIN_MAINLOOP	4
	#define PIN_TST			10

	#define MAINLOOP	(1UL<<PIN_MAINLOOP)
	#define TST			(1UL<<PIN_TST)		

	#define CLKIN_MHz			25

	#define CLKIN_DIV			1	// 1, 2

	#define PLL_MUL				16	// 1...128
	#define CCLK_DIV			1   // 1...32
	#define SCLK_DIV			2	// 1...32
	#define SCLK0_DIV			1	// 1...8
	#define SCLK1_DIV			1	// 1...8
	#define DCLK_DIV			32	// 1...32	DRAM clock
	#define OCLK_DIV			1	// 1...128  Output clock

	#define VCO_CLK_MHz 		(CLKIN_MHz*PLL_MUL/CLKIN_DIV)
	#define CCLK_MHz			(VCO_CLK_MHz/CCLK_DIV)
	#define SCLK_MHz			(VCO_CLK_MHz/SCLK_DIV)
	#define SCLK0_MHz			(SCLK_MHz/SCLK0_DIV)
	#define SCLK1_MHz			(SCLK_MHz/SCLK1_DIV)

	#if (VCO_CLK_MHz < 250) || (VCO_CLK_MHz > 1000)
	#error VCO_CLK_MHz must be 250...1000
	#endif

	#if (CCLK_MHz > 500)
	#error CCLK_MHz must be <= 500
	#endif

	#if (SCLK_MHz > 250)
	#error SCLK_MHz must be <= 250
	#endif

	//#define VRCTL_VALUE         0x0000

	#if CLKIN_DIV == 2
	#define CGUCTL_VALUE        (CGU_CTL_MSEL(PLL_MUL)|CGU_CTL_DF)
	#else
	#define CGUCTL_VALUE        (CGU_CTL_MSEL(PLL_MUL))
	#endif

	#define CCLK (CCLK_MHz*1000000)
	#define SCLK (SCLK_MHz*1000000)
	#define SCLK0 (SCLK0_MHz*1000000)
	#define SCLK1 (SCLK1_MHz*1000000)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	#define MS2SCLK(x) ((u64)((x)*SCLK_MHz*1000))
	#define US2SCLK(x) ((u64)((x)*SCLK_MHz))
	#define NS2SCLK(x) ((u64)(((x)*SCLK_MHz+500)/1000))

	#define MS2SCLK0(x) ((u64)((x)*SCLK0_MHz*1000))
	#define US2SCLK0(x) ((u64)((x)*SCLK0_MHz))
	#define NS2SCLK0(x) ((u64)(((x)*SCLK0_MHz+500)/1000))

	#define MS2SCLK1(x) ((u64)((x)*SCLK1_MHz*1000))
	#define US2SCLK1(x) ((u64)((x)*SCLK1_MHz))
	#define NS2SCLK1(x) ((u64)(((x)*SCLK1_MHz+500)/1000))

	#define MS2CCLK(x) ((u64)((x)*CCLK_MHz*1000))
	#define US2CCLK(x) ((u64)((x)*CCLK_MHz))
	#define NS2CCLK(x) ((u64)(((x)*CCLK_MHz+500)/1000))

	#define MS2CLK(x) MS2SCLK(x)
	#define US2CLK(x) US2SCLK(x)
	#define NS2CLK(x) NS2SCLK(x)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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

#define INIT_PLL() Init_PLL()

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // #elif defined(__ADSPBF60x__) //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef SCLK
#define SCLK (SCLK_MHz*1000000)
#endif

#ifndef CCLK
#define CCLK (CCLK_MHz*1000000)
#endif

#ifndef MS2CCLK
#define MS2CCLK(x) ((u64)((x)*CCLK_MHz*1000))
#endif

#ifndef US2CCLK
#define US2CCLK(x) ((u64)((x)*CCLK_MHz))
#endif

#ifndef NS2CCLK
#define NS2CCLK(x) ((u64)(((x)*CCLK_MHz+500)/1000))
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//#define ADSP_CHECKFLASH
//#define ADSP_CRC_PROTECTION
#define FLASHSPI_WRITESYNC
//#define FLASHSPI_REQUESTUPDATE
#define FLASHSPI_AFP

//#define	NUM_SMALL_BUF	60      
//#define	NUM_MEDIUM_BUF	1
//#define	NUM_HUGE_BUF	1

//#define FLASH_IS25LP080D

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "DMA\DMA_imp.h"

#include "SPIM_imp.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef CPU_BF592

static u16 SPI_CS_MASK[] = { PF8 };

static S_SPIM	spi(0, HW::PIOF, SPI_CS_MASK, ArraySize(SPI_CS_MASK), SCLK);

#elif defined(CPU_BF706)

static u16 SPI_CS_MASK[] = { PB15 };

static S_SPIM	spi(2, HW::PIOB, SPI_CS_MASK, ArraySize(SPI_CS_MASK), SCLK);

#elif defined(CPU_BF607)

static SPI_DSC_CS spi_dsc_cs[1] = { { HW::PIOD, PD11, BAUD2SPI(BAUD_RATE), 0 } };

static S_SPIM	spi(0, spi_dsc_cs, ArraySize(spi_dsc_cs), SCLK, PD0|PD1|PD2|PD3|PD4);

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//#undef BOOT_TIMEOUT			
//#undef BOOT_MAIN_TIMEOUT	
//#define BOOT_TIMEOUT			(2000)
//#define BOOT_MAIN_TIMEOUT		(100000)

#define Pin_MainLoop_Tgl()		MAIN_LOOP_PIN_TGL()

#ifdef BOOT_COM

#include "Comport\ComPort_imp.h"

#ifdef CPU_BF592

static ComPort com;

#elif defined(CPU_BF706)

static ComPort com(1, PIO_RTS, PIN_RTS);

#endif

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "FLASH\FlashSPI_imp_v2.h"

FlashSPI spiFlash(spi);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "ADSP\AFP_imp_v2.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
