#include "core.h"

#include <bfrom.h>
#include <ccblkfn.h>
#include <sysreg.h> 

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define CLKIN_MHz			25

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef __ADSPBF59x__

#define CLKIN_DIV			2	// 1, 2

#define PLL_MUL				8	// 5...64
#define SCLK_DIV			1	// 1...15
#define CCLK_CSEL			0	// 0...3
#define CCLK_DIV			(1UL<<CCLK_CSEL)

#define VCO_CLK_MHz 		(CLKIN_MHz*PLL_MUL/CLKIN_DIV)
#define CCLK_MHz			VCO_CLK_MHz/CCLK_DIV
#define SCLK_MHz			VCO_CLK_MHz/SCLK_DIV

#define VRCTL_VALUE         0x0000

#if CLKIN_DIV == 2
#define PLLCTL_VALUE        (SET_MSEL(PLL_MUL)|DF)
#else
#define PLLCTL_VALUE        (SET_MSEL(PLL_MUL))
#endif

#define PLLDIV_VALUE        (SET_SSEL(SCLK_DIV))
#define PLLLOCKCNT_VALUE    0x0000
#define PLLSTAT_VALUE       0x0000
//#define RSICLK_DIV          0x0001


#define BAUD_RATE_DIVISOR 	5

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

section ("L1_code")

void initcode(ADI_BOOT_DATA* pBS)
{
    *pSIC_IWR0 = IRQ_PLL_WAKEUP;

	ADI_SYSCTRL_VALUES sysctrl = {	VRCTL_VALUE,
									PLLCTL_VALUE,		/* (25MHz CLKIN x (MSEL=16))::CCLK = 400MHz */
									PLLDIV_VALUE,		/* (400MHz/(SSEL=4))::SCLK = 100MHz */
									PLLLOCKCNT_VALUE,
									PLLSTAT_VALUE };


	bfrom_SysControl( SYSCTRL_WRITE | SYSCTRL_PLLCTL | SYSCTRL_PLLDIV, &sysctrl, 0);

	pBS->dFlags |= BFLAG_FASTREAD;
	*pSPI0_BAUD = BAUD_RATE_DIVISOR;
	pBS->dClock = BAUD_RATE_DIVISOR; /* required to keep dClock in pBS (-> ADI_BOOT_DATA) consistent */

    *pSIC_IWR0 = IWR0_ENABLE_ALL;

    sysreg_write(reg_LC0,0);
    sysreg_write(reg_LC1,0);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#elif defined(__ADSPBF70x__)

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define CLKIN_DIV			1	// 1, 2

#define PLL_MUL				16	// 1...128
#define CCLK_DIV			2   // 1...32
#define SCLK_DIV			2	// 1...32
#define SCLK0_DIV			1	// 1...8
#define SCLK1_DIV			1	// 1...8
#define DCLK_DIV			1	// 1...32	DRAM clock
#define OCLK_DIV			1	// 1...128  Output clock

#define VCO_CLK_MHz 		(CLKIN_MHz*PLL_MUL/CLKIN_DIV)
#define CCLK_MHz			VCO_CLK_MHz/CCLK_DIV
#define SCLK_MHz			VCO_CLK_MHz/SCLK_DIV

#if (VCO_CLK_MHz < 231) || (VCO_CLK_MHz > 800)
#error VCO_CLK_MHz must be 231...800
#endif

#if (CCLK_MHz > 400)
#error CCLK_MHz must be <= 400
#endif

#if (SCLK_MHz > 200)
#error SCLK_MHz must be <= 200
#endif

//#define VRCTL_VALUE         0x0000

#if CLKIN_DIV == 2
#define CGUCTL_VALUE        (CGU_CTL_MSEL(PLL_MUL)|CGU_CTL_DF)
#else
#define CGUCTL_VALUE        (CGU_CTL_MSEL(PLL_MUL))
#endif

//#define PLLDIV_VALUE        (SET_SSEL(SCLK_DIV))
//#define PLLLOCKCNT_VALUE    0x0000
//#define PLLSTAT_VALUE       0x0000

#define BAUD_RATE_DIVISOR 	5

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma retain_name /* retain resolved initcode entry */
section ("L1_code")

void initcode(ADI_ROM_BOOT_CONFIG* pBootStruct)
{
   /* Set the L1 instruction cache/SRAM block as SRAM */
	ssync();
	
	HW::L1IM->CTL &= ~(L1IM_ENCPLB|L1IM_CFG);
	
	ssync();

	HW::L1DM->CTL &= ~(L1DM_ENCPLB|L1DM_CFG);

	ssync();


   /* Check if the new MSEL is the same as current MSEL value. If it is, just
    * update the CGU_DIV register.
    */

	u32      ctl = CGUCTL_VALUE;
	u32 curr_ctl = HW::CGU->CTL & (CGU_CTL_MSEL(~0)|CGU_CTL_DF);

	if (curr_ctl == ctl)
	{
      
		while(HW::CGU->STAT & CGU_STAT_CLKSALGN); // Verify that the CGU_STAT.CLKSALGN bit=0 (clocks aligned).

		// Write the new CGU_DIV field values with the CGU_DIV.UPDT bit=1.

		HW::CGU->DIV = CGU_DIV_UPDT | CGU_DIV_OSEL(OCLK_DIV) | CGU_DIV_DSEL(DCLK_DIV) | CGU_DIV_S1SEL(SCLK1_DIV) | CGU_DIV_S0SEL(SCLK0_DIV) | CGU_DIV_SYSSEL(SCLK_DIV) | CGU_DIV_CSEL(CCLK_DIV);
		
		while(HW::CGU->STAT & CGU_STAT_CLKSALGN); // Poll the CGU_STAT.CLKSALGN bit till it is 1 when clocks are aligned indicating the end of the update sequence.

	}
	else
	{

      /* Read CGU_STAT register to verify that:
       *  - The CGU_STAT.PLLEN bit=1 (PLL enabled).
       *  - The CGU_STAT.PLOCK bit=1 (PLL is not locking).
       *  - The CGU_STAT.CLKSALGN bit=0 (clocks aligned).
       */
      
	  while((HW::CGU->STAT & (CGU_STAT_PLLEN|CGU_STAT_PLOCK|CGU_STAT_CLKSALGN)) != (CGU_STAT_PLLEN|CGU_STAT_PLOCK));

      /* Write the new values to the CGU_DIV register’s clock divisor select
       * fields with the CGU_DIV.UPDT bit=0.
       */
		HW::CGU->DIV = CGU_DIV_OSEL(OCLK_DIV) | CGU_DIV_DSEL(DCLK_DIV) | CGU_DIV_S1SEL(SCLK1_DIV) | CGU_DIV_S0SEL(SCLK0_DIV) | CGU_DIV_SYSSEL(SCLK_DIV) | CGU_DIV_CSEL(CCLK_DIV);

      /* Write the new values to the CGU_CTL.DF and CGU_CTL.MSEL fields. */
      HW::CGU->CTL = ctl;

      /* Poll CGU_STAT bits to determine the end of the update sequence
       * when the PLL is locked and the clocks are aligned. This is done 
       * by checking for when the CGU_STAT.PLOCK bit=1, CGU_STAT.PLLBP bit=0
       * and CGU_STAT.CLKSALGN bit=0.
       */
 	  while((HW::CGU->STAT & (CGU_STAT_PLOCK|CGU_STAT_PLLBP|CGU_STAT_CLKSALGN)) != CGU_STAT_PLOCK);
	};

	if (HW::RCU->STAT.BMODE == 1) HW::SPI2->CLK = BAUD_RATE_DIVISOR;

} /* initcode */ 
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif
