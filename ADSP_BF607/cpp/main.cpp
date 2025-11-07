#define REAL_TIME_CLOCK_DISABLE
#define SYSTEM_TICK_TIMER_DISABLE

#include "core.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	#define INIT_PORTA_MUX				0x0000						
	#define INIT_PORTB_MUX				0x0000						
	#define INIT_PORTC_MUX				0x0000						
	#define INIT_PORTD_MUX				0x0000						
	#define INIT_PORTE_MUX				0x0000						
	#define INIT_PORTF_MUX				0x0000						
	#define INIT_PORTG_MUX				0x0000						

	#define INIT_PORTA_FER 				0x0000						
	#define INIT_PORTB_FER 				0x0000						
	#define INIT_PORTC_FER 				0x0000						
	#define INIT_PORTD_FER 				0x0000						
	#define INIT_PORTE_FER 				0x0000						
	#define INIT_PORTF_FER 				0x0000						
	#define INIT_PORTG_FER 				0x0000						

	#define INIT_PORTA_DIR 				0x0000
	#define INIT_PORTB_DIR 				0x0000
	#define INIT_PORTC_DIR 				0x0000
	#define INIT_PORTD_DIR 				0x0000
	#define INIT_PORTE_DIR 				0x0000
	#define INIT_PORTF_DIR 				0x0000
	#define INIT_PORTG_DIR 				0x0000

	#define INIT_PORTA_INEN 			0x0000		
	#define INIT_PORTB_INEN 			0x0000		
	#define INIT_PORTC_INEN 			0x0000		
	#define INIT_PORTD_INEN 			0x0000		
	#define INIT_PORTE_INEN 			0x0000		
	#define INIT_PORTF_INEN 			0x0000		
	#define INIT_PORTG_INEN 			0x0000		

	#define INIT_PORTA_DATA 			0x0000
	#define INIT_PORTB_DATA 			0x0000
	#define INIT_PORTC_DATA 			0x0000
	#define INIT_PORTD_DATA 			0x0000
	#define INIT_PORTE_DATA 			0x0000
	#define INIT_PORTF_DATA 			0x0000
	#define INIT_PORTG_DATA 			0x0000

	#define INIT_PADS_EMAC_PTP_CLKSEL	0
	#define INIT_PADS_PORTS_HYST		0
	#define INIT_PADS_TWI_VSEL			0

	#ifndef __DEBUG
	#define INIT_WDOG_CNT			MS2SCLK0(100)
	#define INIT_WDOG_CTL			WDOG_WDEN
	#else
	#define INIT_WDOG_CNT			MS2SCLK0(100)
	#define INIT_WDOG_CTL			WDOG_WDDIS
	#endif



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


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "ADSP\system_imp.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void main()
{
	PIO_MAINLOOP->DirSet(MAINLOOP);

	while (1)
	{


		PIO_MAINLOOP->NOT(MAINLOOP);
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//#pragma section("CORE1_L1_CODE")
#pragma default_section(CODE, "CORE1_L1_CODE")

extern "C" void core1_main()
{
	PIO_TST->DirSet(TST);

	while (1)
	{
		PIO_TST->NOT(TST);
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
