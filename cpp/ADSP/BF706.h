#ifndef BF706_H__16_07_2024__15_40
#define BF706_H__16_07_2024__15_40

#pragma once

#ifndef CORETYPE_BF706
#error  Must #include "core.h"
#endif 

#ifndef __ADSPBF70x__
#error  Must #include "core.h"
#endif

#include <ADSP-BF706_device.h>
#include <blackfin.h>
#include <bfrom.h>
#include <sys\exception.h>
#include <sysreg.h>
#include <adi_bootstream.h >

#include "types.h"

typedef ADI_ROM_BOOT_HEADER ADI_BOOT_HEADER;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

extern void InitIVG(u32 IVG, void (*EVT)());
extern void InitSEC(u32 PID, void (*EVT)(), byte prio = 0);

extern u32 Get_CCLK();
extern u32 Get_SCLK();
extern u32 Get_SCLK0();
extern u32 Get_SCLK1();
extern u32 Get_CCLK_MHz();	
extern u32 Get_SCLK_MHz();	
extern u32 Get_SCLK0_MHz();	
extern u32 Get_SCLK1_MHz();	

extern void* Alloc_L1_DataA(u32 size);
extern void* Alloc_L1_DataB(u32 size);
extern void* Alloc_L2_NoCache(u32 size);
extern void* Alloc_UnCached(u32 size);
extern void* Alloc_L2_CacheWT(u32 size);

extern bool IsCodeCacheEnabled();
extern bool IsDataCacheEnabled();
extern bool IsDataCacheA_Enabled();
extern bool IsDataCacheB_Enabled();

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
	typedef volatile u16		BF_RW16, BF_WO16;		// Hardware register definition
	typedef volatile u32		BF_RW32, BF_WO32;		// Hardware register definition
	typedef volatile u8			BF_RW8, BF_WO8;			// Hardware register definition

	typedef volatile const u16	BF_RO16;		// Hardware register definition
	typedef volatile const u32	BF_RO32;		// Hardware register definition
	typedef volatile const u8	BF_RO8;			// Hardware register definition
	typedef volatile void*		BF_PTR;			// Hardware register definition

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_L1DM_TypeDef
	 * \brief  Data Memory Unit
	*/ 

	struct S_L1DM
	{
		BF_RO32 SRAM_BASE_ADDR;					/*!< SRAM Base Address Register */
		BF_RW32 CTL;							/*!< Data Memory Control Register */
		BF_RO32 STAT;							/*!< Data Memory CPLB Status Register */
		BF_RO32 CPLB_FAULT_ADDR;				/*!< Data Memory CPLB Fault Address Register (legacy name) */
		BF_RW32 CPLB_DFLT;						/*!< Data Memory CPLB Default Settings Register */
		BF_RO32 PERR_STAT;						/*!< Data Memory Parity Error Status Register */
													BF_RO8                  z__RESERVED0[232];
		BF_RW32 CPLB_ADDR[16];					/*!< Data Memory CPLB Address Registers */
													BF_RO8                  z__RESERVED1[192];
		BF_RW32 CPLB_DATA[16];					/*!< Data Memory CPLB Data Registers */
	}; 

	#define L1DM_ENX				(1UL<<16)		/* Enable Extended Data Access */
	#define L1DM_RDCHK				(1UL<<9)		/* Read Parity Check */
	#define L1DM_CBYPASS			(1UL<<8)		/* Cache Bypass */
	#define L1DM_DCBS				(1UL<<4)		/* Data Cache Bank Select */
	#define L1DM_ENCPLB				(1UL<<1)		/* Enable CPLB Operations */
	#define L1DM_CFG				(3UL<<2)		/* Configure as Cache or SRAM */
	#define L1DM_SRAM_AB			(0UL<<2)		/* CFG: L1 data block A as SRAM, L1 data block B as SRAM */
	#define L1DM_ACACHE_BSRAM       (1UL<<2)		/* CFG: L1 data block A as cache, L1 data block B as SRAM */
	#define L1DM_ACACHE_BCACHE      (3UL<<2)		/* CFG: L1 data block A as cache, L1 data block B as cache */ 

	#define L1DM_DFLT_L1SWRITE		(1UL<<11)		/* L1 Supervisor Mode Write */
	#define L1DM_DFLT_L1UWRITE		(1UL<<10)		/* L1 User Mode Write */
	#define L1DM_DFLT_L1UREAD		(1UL<<9)		/* L1 User Mode Read */
	#define L1DM_DFLT_L1EOM			(1UL<<8)		/* L1 Exception On Miss */
	#define L1DM_DFLT_SYSSWRITE		(1UL<<7)		/* System Supervisor Mode Write */
	#define L1DM_DFLT_SYSUWRITE		(1UL<<6)		/* System User Mode Write */
	#define L1DM_DFLT_SYSUREAD		(1UL<<5)		/* System User Mode Read */
	#define L1DM_DFLT_SYSEOM		(1UL<<4)		/* System Exception On Miss */

	#define L1DM_DFLT_NOCACHE		(0)				/* SYSCPROPS: Non-cacheable memory space */
	#define L1DM_DFLT_WB_L1			(1)				/* SYSCPROPS: Non-cacheable in L2; write back cacheable in L1 */
	#define L1DM_DFLT_WB_L2			(2)				/* SYSCPROPS: Write back cacheable in L2; non-cacheable in L1 */
	#define L1DM_DFLT_WB_L1L2		(3)				/* SYSCPROPS: Write back cacheable in L1 and L2 */
	#define L1DM_DFLT_IO			(4)				/* SYSCPROPS: I/O device space */
	#define L1DM_DFLT_WT_L1			(5)				/* SYSCPROPS: Non-cacheable in L2; write through cacheable in L1 */
	#define L1DM_DFLT_WT_L2			(6)				/* SYSCPROPS: Write through cacheable in L2; non-cacheable in L1 */
	#define L1DM_DFLT_WT_L1L2		(7)				/* SYSCPROPS: Write through cacheable in L1 and L2 */ 

	#define L1DM_PSIZE(v)			(((v)&15)<<16)  /* Page Size */
	#define L1DM_PSIZE_1KB			(0UL<<16)		/* PSIZE: 1K byte page size */
	#define L1DM_PSIZE_4KB			(1UL<<16)		/* PSIZE: 4K byte page size */
	#define L1DM_PSIZE_16KB			(2UL<<16)		/* PSIZE: 16K byte page size */
	#define L1DM_PSIZE_64KB			(3UL<<16)		/* PSIZE: 64K byte page size */
	#define L1DM_PSIZE_256KB		(4UL<<16)		/* PSIZE: 256K byte page size */
	#define L1DM_PSIZE_1MB			(5UL<<16)		/* PSIZE: 1M byte page size */
	#define L1DM_PSIZE_4MB			(6UL<<16)		/* PSIZE: 4M byte page size */
	#define L1DM_PSIZE_16MB			(7UL<<16)		/* PSIZE: 16M byte page size */
	#define L1DM_PSIZE_64MB			(8UL<<16)		/* PSIZE: 64M byte page size */
	#define L1DM_PSIZE_256MB		(9UL<<16)		/* PSIZE: 256M byte page size */ 
	#define L1DM_PSIZE_1GB			(10UL<<16)		/* PSIZE: 1G byte page size */

	#define L1DM_NOCACHE			(0<<12)			/* CPROPS: Non-cacheable memory space */
	#define L1DM_WB_L1				(1<<12)			/* CPROPS: Non-cacheable in L2; write back cacheable in L1 */
	#define L1DM_WB_L2				(2<<12)			/* CPROPS: Write back cacheable in L2; non-cacheable in L1 */
	#define L1DM_WB_L1L2			(3<<12)			/* CPROPS: Write back cacheable in L1 and L2 */
	#define L1DM_IO					(4<<12)			/* CPROPS: I/O device space */
	#define L1DM_WT_L1				(5<<12)			/* CPROPS: Non-cacheable in L2; write through cacheable in L1 */
	#define L1DM_WT_L2				(6<<12)			/* CPROPS: Write through cacheable in L2; non-cacheable in L1 */
	#define L1DM_WT_L1L2			(7<<12)			/* CPROPS: Write through cacheable in L1 and L2 */

	#define L1DM_DIRTY				(1UL<<7)		/* CPROPS: Dirty CPLB */
	#define L1DM_SWRITE				(1UL<<4)		/* CPROPS: Supervisor Mode Write */
	#define L1DM_UWRITE				(1UL<<3)		/* CPROPS: User Mode Write */
	#define L1DM_UREAD				(1UL<<2)		/* CPROPS: User Mode Read */
	#define L1DM_LOCK				(1UL<<1)		/* CPROPS: Lock CPLB */
	#define L1DM_VALID				(1UL<<0)		/* CPROPS: Valid CPLB */ 


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_L1IM_TypeDef
	 * \brief  Instruction Memory Unit
	*/ 

	struct S_L1IM
	{
    												BF_RO8                  z__RESERVED0[4];
		BF_RW32 CTL;                          /*!< Instruction Memory Control Register */
		BF_RO32 STAT;                         /*!< Instruction Memory CPLB Status Register */
		BF_RO32 CPLB_FAULT_ADDR;             /*!< Instruction Memory CPLB Fault Address Register (legacy name) */
		BF_RW32 CPLB_DFLT;                    /*!< Instruction Memory CPLB Default Settings Register */
		BF_RW32 PERR_STAT;                    /*!< Instruction Parity Error Status Register */
    											BF_RO8                  z__RESERVED1[232];
		BF_RW32 CPLB_ADDR[16];                /*!< Instruction Memory CPLB Address Registers */
    											BF_RO8                  z__RESERVED2[192];
		BF_RW32 CPLB_DATA[16];                /*!< Instruction Memory CPLB Data Registers */
	};

	#define L1IM_CPRIORST				(1UL<<13)							/* ICTL: Cache Line Priority Reset */
	#define L1IM_RDCHK					(1UL<<9)							/* ICTL: Read Parity Checking */
	#define L1IM_CBYPASS				(1UL<<8)							/* ICTL: Cache Bypass */
	#define L1IM_CFG					(1UL<<2)							/* ICTL: Configure L1 code memory as cache */
	#define L1IM_ENCPLB					(1UL<<1)							/* ICTL: Enable ICPLB */

	#define L1IM_ILLADDR				(1UL<<19)							/* ISTAT: Illegal Address */
	#define L1IM_MODE					(1UL<<17)							/* ISTAT: Access Mode */
	#define L1IM_FAULT					(0xFFFF)							/* ISTAT: Fault Status */ 

	#define L1IM_L1UREAD				(1UL<<9)							/* ICPLB_DFLT: L1 user mode read access default */
	#define L1IM_L1EOM					(1UL<<8)							/* ICPLB_DFLT: Access exception on Instruction CPLB miss to L1 memory space */
	#define L1IM_SYSUREAD				(1UL<<5)							/* ICPLB_DFLT: System user mode read access default */
	#define L1IM_SYSEOM					(1UL<<4)							/* ICPLB_DFLT: Access exception on Instruction CPLB miss to System space */
	#define L1IM_DFLT_NOCACHE			(0UL<<0)							/* ICPLB_DFLT.SYSCPROPS: Non-cacheable memory space : Default cacheability properties for system space */ 
	#define L1IM_DFLT_L1CACHE			(1UL<<0)							/* ICPLB_DFLT.SYSCPROPS: Cacheable in L1 : Default cacheability properties for system space */ 

	//#define L1IM_IPERR_STAT_BYTELOC         24							/* Parity Error Bytes */
	//#define L1IM_IPERR_STAT_PORT            22							/* Parity Error Port */
	//#define L1IM_IPERR_STAT_ADDRESS          3							/* Parity Error Address */
	//#define L1IM_IPERR_STAT_LOCATION         0							/* Parity Error Location */ 

	#define L1IM_PSIZE(v)				(((v)&15)<<16)                      /* Page Size */
	#define L1IM_PSIZE_1KB				(0UL<<16)							/* PSIZE: 1K byte page size */
	#define L1IM_PSIZE_4KB				(1UL<<16)							/* PSIZE: 4K byte page size */
	#define L1IM_PSIZE_16KB				(2UL<<16)							/* PSIZE: 16K byte page size */
	#define L1IM_PSIZE_64KB				(3UL<<16)							/* PSIZE: 64K byte page size */
	#define L1IM_PSIZE_256KB			(4UL<<16)							/* PSIZE: 256K byte page size */
	#define L1IM_PSIZE_1MB				(5UL<<16)							/* PSIZE: 1M byte page size */
	#define L1IM_PSIZE_4MB				(6UL<<16)							/* PSIZE: 4M byte page size */
	#define L1IM_PSIZE_16MB				(7UL<<16)							/* PSIZE: 16M byte page size */
	#define L1IM_PSIZE_64MB				(8UL<<16)							/* PSIZE: 64M byte page size */
	#define L1IM_PSIZE_256MB			(9UL<<16)							/* PSIZE: 256M byte page size */ 
	#define L1IM_PSIZE_1GB				(10UL<<16)							/* PSIZE: 1G byte page size */

	#define L1IM_NOCACHE				(0UL<<12)							/* ICPLB_DATA.CPROPS: Non-cacheable memory space */
	#define L1IM_L1CACHE				(1UL<<12)							/* ICPLB_DATA.CPROPS: Non-cacheable in L2; cacheable in L1 */

	#define L1IM_CPRIO_LO				(0UL<<8)							/* ICPLB_DATA: Cache Line Priority : Low importance */
	#define L1IM_CPRIO_HI				(1UL<<8)							/* ICPLB_DATA: Cache Line Priority : High importance */
	#define L1IM_UREAD					(1UL<<2)							/* ICPLB_DATA: Allow User Read */
	#define L1IM_LOCK					(1UL<<1)							/* ICPLB_DATA: CPLB Lock */
	#define L1IM_VALID					(1UL<<0)							/* ICPLB_DATA: CPLB Valid */ 

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_ICU_TypeDef
	 * \brief  Interrupt Controller
	*/ 

	struct S_ICU
	{
		BF_PTR	Evt[16];                      /*!< Event Vector Table Registers (legacy name) */
    												BF_RO8                  z__RESERVED0[192];
		BF_RW32 Evt_OVERRIDE;                 /*!< Event Vector Table Override Register (legacy name) */
		BF_RW32 IMask;                        /*!< Interrupt Mask Register (legacy name) */
		BF_RO32 IPend;                        /*!< Interrupt Pending Register (legacy name) */
		BF_RW32 ILat;                         /*!< Interrupt Latch Register (legacy name) */
    												BF_RO8                  z__RESERVED1[4];
		BF_RW32	Cid;                           /*!< Context ID Register */
		BF_RW32 Cec_sid;                      /*!< System ID Register (legacy name) */
	}; 	

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_TMR_TypeDef
	 * \brief  Core Timer
	*/ 

	struct S_TMR
	{
		BF_RW32 CNTL;                        /*!< Timer Control Register (legacy name) */
		BF_RW32 PERIOD;                      /*!< Timer Period Register (legacy name) */
		BF_RW32 SCALE;                       /*!< Timer Scale Register (legacy name) */
		BF_RW32 COUNT;                       /*!< Timer Count Register (legacy name) */
	}; 

	#define TMR_INT							(1UL<<3)	/* Timer generated interrupt (sticky) */
	#define TMR_AUTORLD						(1UL<<2)	/* Timer auto reload */
	#define TMR_EN							(1UL<<1)	/* Timer enable */
	#define TMR_PWR							(1UL<<0)	/* Timer Low Power Control */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_PF_TypeDef
	 * \brief  Performance Monitor
	*/

	struct S_PF
	{
		BF_RW32 CTL;                        /*!< Control Register (legacy name) */
    											BF_RO8                  z__RESERVED0[252];
		BF_RW32 CNTR0;                      /*!< Counter 0 Register (legacy name) */
		BF_RW32 CNTR1;                      /*!< Counter 1 Register (legacy name) */
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_OPT_TypeDef
	 * \brief  Optional Feature 
	*/

	struct S_OPT
	{
		BF_RO32 _FEATURE0;                      /*!< Feature Core 0  Register (_FEATURE0 to avoid conflict with legacy macro) */
	}; 

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_BP_TypeDef
	 * \brief  Branch Predictor
	*/
	
	struct S_BP
	{
		BF_RW32 CFG;                           /*!< Configuration Register */
		BF_RW32 STAT;                          /*!< Status Register */
	}; 

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	/*!
	 * \struct ADI_RCU_TypeDef
	 * \brief  Reset Control Unit
	*/

	struct S_RCU
	{
		BF_RW32 CTL;                           /*!< Control Register */

		struct
		{
			BF_RW32 HWRST		:1;     /* Hardware Reset */
			BF_RW32 HBRST		:1;     /* Hibernate Reset */
			BF_RW32 SSRST		:1;     /* System Source Reset */
			BF_RW32	SWRST		:1;		/* Software Reset */ 
			BF_RW32				:1;     
			BF_RW32 RSTOUT		:1;		/* Reset Out Status */  
			BF_RW32				:2;     
			BF_RW32 BMODE		:4;		/* Boot Mode */  
			BF_RW32				:4;     
			BF_RW32 ADDRERR		:1;		/* Address Error */   
			BF_RW32 LWERR		:1;     /* Lock Write Error */
			BF_RW32 RSTOUTERR	:1;     /* Reset Out Error */

			operator u32() { return *((u32*)this); }
			u32 operator=(u32 v) { return *((u32*)this) = v; }

			u32 Bits() { return *((u32*)this); } 

		} STAT;	//RW16	STATUS;	/**< \brief Offset: 0x1A (R/W 16) I2CM Status */

		//BF_RW32 STAT;                          /*!< Status Register */
		BF_RW32 CRCTL;                         /*!< Core Reset Control Register */
		BF_RW32 CRSTAT;                        /*!< Core Reset Status Register */
		BF_RW32 SIDIS;                         /*!< System Interface Disable Register */
		BF_RO32 SISTAT;                        /*!< System Interface Status Register */
		BF_RW32 SVECT_LCK;                     /*!< SVECT Lock Register */
		BF_RW32 BCODE;                         /*!< Boot Code Register */
		BF_RW32 SVECT0;                        /*!< Software Vector Register 0 */
    												BF_RO8                  z__RESERVED0[60];
		BF_RW32 MSG;                           /*!< Message Register */
		BF_RW32 MSG_SET;                       /*!< Message Set Bits Register */
		BF_RW32 MSG_CLR;                       /*!< Message Clear Bits Register */
    												BF_RO8                  z__RESERVED1[4];
		BF_RO32 REVID;                         /*!< Revision ID Register */
	};

	#define RCU_STAT_RSTOUTERR              (1UL<<18)                               /* Reset Out Error */
	#define RCU_STAT_LWERR                  (1UL<<17)                               /* Lock Write Error */
	#define RCU_STAT_ADDRERR                (1UL<<16)                               /* Address Error */
	#define RCU_STAT_BMODE(v)                (((v)&0xF)<<8)                         /* Boot Mode */
	#define RCU_STAT_RSTOUT                  (1UL<<5)                               /* Reset Out Status */
	#define RCU_STAT_SWRST                   (1UL<<3)                               /* Software Reset */
	#define RCU_STAT_SSRST                   (1UL<<2)                               /* System Source Reset */
	#define RCU_STAT_HBRST                   (1UL<<1)                               /* Hibernate Reset */
	#define RCU_STAT_HWRST                   (1UL<<0)                               /* Hardware Reset */
											
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_TRU_TypeDef
	 * \brief  Trigger Routing Unit
	*/

	struct S_TRU
	{
		BF_RW32 SSR[102];                      /*!< Slave Select Register */
    												BF_RO8                  z__RESERVED0[1608];
		BF_RW32 MTR;                           /*!< Master Trigger Register */
    												BF_RO8                  z__RESERVED1[4];
		BF_RW32 ERRADDR;                       /*!< Error Address Register */
		BF_RW32 STAT;                          /*!< Status Information Register */
    												BF_RO8                  z__RESERVED2[4];
		BF_RW32 GCTL;                          /*!< Global Control Register */
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_CGU_TypeDef
	 * \brief  Clock Generation Unit
	*/

	struct S_CGU
	{
		BF_RW32 CTL;                           /*!< Control Register */
		BF_RW32 PLLCTL;                        /*!< PLL Control Register */
		BF_RW32 STAT;                          /*!< Status Register */
		BF_RW32 DIV;                           /*!< Clocks Divisor Register */
		BF_RW32 CLKOUTSEL;                     /*!< CLKOUT Select Register */
    												BF_RO8                  z__RESERVED0[4];
		BF_RW32 TSCTL;                         /*!< Timestamp Control Register */
		BF_RW32 TSVALUE0;                      /*!< Timestamp Counter Initial 32 LSB Value Register */
		BF_RW32 TSVALUE1;                      /*!< Timestamp Counter Initial MSB Value Register */
		BF_RO32 TSCOUNT0;                      /*!< Timestamp Counter 32 LSB */
		BF_RO32 TSCOUNT1;                      /*!< Timestamp Counter 32 MSB Register */
		BF_RW32 CCBF_DIS;                      /*!< Core Clock Buffer Disable Register */
		BF_RO32 CCBF_STAT;                     /*!< Core Clock Buffer Status Register */
    												BF_RO8                  z__RESERVED1[4];
		BF_RW32 SCBF_DIS;                      /*!< System Clock Buffer Disable Register */
		BF_RO32 SCBF_STAT;                     /*!< System Clock Buffer Status Register */
    												BF_RO8                  z__RESERVED2[8];
		BF_RO32 REVID;                         /*!< Revision ID Register */
	};

	#define CGU_CTL_LOCK                     (1UL<<31)                               /* Lock */
	#define CGU_CTL_WFI                      (1UL<<30)                               /* Wait For Idle */
	#define CGU_CTL_MSEL(v)                  (((v)&0x7F)<<8)                               /* Multiplier Select */
	#define CGU_CTL_DF                       (1UL<<0)                               /* Divide Frequency */

	#define CGU_STAT_PCFGERR                (1UL<<21)                               /* PLL Configuration Error */
	#define CGU_STAT_WDIVERR                (1UL<<20)                               /* Write to DIV Error */
	#define CGU_STAT_WDFMSERR               (1UL<<19)                               /* Write to DF or MSEL Error */
	#define CGU_STAT_LWERR                  (1UL<<17)                               /* Lock Write Error */
	#define CGU_STAT_ADDRERR                (1UL<<16)                               /* Address Error */
	#define CGU_STAT_CLKSALGN                (1UL<<3)                               /* Clock Alignment */
	#define CGU_STAT_PLOCK                   (1UL<<2)                               /* PLL Lock */
	#define CGU_STAT_PLLBP                   (1UL<<1)                               /* PLL Bypass */
	#define CGU_STAT_PLLEN                   (1UL<<0)                               /* PLL Enable */

	#define CGU_DIV_LOCK                    (1UL<<31)                               /* Lock */
	#define CGU_DIV_UPDT                    (1UL<<30)                               /* Update Clock Divisors */
	#define CGU_DIV_ALGN                    (1UL<<29)                               /* Align */
	#define CGU_DIV_OSEL(v)                	(((v)&0x7F)<<22)                               /* OUTCLK Divisor */
	#define CGU_DIV_DSEL(v)                	(((v)&0x1F)<<16)                               /* DCLK Divisor */
	#define CGU_DIV_S1SEL(v)               	(((v)&0x07)<<13)                               /* SCLK 1 Divisor */
	#define CGU_DIV_SYSSEL(v)              	(((v)&0x1F)<<8)                               /* SYSCLK Divisor */
	#define CGU_DIV_S0SEL(v)               	(((v)&0x07)<<5)                               /* SCLK 0 Divisor */
	#define CGU_DIV_CSEL(v)                	(((v)&0x1F)<<0)                               /* CCLK Divisor */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_DPM_TypeDef
	 * \brief  Dynamic Power Management
	*/

	struct S_DPM
	{
		BF_RW32 CTL;                           /*!< Control Register */
		BF_RW32 STAT;                          /*!< Status Register */
    												BF_RO8                  z__RESERVED0[20];
		BF_RW32 WAKE_EN;                       /*!< Wakeup Enable Register */
		BF_RW32 WAKE_POL;                      /*!< Wakeup Polarity Register */
		BF_RW32 WAKE_STAT;                     /*!< Wakeup Status Register */
		BF_RW32 HIB_DIS;                       /*!< Hibernate Disable Register */
		BF_RW32 PGCNTR;                        /*!< Power Good Counter Register */
		BF_RW32 RESTORE[16];                   /*!< Restore Registers */
    												BF_RO8                  z__RESERVED1[20];
		BF_RO32 REVID;                         /*!< Revision ID */
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_SEC_CB_TypeDef
	 * \brief  SCI Registers
	*/

	struct S_SEC_CB
	{
		BF_RW32 CCTL;	/*!< SCI Control Register n */
		BF_RW32 CSTAT;	/*!< SCI Status Register n */
		BF_RO32 CPND;	/*!< Core Pending Register n */
		BF_RO32 CACT;	/*!< SCI Active Register n */
		BF_RW32 CPMSK;	/*!< SCI Priority Mask Register n */
		BF_RW32 CGMSK;	/*!< SCI Group Mask Register n */
		BF_RW32 CPLVL;	/*!< SCI Priority Level Register n */
		BF_RW32 CSID;	/*!< SCI Source ID Register n */
    					BF_RO8	z__RESERVED0[32];
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_SEC_SB_TypeDef
	 * \brief  SSI Registers
	*/

	struct S_SEC_SB
	{
		BF_RW32 SCTL;	/*!< Source Control Register n */
		BF_RW32 SSTAT;	/*!< Source Status Register n */
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_SEC_TypeDef
	 * \brief  System Event Controller
	*/

	struct S_SEC
	{
		BF_RW32 	GCTL;						/*!< Global Control Register */
		BF_RW32 	GSTAT;						/*!< Global Status Register */
		BF_RW32 	RAISE;						/*!< Global Raise Register */
		BF_RW32 	END;						/*!< Global End Register */
		BF_RW32 	FCTL;						/*!< Fault Control Register */
		BF_RW32 	FSTAT;						/*!< Fault Status Register */
		BF_RO32 	FSID;						/*!< Fault Source ID Register */
		BF_RW32 	FEND;						/*!< Fault End Register */
		BF_RW32 	FDLY;						/*!< Fault Delay Register */
		BF_RO32 	FDLY_CUR;					/*!< Fault Delay Current Register */
		BF_RW32 	FSRDLY;						/*!< Fault System Reset Delay Register */
		BF_RO32 	FSRDLY_CUR;					/*!< Fault System Reset Delay Current Register */
		BF_RW32 	FCOPP;						/*!< Fault COP Period Register */
		BF_RO32 	FCOPP_CUR;					/*!< Fault COP Period Current Register */
    												BF_RO8                  z__RESERVED0[968];
		S_SEC_CB	SCI;							/*!< SCI Registers */
    												BF_RO8                  z__RESERVED1[960];
		S_SEC_SB	SSI[PARAM_SEC0_SCOUNT];		/*!< SSI Registers */

		typedef void (*EVT)();
	};

	#define SEC_INTERRUPT_HANDLER(_NAME) _Pragma("diag(suppress:1658)") _Pragma("regs_clobbered \"R0 R1 R2 P0 P1 P2 ASTAT\"") void _NAME()

	#define SEC_LOCK	(1UL<<31)			/*	CCTL Lock */
	#define SEC_NMIEN	(1UL<<16)			/*	CCTL NMI Enable */
	#define SEC_WFI		(1UL<<12)			/*	CCTL Wait For Idle */
	#define SEC_RESET	(1UL<<1)			/*	CCTL Reset */
	#define SEC_EN		(1UL<<0)			/*	CCTL Enable */

	#define SEC_NMI		(1UL<<16)			/*	CSTAT NMI */
	//#define SEC_WFI		(1UL<<12)		/*	CSTAT Wait For Idle */
	#define SEC_SIDV	(1UL<<10)			/*	CSTAT SID Valid */
	#define SEC_ACTV	(1UL<<9)			/*	CSTAT ACT Valid */
	#define SEC_PNDV	(1UL<<8)			/*	CSTAT PND Valid */
	#define SEC_ERRC	(1UL<<4)			/*	CSTAT Error Cause */
	#define SEC_ERR		(1UL<<1)			/*	CSTAT Error */

	//#define SEC_LOCK	(1UL<<31)			/*	FCTL Lock */
	#define SEC_TES		(1UL<<13)			/*	FCTL Trigger Event Select */
	#define SEC_CMS		(1UL<<12)			/*	FCTL COP Mode Select */
	#define SEC_FIEN	(1UL<<7)			/*	FCTL Fault Input Enable */
	#define SEC_SREN	(1UL<<6)			/*	FCTL System Reset Enable */
	#define SEC_TOEN	(1UL<<5)			/*	FCTL Trigger Output Enable */
	#define SEC_FOEN	(1UL<<4)			/*	FCTL Fault Output Enable */
	//#define SEC_RESET	(1UL<<1)			/*	FCTL Reset */
	//#define SEC_EN	(1UL<<0)			/*	FCTL Enable */

	//#define SEC_LOCK	(1UL<<31)			/*	GCTL Lock */
	//#define SEC_RESET	(1UL<<1)			/*	GCTL Reset */
	//#define SEC_EN	(1UL<<0)			/*	GCTL Enable */

	//#define SEC_LOCK	(1UL<<31)			/*	SCTL Lock */
	#define SEC_CTG(v)	(((v)&0xFF)<<24)	/*	SCTL Core Target Select */
	#define SEC_GRP		(1UL<<16)			/*	SCTL Group Select */
	#define SEC_PRIO(v)	(((v)&0xFF)<<8)		/*	SCTL Priority Level Select */
	#define SEC_ERREN	(1UL<<4)			/*	SCTL Error Enable */
	#define SEC_ES		(1UL<<3)			/*	SCTL Edge Select */
	#define SEC_SEN		(1UL<<2)			/*	SCTL Source (signal) Enable */
	#define SEC_FEN		(1UL<<1)			/*	SCTL Fault Enable */
	#define SEC_IEN		(1UL<<0)			/*	SCTL Interrupt Enable */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_SPU_TypeDef
	 * \brief  System Protection Unit
	*/

	struct S_SPU
	{
		BF_RW32 CTL;                           /*!< Control Register */
		BF_RW32 STAT;                          /*!< Status Register */
    												BF_RO8                  z__RESERVED0[1016];
		BF_RW32 WP[78];                        /*!< Write Protect Register n */
    												BF_RO8                  z__RESERVED1[776];
		BF_RW32 SECURECTL;                     /*!< Secure Control Register */
    												BF_RO8                  z__RESERVED2[8];
		BF_RO32 SECURECHK;                     /*!< Secure Check Register */
    												BF_RO8                  z__RESERVED3[304];
		BF_RW32 SECUREC;                       /*!< Secure Core Registers */
    												BF_RO8                  z__RESERVED4[124];
		BF_RW32 SECUREP[78];                   /*!< Secure Peripheral Register */
	};


	/*!
	 * \struct ADI_SMPU_Rn_TypeDef
	 * \brief  Region n
	*/

	struct S_SPMU_Rn
	{
		BF_RW32 RCTL;                          /*!< Region n Control */
		BF_RW32 RADDR;                         /*!< Region n Address */
		BF_RW32 RIDA;                          /*!< Region n ID Register A */
		BF_RW32 RIDMSKA;                       /*!< Region n ID Mask Register A */
		BF_RW32 RIDB;                          /*!< Region n ID Register B */
		BF_RW32 RIDMSKB;                       /*!< Region n ID Mask Register B */
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_SMPU_TypeDef
	 * \brief  System Memory Protection Unit
	*/

	struct S_SMPU
	{
		BF_RW32 CTL;                           /*!< SMPU Control */
		BF_RW32 STAT;                          /*!< SMPU Status */
		BF_RO32 IADDR;                         /*!< Interrupt Address */
		BF_RO32 IDTLS;                         /*!< Interrupt Details */
		BF_RO32 BADDR;                         /*!< Bus Error Address */
		BF_RO32 BDTLS;                         /*!< Bus Error Details */
    												BF_RO8                  z__RESERVED0[8];
		ADI_SMPU_Rn_TypeDef Rn[8];                       /*!< Region n */
    												BF_RO8                  z__RESERVED1[320];
		BF_RO32 REVID;                         /*!< SMPU Revision ID */
    												BF_RO8                  z__RESERVED2[1500];
		BF_RW32 SECURECTL;                     /*!< SMPU Control Secure Accesses */
    												BF_RO8                  z__RESERVED3[28];
		BF_RW32 SECURERCTL[8];                 /*!< Region n Control Secure Accesses */
	};

	typedef S_SMPU S_SMPU0, S_SMPU1;
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_L2CTL_TypeDef
	 * \brief  L2 Memory Controller
	*/

	struct S_L2CTL
	{
		BF_RW32 CTL;                           /*!< Control Register */
		BF_RW32 ACTL_C0;                       /*!< Access Control Core 0 Register */
    												BF_RO8                  z__RESERVED0[4];
		BF_RW32 ACTL_SYS;                      /*!< Access Control System Register */
		BF_RW32 STAT;                          /*!< Status Register */
		BF_RW32 RPCR;                          /*!< Read Priority Count Register */
		BF_RW32 WPCR;                          /*!< Write Priority Count Register */
    												BF_RO8                  z__RESERVED1[8];
		BF_RW32 INIT;                          /*!< Initialization Register */
    												BF_RO8                  z__RESERVED2[16];
		BF_RO32 ISTAT;                         /*!< Initialization Status Register */
		BF_RW32 PCTL;                          /*!< Power Control Register */
		BF_RO32 ERRADDR0;                      /*!< ECC Error Address 0 Register */
		BF_RO32 ERRADDR1;                      /*!< ECC Error Address 1 Register */
		BF_RO32 ERRADDR2;                      /*!< ECC Error Address 2 Register */
		BF_RO32 ERRADDR3;                      /*!< ECC Error Address 3 Register */
		BF_RO32 ERRADDR4;                      /*!< ECC Error Address 4 Register */
		BF_RO32 ERRADDR5;                      /*!< ECC Error Address 5 Register */
		BF_RO32 ERRADDR6;                      /*!< ECC Error Address 6 Register */
		BF_RO32 ERRADDR7;                      /*!< ECC Error Address 7 Register */
		BF_RO32 ERRADDR8;                      /*!< ECC Error Address 8 Register */
    												BF_RO8                  z__RESERVED3[28];
		BF_RO32 ET0;                           /*!< Error Type 0 Register */
		BF_RO32 EADDR0;                        /*!< Error Type 0 Address Register */
		BF_RO32 ET1;                           /*!< Error Type 1 Register */
		BF_RO32 EADDR1;                        /*!< Error Type 1 Address Register */
    												BF_RO8                  z__RESERVED4[92];
		BF_RW32 SCTL;                          /*!< Scrub Control Register */
		BF_RW32 SADR;                          /*!< Scrub Start Address Register */
		BF_RW32 SCNT;                          /*!< Scrub Count Register */
    												BF_RO8                  z__RESERVED5[4];
		BF_RO32 REVID;                         /*!< Revision ID Register */
	};

	#define L2CTL_LOCK                  (1UL<<31)	/* Lock */
	#define L2CTL_DISURP                (1UL<<28)	/* Disable Urgent Request Priority */
	#define L2CTL_ECCMAP8               (1UL<<24)	/* ECC Map Bank 8 (ROM) */
	#define L2CTL_ECCMAP7               (1UL<<23)	/* ECC Map Bank 7 */
	#define L2CTL_ECCMAP6               (1UL<<22)	/* ECC Map Bank 6 */
	#define L2CTL_ECCMAP5               (1UL<<21)	/* ECC Map Bank 5 */
	#define L2CTL_ECCMAP4               (1UL<<20)	/* ECC Map Bank 4 */
	#define L2CTL_ECCMAP3               (1UL<<19)	/* ECC Map Bank 3 */
	#define L2CTL_ECCMAP2               (1UL<<18)	/* ECC Map Bank 2 */
	#define L2CTL_ECCMAP1               (1UL<<17)	/* ECC Map Bank 1 */
	#define L2CTL_ECCMAP0               (1UL<<16)	/* ECC Map Bank 0 */
	#define L2CTL_BK8EDIS               (1UL<<8)	/* Bank 8 (ROM) ECC Disable */
	#define L2CTL_BK7EDIS               (1UL<<7)	/* Bank 7 ECC Disable */
	#define L2CTL_BK6EDIS               (1UL<<6)	/* Bank 6 ECC Disable */
	#define L2CTL_BK5EDIS				(1UL<<5)	/* Bank 5 ECC Disable */
	#define L2CTL_BK4EDIS				(1UL<<4)	/* Bank 4 ECC Disable */
	#define L2CTL_BK3EDIS				(1UL<<3)	/* Bank 3 ECC Disable */
	#define L2CTL_BK2EDIS				(1UL<<2)	/* Bank 2 ECC Disable */
	#define L2CTL_BK1EDIS				(1UL<<1)	/* Bank 1 ECC Disable */
	#define L2CTL_BK0EDIS				(1UL<<0)	/* Bank 0 ECC Disable */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_SWU_WB_TypeDef
	 * \brief  SWU0 Module's WB register block
	*/

	struct S_SWU_WB
	{
		BF_RW32 CTL;                           /*!< Control Register n */
		BF_RW32 LA;                            /*!< Lower Address Register n */
		BF_RW32 UA;                            /*!< Upper Address Register n */
		BF_RW32 ID;                            /*!< ID Register n */
		BF_RW32 CNT;                           /*!< Count Register n */
		BF_RW32 TARG;                          /*!< Target Register n */
		BF_RW32 HIST;                          /*!< Bandwidth History Register n */
		BF_RW32 CUR;                           /*!< Current Register n */
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_SWU_TypeDef
	 * \brief  System WatchPoint Unit
	*/

	struct S_SWU
	{
		BF_RW32 	GCTL;                      	/*!< Global Control Register */
		BF_RW32 	GSTAT;                     	/*!< Global Status Register */
    											BF_RO8                  z__RESERVED0[8];
		S_SWU_WB	WB[4];						/*!<  */
	}; 

	typedef S_SWU S_SWU0, S_SWU1, S_SWU2, S_SWU3, S_SWU4, S_SWU5, S_SWU6, S_SWU7;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_PADS_TypeDef
	 * \brief  Pads Controller
	*/

	struct S_PADS
	{
    												BF_RO8                  z__RESERVED0[4];
		BF_RW32 PCFG0;                         /*!< Peripheral Configuration0 Register */
	};

	#define PADS_PUE                  (1UL<<12)		/* Pull-Up Enable */
	#define PADS_TWI0VSEL             (1UL<<9)      /* Drive/tolerate for TWI0 Pins SCL & SDA */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_PINT_TypeDef
	 * \brief  PINT
	*/

	struct S_PINT
	{
		BF_RW32 MSK_SET;                       /*!< Pint Mask Set Register */
		BF_RW32 MSK_CLR;                       /*!< Pint Mask Clear Register */
		BF_RW32 REQ;                           /*!< Pint Request Register */
		BF_RW32 ASSIGN;                        /*!< Pint Assign Register */
		BF_RW32 EDGE_SET;                      /*!< Pint Edge Set Register */
		BF_RW32 EDGE_CLR;                      /*!< Pint Edge Clear Register */
		BF_RW32 INV_SET;                       /*!< Pint Invert Set Register */
		BF_RW32 INV_CLR;                       /*!< Pint Invert Clear Register */
		BF_RW32 PINSTATE;                      /*!< Pint Pinstate Register */
		BF_RW32 LATCH;                         /*!< Pint Latch Register */
	};

	typedef S_PINT S_PINT0, S_PINT1, S_PINT2;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_TIMER_TMR_TypeDef
	 * \brief  TIMER0 Module's TMR register block
	*/

	struct S_TIMER_TMR
	{
		BF_RW32 CFG;                           /* Timer n Configuration Register */
		BF_RO32 CNT;                           /* Timer n Counter Register */
		BF_RW32 PER;                           /* Timer n Period Register */
		BF_RW32 WID;                           /* Timer n Width Register */
		BF_RW32 DLY;                           /* Timer n Delay Register */
    												BF_RO8                  z__RESERVED0[12];
	};


	#define TMR_MODE_IDLE			(0x0UL)		/* TMODE: Idle Mode */
	#define TMR_MODE_WIDCAP0		(0xAUL)		/* TMODE: Measurement report at asserting edge of waveform */
	#define TMR_MODE_WIDCAP1		(0xBUL)		/* TMODE: Measurement report at de-asserting edge of waveform */
	#define TMR_MODE_PWMCONT		(0xCUL)		/* TMODE: Continuous PWMOUT mode */
	#define TMR_MODE_PWMSING		(0xDUL)		/* TMODE: Single pulse PWMOUT mode */
	#define TMR_MODE_EXTCLK			(0xEUL)		/* TMODE: EXTCLK mode */
	#define TMR_MODE_PININT			(0xFUL)		/* TMODE: PININT (pin interrupt) mode */
	#define TMR_MODE_WDPER			(0x8UL)		/* TMODE: Period Watchdog Mode */
	#define TMR_MODE_WDWID			(0x9UL)		/* TMODE: Width Watchdog Mode */

	#define TMR_IRQ_EDGE			(0UL<<4)	/* IRQMODE: Active Edge Mode */
	#define TMR_IRQ_DELAY			(1UL<<4)	/* IRQMODE: Delay Expired Mode */
	#define TMR_IRQ_WIDTH_DELAY		(2UL<<4)	/* IRQMODE: Width Plus Delay Expired Mode */
	#define TMR_IRQ_PERIOD			(3UL<<4)	/* IRQMODE: Period Expired Mode */

	#define TMR_SLAVETRIG			(1UL<<6)	/* Slave Trigger Response */
	#define TMR_PULSEHI				(1UL<<7)	/* Polarity Response Select */
	#define TMR_CLKSEL_SCLK			(0UL<<8)	/* CLKSEL: Use SCLK */
	#define TMR_CLKSEL_ALT0			(1UL<<8)	/* CLKSEL: Use TMR_ALT_CLK0 as the TMR clock */
	#define TMR_CLKSEL_ALT1			(3UL<<8)	/* CLKSEL: Use TMR_ALT_CLK1 as the TMR clock */
	#define TMR_TINSEL				(1UL<<10)	/* Timer Input Select (for WIDCAP, WATCHDOG, PININT modes) */
	#define TMR_OUTDIS				(1UL<<11)	/* Output Disable */
	#define TMR_BDLYEN				(1UL<<12)	/* Broadcast Delay Enable */
	#define TMR_BWIDEN				(1UL<<13)	/* Broadcast Width Enable */
	#define TMR_BPEREN				(1UL<<14)	/* Broadcast Period Enable */
	#define TMR_EMURUN				(1UL<<15)	/* Run Timer (Counter) During Emulation */
	#define TMR_TGLTRIG				(1UL<<16)	/* Slave Trigger Toggle Enable */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_TIMER_TypeDef
	 * \brief  General-Purpose Timer Block
	*/

	struct S_TIMER
	{
    												BF_RO8                  z__RESERVED0[4];
		BF_RW32 RUN;							/* Run Register */
		BF_RW32 RUN_SET;						/* Run Set Register */
		BF_RW32 RUN_CLR;						/* Run Clear Register */
		BF_RW32 STOP_CFG;						/* Stop Configuration Register */
		BF_RW32 STOP_CFG_SET;					/* Stop Configuration Set Register */
		BF_RW32 STOP_CFG_CLR;					/* Stop Configuration Clear Register */
		BF_RW32 DATA_IMSK;						/* Data Interrupt Mask Register */
		BF_RW32 STAT_IMSK;						/* Status Interrupt Mask Register */
		BF_RW32 TRG_MSK;						/* Trigger Master Mask Register */
		BF_RW32 TRG_IE;							/* Trigger Slave Enable Register */
		BF_RW32 DATA_ILAT;						/* Data Interrupt Latch Register */
		BF_RW32 STAT_ILAT;						/* Status Interrupt Latch Register */
		BF_RO32 ERR_TYPE;						/* Error Type Status Register */
		BF_RW32 BCAST_PER;						/* Broadcast Period Register */
		BF_RW32 BCAST_WID;						/* Broadcast Width Register */
		BF_RW32 BCAST_DLY;						/* Broadcast Delay Register */
    												BF_RO8                  z__RESERVED1[28];
		S_TIMER_TMR	 TMR[8];					/* TMR register block */
	};

	#define TIMER_TMR0		(1UL<<0)			/* TMR0 Bit mask */
	#define TIMER_TMR1		(1UL<<1)			/* TMR1 Bit mask */
	#define TIMER_TMR2		(1UL<<2)			/* TMR2 Bit mask */
	#define TIMER_TMR3		(1UL<<3)			/* TMR3 Bit mask */
	#define TIMER_TMR4		(1UL<<4)			/* TMR4 Bit mask */
	#define TIMER_TMR5		(1UL<<5)			/* TMR5 Bit mask */
	#define TIMER_TMR6		(1UL<<6)			/* TMR6 Bit mask */
	#define TIMER_TMR7		(1UL<<7)			/* TMR7 Bit mask */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	#define WDOG_WDRO	(1UL<<15)				/* Watch Dog Rollover */
	#define WDOG_WDEN	(1UL<<4)				/* Watch Dog Enable */
	#define WDOG_WDDIS	(0x0AD0)				/* Watch Dog Enable */

	/*!
	 * \struct ADI_WDOG_TypeDef
	 * \brief  Watch Dog Timer Unit
	*/

	struct S_WDOG
	{
		BF_RW32 CTL;                           /*!< Control Register */
		BF_RW32 CNT;                           /*!< Count Register */
		BF_RW32 STAT;                          /*!< Watchdog Timer Status Register */

		inline void Update()	{ STAT = 0; }
		inline void Reset()	{ STAT = 0; }
		inline void Disable()	{ CTL = WDOG_WDDIS; }
	};

	typedef S_WDOG S_WDT;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_TAPC_TypeDef
	 * \brief  Test Access Port Controller
	*/

	struct S_TAPC
	{
		BF_RO32 IDCODE;                        /*!< IDCODE Register */
		BF_RO32 USERCODE;                      /*!< USERCODE Register */
		BF_RW32 SDBGKEY_CTL;                   /*!< Secure Debug Control Register */
		BF_RO32 SDBGKEY_STAT;                  /*!< Secure Debug Key Status Register */
		BF_RW32 SDBGKEY0;                      /*!< Secure Debug Key 0 Register */
		BF_RW32 SDBGKEY1;                      /*!< Secure Debug Key 1 Register */
		BF_RW32 SDBGKEY2;                      /*!< Secure Debug Key 2 Register */
		BF_RW32 SDBGKEY3;                      /*!< Secure Debug Key 3 Register */
    												BF_RO8                  z__RESERVED0[16];
		BF_RW32 DBGCTL;                        /*!< Debug Control Register */
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_TWI_TypeDef
	 * \brief  2-Wire Interface
	*/

	struct S_TWI
	{
		BF_RW32 CLKDIV;                        /*!< SCL Clock Divider Register */
		BF_RW32 CTL;                           /*!< Control Register */
		BF_RW32 SLVCTL;                        /*!< Slave Mode Control Register */
		BF_RO32 SLVSTAT;                       /*!< Slave Mode Status Register */
		BF_RW32 SLVADDR;                       /*!< Slave Mode Address Register */
		BF_RW32 MSTRCTL;                       /*!< Master Mode Control Registers */
		BF_RW32 MSTRSTAT;                      /*!< Master Mode Status Register */
		BF_RW32 MSTRADDR;                      /*!< Master Mode Address Register */
		BF_RW32 ISTAT;                         /*!< Interrupt Status Register */
		BF_RW32 IMSK;                          /*!< Interrupt Mask Register */
		BF_RW32 FIFOCTL;                       /*!< FIFO Control Register */

		struct
		{
			BF_RO32 TXSTAT		:2;				/* Rx FIFO Status */
			BF_RO32 RXSTAT		:2;				/* Tx FIFO Status */

			operator u32() { return *((u32*)this); }
			u32 Bits() { return *((u32*)this); } 

		} FIFOSTAT; // BF_RO32 FIFOSTAT; /*!< FIFO Status Register */
		                     
    												BF_RO8                  z__RESERVED0[80];
		BF_WO32 TXDATA8;                       /*!< Tx Data Single-Byte Register */
		BF_WO32 TXDATA16;                      /*!< Tx Data Double-Byte Register */
		BF_WO32 RXDATA8;                       /*!< Rx Data Single-Byte Register */
		BF_WO32 RXDATA16;                      /*!< Rx Data Double-Byte Register */
	};

	#define TWI_CLKHI(v)		(((v)&0xFF)<<8)		/* SCL Clock High Periods */
	#define TWI_CLKLO(v)		(((v)&0xFF)<<0)		/* SCL Clock Low Periods */

	#define TWI_CTL_SCCB		(1UL<<9)			/* SCCB Compatibility */
	#define TWI_CTL_EN			(1UL<<7)			/* Enable Module */
	#define TWI_CTL_PRESCALE(v)	(((v)&0x7F)<<0)		/* SCLK Prescale Value */

	#define TWI_SLV_GEN			(1UL<<4)			/* General Call Enable */
	#define TWI_SLV_NAK			(1UL<<3)			/* Not Acknowledge */
	#define TWI_SLV_TDVAL		(1UL<<2)			/* Transmit Data Valid for Slave */
	#define TWI_SLV_EN			(1UL<<0)			/* Enable Slave Mode */

	#define TWI_SLV_GCALL		(1UL<<1)			/* General Call */
	#define TWI_SLV_DIR			(1UL<<0)			/* Transfer Direction for Slave */

	#define TWI_MST_SCLOVR		(1UL<<15)			/* Serial Clock Override */
	#define TWI_MST_SDAOVR		(1UL<<14)			/* Serial Data Override */
	#define TWI_MST_DCNT(v)		(((v)&0xFF)<<6)		     /* Data Transfer Count */
	#define TWI_MST_RSTART		(1UL<<5)			/* Repeat Start */
	#define TWI_MST_STOP		(1UL<<4)			/* Issue Stop Condition */
	#define TWI_MST_FAST		(1UL<<3)			/* Fast Mode */
	#define TWI_MST_DIR			(1UL<<2)			/* Transfer Direction for Master */
	#define TWI_MST_EN			(1UL<<0)			/* Enable Master Mode */

	#define TWI_BUSBUSY			(1UL<<8)			/* Bus Busy */
	#define TWI_SCLSEN			(1UL<<7)			/* Serial Clock Sense */
	#define TWI_SDASEN			(1UL<<6)			/* Serial Data Sense */
	#define TWI_BUFWRERR		(1UL<<5)			/* Buffer Write Error */
	#define TWI_BUFRDERR		(1UL<<4)			/* Buffer Read Error */
	#define TWI_DNAK			(1UL<<3)			/* Data Not Acknowledged */
	#define TWI_ANAK			(1UL<<2)			/* Address Not Acknowledged */
	#define TWI_LOSTARB			(1UL<<1)			/* Lost Arbitration */
	#define TWI_MPROG			(1UL<<0)			/* Master Transfer in Progress */

	#define TWI_SCLI			(1UL<<15)			/* Serial Clock Interrupt */
	#define TWI_SDAI			(1UL<<14)			/* Serial Data Interrupt */
	#define TWI_RXSERV			(1UL<<7 )			/* Rx FIFO Service */
	#define TWI_TXSERV			(1UL<<6 )			/* Tx FIFO Service */
	#define TWI_MERR			(1UL<<5 )			/* Master Transfer Error */
	#define TWI_MCOMP			(1UL<<4 )			/* Master Transfer Complete */
	#define TWI_SOVF			(1UL<<3)			/* Slave Overflow */
	#define TWI_SERR			(1UL<<2)			/* Slave Transfer Error */
	#define TWI_SCOMP			(1UL<<1)			/* Slave Transfer Complete */
	#define TWI_SINIT			(1UL<<0)			/* Slave Transfer Initiated */

	#define TWI_RXILEN			(1UL<<3)			/* Rx Buffer Interrupt Length */
	#define TWI_TXILEN			(1UL<<2)			/* Tx Buffer Interrupt Length */
	#define TWI_RXFLUSH			(1UL<<1)			/* Rx Buffer Flush */
	#define TWI_TXFLUSH			(1UL<<0)			/* Tx Buffer Flush */

	#define TWI_RXSTAT_EMPTY	(0)					/* Rx FIFO Status */
	#define TWI_RXSTAT_1BYTE	(1)					/* Rx FIFO Status */
	#define TWI_RXSTAT_FULL		(3)					/* Rx FIFO Status */

	#define TWI_TXSTAT_EMPTY	(0)					/* Tx FIFO Status */
	#define TWI_TXSTAT_1BYTE	(1)					/* Tx FIFO Status */
	#define TWI_TXSTAT_FULL		(3)					/* Tx FIFO Status */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_SPORT_TypeDef
	 * \brief  Serial Port
	*/

	struct S_SPORT
	{
		BF_RW32 CTL_A;                         /*!< Half SPORT 'A' Control Register */
		BF_RW32 DIV_A;                         /*!< Half SPORT 'A' Divisor Register */
		BF_RW32 MCTL_A;                        /*!< Half SPORT 'A' Multi-channel Control Register */
		BF_RW32 CS0_A;                         /*!< Half SPORT 'A' Multi-channel 0-31 Select Register */
		BF_RW32 CS1_A;                         /*!< Half SPORT 'A' Multi-channel 32-63 Select Register */
		BF_RW32 CS2_A;                         /*!< Half SPORT 'A' Multi-channel 64-95 Select Register */
		BF_RW32 CS3_A;                         /*!< Half SPORT 'A' Multi-channel 96-127 Select Register */
    												BF_RO8                  z__RESERVED0[4];
		BF_RW32 ERR_A;                         /*!< Half SPORT 'A' Error Register */
		BF_RO32 MSTAT_A;                       /*!< Half SPORT 'A' Multi-channel Status Register */
		BF_RW32 CTL2_A;                        /*!< Half SPORT 'A' Control 2 Register */
    												BF_RO8                  z__RESERVED1[20];
		BF_RW32 TXPRI_A;                       /*!< Half SPORT 'A' Tx Buffer (Primary) Register */
		BF_RW32 RXPRI_A;                       /*!< Half SPORT 'A' Rx Buffer (Primary) Register */
		BF_RW32 TXSEC_A;                       /*!< Half SPORT 'A' Tx Buffer (Secondary) Register */
		BF_RW32 RXSEC_A;                       /*!< Half SPORT 'A' Rx Buffer (Secondary) Register */
    												BF_RO8                  z__RESERVED2[48];
		BF_RW32 CTL_B;                         /*!< Half SPORT 'B' Control Register */
		BF_RW32 DIV_B;                         /*!< Half SPORT 'B' Divisor Register */
		BF_RW32 MCTL_B;                        /*!< Half SPORT 'B' Multi-channel Control Register */
		BF_RW32 CS0_B;                         /*!< Half SPORT 'B' Multi-channel 0-31 Select Register */
		BF_RW32 CS1_B;                         /*!< Half SPORT 'B' Multi-channel 32-63 Select Register */
		BF_RW32 CS2_B;                         /*!< Half SPORT 'B' Multichannel 64-95 Select Register */
		BF_RW32 CS3_B;                         /*!< Half SPORT 'B' Multichannel 96-127 Select Register */
    												BF_RO8                  z__RESERVED3[4];
		BF_RW32 ERR_B;                         /*!< Half SPORT 'B' Error Register */
		BF_RO32 MSTAT_B;                       /*!< Half SPORT 'B' Multi-channel Status Register */
		BF_RW32 CTL2_B;                        /*!< Half SPORT 'B' Control 2 Register */
    												BF_RO8                  z__RESERVED4[20];
		BF_RW32 TXPRI_B;                       /*!< Half SPORT 'B' Tx Buffer (Primary) Register */
		BF_RW32 RXPRI_B;                       /*!< Half SPORT 'B' Rx Buffer (Primary) Register */
		BF_RW32 TXSEC_B;                       /*!< Half SPORT 'B' Tx Buffer (Secondary) Register */
		BF_RW32 RXSEC_B;                       /*!< Half SPORT 'B' Rx Buffer (Secondary) Register */
	}; 	

	typedef S_SPORT S_SPORT0, S_SPORT1;

#define SPORT_SPENPRI					(1UL<<0)				/* Serial Port Enable (Primary) */
#define SPORT_DTYPE_RJ_ZFILL			(0UL<<1)				/* DTYPE: Right-justify data, zero-fill unused MSBs */
#define SPORT_DTYPE_RJ_SFILL			(1UL<<1)				/* DTYPE: Right-justify data, sign-extend unused MSBs */
#define SPORT_DTYPE_ULAW				(2UL<<1)				/* DTYPE: m-law compand data */
#define SPORT_DTYPE_ALAW				(3UL<<1)				/* DTYPE: A-law compand data */
#define SPORT_LSBF						(1UL<<3)				/* Least-Significant Bit First */
#define SPORT_SLEN(v)					(((v)&31)<<4)		/* Serial Word Length */
#define SPORT_PACK						(1UL<<9)				/* Packing Enable */
#define SPORT_ICLK						(1UL<<10)				/* Internal Clock */
#define SPORT_OPMODE					(1UL<<11)				/* Operation mode */
#define SPORT_CKRE						(1UL<<12)				/* Clock Rising Edge */
#define SPORT_FSR						(1UL<<13)				/* Frame Sync Required */
#define SPORT_IFS						(1UL<<14)				/* Internal Frame Sync */
#define SPORT_DIFS						(1UL<<15)				/* Data-Independent Frame Sync */
#define SPORT_LFS						(1UL<<16)				/* Active-Low Frame Sync / L_FIRST / PLFS */
#define SPORT_LAFS						(1UL<<17)				/* Late Frame Sync / OPMODE2 */
#define SPORT_RJUST						(1UL<<18)				/* Right-Justified Operation Mode */
#define SPORT_FSED						(1UL<<19)				/* Frame Sync Edge Detect */
#define SPORT_TFIEN						(1UL<<20)				/* Transmit Finish Interrupt Enable */
#define SPORT_GCLKEN					(1UL<<21)				/* Gated Clock Enable */
#define SPORT_SPENSEC					(1UL<<24)				/* Serial Port Enable (Secondary) */
#define SPORT_SPTRAN					(1UL<<25)				/* Serial Port Transfer Direction */
#define SPORT_DERRSEC					(1UL<<26)				/* Data Error Status (Secondary) */
#define SPORT_DXSSEC					(3UL<<27)				/* Data Transfer Buffer Status (Secondary) */
#define SPORT_DERRPRI					(1UL<<29)				/* Data Error Status (Primary) */
#define SPORT_DXSPRI					(3UL<<30)				/* Data Transfer Buffer Status (Primary) */

#define SPORT_FSDIV(v)					(((v)&0xFFFF)<<16)	/* Frame Sync Divisor */
#define SPORT_CLKDIV(v)					(((v)&0xFFFF)<<0)	/* Clock Divisor */

#define SPORT_WOFFSET					(((v)&0x3FF)<<16)       /* Window Offset */
#define SPORT_WSIZE						(((v)&0x7F)<<8)     /* Window Size */
#define SPORT_MFD						(((v)&0xF)<<4)          /* Multi-channel Frame Delay */
#define SPORT_MCPDE						(1UL<<2)				/* Multi-Channel Packing DMA Enable */
#define SPORT_MCE						(1UL<<0)				/* Multichannel enable */

#define SPORT_CURCHAN					(0x3FFUL<<0)            /* Multichannel Status: Current Channel */

#define SPORT_CKMUXSEL					(1UL<<1)                /* Clock Multiplexer Select */
#define SPORT_FSMUXSEL					(1UL<<0)                /* Frame Sync Multiplexer Select */

#define SPORT_FSERRSTAT					(1UL<<6)				/* Frame Sync Error Status */
#define SPORT_DERRSSTAT					(1UL<<5)				/* Data Error Secondary Status */
#define SPORT_DERRPSTAT					(1UL<<4)				/* Data Error Primary Status */
#define SPORT_FSERRMSK					(1UL<<2)				/* Frame Sync Error (Interrupt) Mask */
#define SPORT_DERRSMSK					(1UL<<1)				/* Data Error Secondary (Interrupt) Mask */
#define SPORT_DERRPMSK					(1UL<<0)				/* Data Error Primary (Interrupt) Mask */


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_SPIHP_TypeDef
	 * \brief  SPI Host Port
	*/

	struct S_SPIHP
	{
		BF_RW32 CTL;                           /*!< Control Register */
		BF_RW32 STAT;                          /*!< Status Register */
		BF_RW32 RDPF;                          /*!< Read Prefetch Register */
    												BF_RO8                  z__RESERVED0[4];
		BF_RW32 BAR[16];                       /*!< Base Address Register */
		BF_RW32 AUX[4];                        /*!< Auxiliary Register */
	} ;
	 
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_SMC_TypeDef
	 * \brief  Static Memory Controller
	*/

	struct S_SMC
	{
    												BF_RO8                  z__RESERVED0[12];
		BF_RW32 B0CTL;                         /*!< Bank 0 Control Register */
		BF_RW32 B0TIM;                         /*!< Bank 0 Timing Register */
		BF_RW32 B0ETIM;                        /*!< Bank 0 Extended Timing Register */
    												BF_RO8                  z__RESERVED1[4];
		BF_RW32 B1CTL;                         /*!< Bank 1 Control Register */
		BF_RW32 B1TIM;                         /*!< Bank 1 Timing Register */
		BF_RW32 B1ETIM;                        /*!< Bank 1 Extended Timing Register */
    												BF_RO8                  z__RESERVED2[4];
		BF_RW32 B2CTL;                         /*!< Bank 2 Control Register */
		BF_RW32 B2TIM;                         /*!< Bank 2 Timing Register */
		BF_RW32 B2ETIM;                        /*!< Bank 2 Extended Timing Register */
    												BF_RO8                  z__RESERVED3[4];
		BF_RW32 B3CTL;                         /*!< Bank 3 Control Register */
		BF_RW32 B3TIM;                         /*!< Bank 3 Timing Register */
		BF_RW32 B3ETIM;                        /*!< Bank 3 Extended Timing Register */
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_UART_TypeDef
	 * \brief  UART
	*/

	struct S_UART
	{
    												BF_RO8                  z__RESERVED0[4];
		BF_RW32 CTL;                           /*!< Control Register */
		BF_RW32 STAT;                          /*!< Status Register */
		BF_RW32 SCR;                           /*!< Scratch Register */
		BF_RW32 CLK;                           /*!< Clock Rate Register */
		BF_RW32 IMSK;                          /*!< Interrupt Mask Register */
		BF_RW32 IMSK_SET;                      /*!< Interrupt Mask Set Register */
		BF_RW32 IMSK_CLR;                      /*!< Interrupt Mask Clear Register */
		BF_RW32 RBR;                           /*!< Receive Buffer Register */
		BF_RW32 THR;                           /*!< Transmit Hold Register */
		BF_RW32 TAIP;                          /*!< Transmit Address/Insert Pulse Register */
		BF_RW32 TSR;                           /*!< Transmit Shift Register */
		BF_RW32 RSR;                           /*!< Receive Shift Register */
		BF_RW32 TXCNT;                         /*!< Transmit Counter Register */
		BF_RW32 RXCNT;                         /*!< Receive Counter Register */
	};

	typedef S_UART S_UART0, S_UART1;

	#define UART_RFRT                   (1UL<<30)                               /* Receive FIFO RTS Threshold */
	#define UART_RFIT                   (1UL<<29)                               /* Receive FIFO IRQ Threshold */
	#define UART_ACTS                   (1UL<<28)                               /* Automatic CTS */
	#define UART_ARTS                   (1UL<<27)                               /* Automatic RTS */
	#define UART_XOFF                   (1UL<<26)                               /* Transmitter off */
	#define UART_MRTS                   (1UL<<25)                               /* Manual Request to Send */
	#define UART_TPOLC                  (1UL<<24)                               /* IrDA TX Polarity Change */
	#define UART_RPOLC                  (1UL<<23)                               /* IrDA RX Polarity Change */
	#define UART_FCPOL                  (1UL<<22)                               /* Flow Control Pin Polarity */
	#define UART_SB                     (1UL<<19)                               /* Set Break */
	#define UART_FFE                    (1UL<<18)                               /* Force Framing Error on Transmit */
	#define UART_FPE                    (1UL<<17)                               /* Force Parity Error on Transmit */
	#define UART_STP                    (1UL<<16)                               /* Sticky Parity */
	#define UART_EPS                    (1UL<<15)                               /* Even Parity Select */
	#define UART_PEN                    (1UL<<14)                               /* Parity Enable */
	#define UART_STBH                   (1UL<<13)                               /* Stop Bits (Half Bit Time) */
	#define UART_STB                    (1UL<<12)                               /* Stop Bits */
	#define UART_5BIT					(0UL<<8)                              /* Word Length Select */
	#define UART_6BIT					(1UL<<8)                              /* Word Length Select */
	#define UART_7BIT					(2UL<<8)                              /* Word Length Select */
	#define UART_8BIT					(3UL<<8)                              /* Word Length Select */
	#define UART_MOD_UART               (0UL<<4)                              /* Mode of Operation */
	#define UART_MOD_MDB                (1UL<<4)                              /* Mode of Operation */
	#define UART_MOD_IRDA               (2UL<<4)                              /* Mode of Operation */
	#define UART_LOOP_EN                (1UL<<1)                              /* Loopback Enable */
	#define UART_EN                     (1UL<<0)                              /* Enable UART */

	#define UART_RFCS                   (1UL<<17)                               /* Receive FIFO Count Status */
	#define UART_CTS                    (1UL<<16)                               /* Clear to Send */
	#define UART_SCTS                   (1UL<<12)                               /* Sticky CTS */
	#define UART_RO                     (1UL<<11)                               /* Reception On-going */
	#define UART_ADDR                   (1UL<<10)                               /* Address Bit Status */
	#define UART_ASTKY                  (1UL<<9)                              /* Address Sticky */
	#define UART_TFI                    (1UL<<8)                              /* Transmission Finished Indicator */
	#define UART_TEMT                   (1UL<<7)                              /* TSR and THR Empty */
	#define UART_THRE                   (1UL<<5)                               /* Transmit Hold Register Empty */
	#define UART_BI                     (1UL<<4)                               /* Break Indicator */
	#define UART_FE                     (1UL<<3)                               /* Framing Error */
	#define UART_PE                     (1UL<<2)                               /* Parity Error */
	#define UART_OE                     (1UL<<1)                               /* Overrun Error */
	#define UART_DR                     (1UL<<0)                               /* Data Ready */
	
	#define UART_EDBO                   (1UL<<31)                               /* Enable Divide By One */
	#define UART_DIV(v)                 (((v)&0xFFFF)<<0)                               /* Divisor */

	#define UART_ETXS                   (1UL<<9)                               /* Enable TX to Status Interrupt Mask Status */
	#define UART_ERXS                   (1UL<<8)                               /* Enable RX to Status Interrupt Mask Status */
	#define UART_EAWI                   (1UL<<7)                               /* Enable Address Word Interrupt Mask Status */
	#define UART_ERFCI                  (1UL<<6)                               /* Enable Receive FIFO Count Interrupt Mask Status */
	#define UART_ETFI                   (1UL<<5)                               /* Enable Transmission Finished Interrupt Mask Status */
	#define UART_EDTPTI                 (1UL<<4)                               /* Enable DMA TX Peripheral Trigerred Interrupt Mask Status */
	#define UART_EDSSI                  (1UL<<3)                               /* Enable Modem Status Interrupt Mask Status */
	#define UART_ELSI                   (1UL<<2)                               /* Enable Line Status Interrupt Mask Status */
	#define UART_ETBEI                  (1UL<<1)                               /* Enable Transmit Buffer Empty Interrupt Mask Status */
	#define UART_ERBFI                  (1UL<<0)                               /* Enable Receive Buffer Full Interrupt Mask Status */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_EPPI_TypeDef
	 * \brief  Parallel Peripheral Interface
	*/

	struct S_EPPI
	{
		BF_RW32 STAT;                          /*!< Status Register */
		BF_RW32 HCNT;                          /*!< Horizontal Transfer Count Register */
		BF_RW32 HDLY;                          /*!< Horizontal Delay Count Register */
		BF_RW32 VCNT;                          /*!< Vertical Transfer Count Register */
		BF_RW32 VDLY;                          /*!< Vertical Delay Count Register */
		BF_RW32 FRAME;                         /*!< Lines Per Frame Register */
		BF_RW32 LINE;                          /*!< Samples Per Line Register */
		BF_RW32 CLKDIV;                        /*!< Clock Divide Register */
		BF_RW32 CTL;                           /*!< Control Register */
		BF_RW32 FS1_WLHB;                      /*!< FS1 Width Register / EPPI Horizontal Blanking Samples Per Line Register */
		BF_RW32 FS1_PASPL;                     /*!< FS1 Period Register / EPPI Active Samples Per Line Register */
		BF_RW32 FS2_WLVB;                      /*!< FS2 Width Register / EPPI Lines Of Vertical Blanking Register */
		BF_RW32 FS2_PALPF;                     /*!< FS2 Period Register / EPPI Active Lines Per Field Register */
		BF_RW32 IMSK;                          /*!< Interrupt Mask Register */
    												BF_RO8                  z__RESERVED0[4];
		BF_RW32 ODDCLIP;                       /*!< Clipping Register for ODD (Chroma) Data */
		BF_RW32 EVENCLIP;                      /*!< Clipping Register for EVEN (Luma) Data */
		BF_RW32 FS1_DLY;                       /*!< Frame Sync 1 Delay Value */
		BF_RW32 FS2_DLY;                       /*!< Frame Sync 2 Delay Value */
		BF_RW32 CTL2;                          /*!< Control Register 2 */
	};


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_CNT_TypeDef
	 * \brief  CNT
	*/

	struct S_CNT
	{
		BF_RW32 CFG;                           /*!< Configuration Register */
		BF_RW32 IMSK;                          /*!< Interrupt Mask Register */
		BF_RW32 STAT;                          /*!< Status Register */
		BF_RW32 CMD;                           /*!< Command Register */
		BF_RW32 DEBNCE;                        /*!< Debounce Register */
		BF_RW32 CNTR;                          /*!< Counter Register */
		BF_RW32 MAX;                           /*!< Maximum Count Register */
		BF_RW32 MIN;                           /*!< Minimum Count Register */
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_MSI_TypeDef
	 * \brief  Media Services Interface
	*/

	struct S_MSI
	{
		BF_RW32 CTL;                           /*!< Contorl register */
		BF_RW32 PWREN;                         /*!< Power Enable Register */
		BF_RW32 CLKDIV;                        /*!< Clock Divider Register */
    												BF_RO8                  z__RESERVED0[4];
		BF_RW32 CLKEN;                         /*!< Clock Enable Register */
		BF_RW32 TMOUT;                         /*!< Timeout Register */
		BF_RW32 CTYPE;                         /*!< Card Type Register */
		BF_RW32 BLKSIZ;                        /*!< Block Size Register */
		BF_RW32 BYTCNT;                        /*!< Byte Count Register */
		BF_RW32 IMSK;                          /*!< Interrupt Mask Register */
		BF_RW32 CMDARG;                        /*!< Command Argument Register */
		BF_RW32 CMD;                           /*!< Command Register */
		BF_RO32 RESP0;                         /*!< Response Register 0 */
		BF_RO32 RESP1;                         /*!< Response Register 1 */
		BF_RO32 RESP2;                         /*!< Response Register 2 */
		BF_RO32 RESP3;                         /*!< Response Register 3 */
		BF_RO32 MSKISTAT;                      /*!< Masked Interrupt Status Register */
		BF_RW32 ISTAT;                         /*!< Raw Interrupt Status Register */
		BF_RO32 STAT;                          /*!< Status Register */
		BF_RW32 FIFOTH;                        /*!< FIFO Threshold Watermark Register */
		BF_RO32 CDETECT;                       /*!< Card Detect Register */
    												BF_RO8                  z__RESERVED1[8];
		BF_RO32 TCBCNT;                        /*!< Transferred CIU Card Byte Count Register */
		BF_RO32 TBBCNT;                        /*!< Transferred Host to BIU-FIFO Byte Count Register */
		BF_RW32 DEBNCE;                        /*!< Debounce Count Register */
		BF_RW32 USRID;                         /*!< User ID Register */
		BF_RO32 VERID;                         /*!< Version ID Register */
		BF_RO32 HWCFG;                         /*!< Hardware Configuration Register */
    												BF_RO8                  z__RESERVED2[12];
		BF_RW32 BUSMODE;                       /*!< Bus Mode Register */
		BF_RW32 PLDMND;                        /*!< Poll Demand Register */
		BF_RW32 DBADDR;                        /*!< Descriptor List Base Address Register */
		BF_RW32 IDSTS;                         /*!< Internal DMA Status Register */
		BF_RW32 IDINTEN;                       /*!< Internal DMA Interrupt Enable Register */
		BF_RO32 DSCADDR;                       /*!< Current Host Descriptor Address Register */
		BF_RO32 BUFADDR;                       /*!< Current Buffer Descriptor Address Register */
    												BF_RO8                  z__RESERVED3[100];
		BF_RW32 CDTHRCTL;                      /*!< Card Threshold Control Register */
    												BF_RO8                  z__RESERVED4[4];
		BF_RW32 UHS_EXT;                       /*!< UHS Register Extention */
    												BF_RO8                  z__RESERVED5[4];
		BF_RW32 ENSHIFT;                       /*!< Register to control the amount of shift on enables */
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_DMA_TypeDef
	 * \brief  DMA Channel
	*/

	struct S_DMACH
	{
		BF_PTR	DSCPTR_NXT;                    /*!< Pointer to Next Initial Descriptor */
		BF_PTR	ADDRSTART;                     /*!< Start Address of Current Buffer */
		BF_RW32	CFG;                           /*!< Configuration Register */
		BF_RW32	XCNT;                          /*!< Inner Loop Count Start Value */
		BF_RW32	XMOD;                          /*!< Inner Loop Address Increment */
		BF_RW32	YCNT;                          /*!< Outer Loop Count Start Value (2D only) */
		BF_RW32	YMOD;                          /*!< Outer Loop Address Increment (2D only) */
    												BF_RO8                  z__RESERVED0[8];
		BF_PTR	DSCPTR_CUR;                    /*!< Current Descriptor Pointer */
		BF_RO32	DSCPTR_PRV;                    /*!< Previous Initial Descriptor Pointer */
		BF_PTR	ADDR_CUR;                      /*!< Current Address */
		BF_RW32	STAT;                          /*!< Status Register */
		BF_RO32	XCNT_CUR;                      /*!< Current Count(1D) or intra-row XCNT (2D) */
		BF_RO32	YCNT_CUR;                      /*!< Current Row Count (2D only) */
    												BF_RO8                  z__RESERVED1[4];
		BF_RW32	BWLCNT;                        /*!< Bandwidth Limit Count */
		BF_RO32	BWLCNT_CUR;                    /*!< Bandwidth Limit Count Current */
		BF_RW32	BWMCNT;                        /*!< Bandwidth Monitor Count */
		BF_RO32	BWMCNT_CUR;                    /*!< Bandwidth Monitor Count Current */

		BF_RO32	z__RESERVED2[1004];
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct DMADSC_AM // DMA Descriptor-Array Mode
	{
		void* 	ADDRSTART;	
		u32 	DMACFG;
		u32 	XCNT;
		u32 	XMOD;
		u32 	YCNT;
		u32 	YMOD;
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	struct DMADSC_LM // DMA Descriptor-List Mode
	{
		void* 	DSCPTR_NXT;
		void*	ADDRSTART; 
		u32		DMACFG;
		u32		XCNT;
		u32		XMOD;
		u32		YCNT;
		u32		YMOD;
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_DMA
	{
		S_DMACH	CH[16];
		
		S_DMACH	 SRC0;
		S_DMACH	 DST0;

		S_DMACH	z__Reserved1[18];

		S_DMACH	 SRC1;
		S_DMACH	 DST1;
		S_DMACH	 SRC2;
		S_DMACH	 DST2;

	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	#define DMA_HSDA                (1UL<<31)       					/* High Speed Descriptor Array Mode */
	#define DMA_DBURST              (1UL<<30)       					/* Descriptor Bursting (HSDA Mode) */
	#define DMA_PDRF                (1UL<<28)       					/* Peripheral Data Request Forward */
	#define DMA_TWOD                (1UL<<26)       					/* Two Dimension Addressing Enable */
	#define DMA_DESCIDCPY           (1UL<<25)       					/* Descriptor ID Copy Control */
	#define DMA_TOVEN               (1UL<<24)       					/* Trigger Overrun Error Enable */
	#define DMA_TRIG_NO				(0UL<<22)							/* Generate Outgoing Trigger */
	#define DMA_TRIG_XCNT			(1UL<<22)							/* Generate Outgoing Trigger */
	#define DMA_TRIG_YCNT			(2UL<<22)							/* Generate Outgoing Trigger */
	#define DMA_INT_NO				(0UL<<20)							/* Generate Interrupt */
	#define DMA_INT_XCNT			(1UL<<20)							/* Generate Interrupt */
	#define DMA_INT_YCNT			(2UL<<20)							/* Generate Interrupt */
	#define DMA_INT_PERIPH			(3UL<<20)							/* Generate Interrupt */
	#define DMA_NDSIZE              (((v)&7)<<16)   					/* Next Descriptor Set Size */
	#define DMA_NDSIZE1             (0UL<<16)							/* Next Descriptor Set Size */
	#define DMA_NDSIZE2             (1UL<<16)							/* Next Descriptor Set Size */
	#define DMA_NDSIZE3             (2UL<<16)							/* Next Descriptor Set Size */
	#define DMA_NDSIZE4             (3UL<<16)							/* Next Descriptor Set Size */
	#define DMA_NDSIZE5             (4UL<<16)							/* Next Descriptor Set Size */
	#define DMA_NDSIZE6             (5UL<<16)							/* Next Descriptor Set Size */
	#define DMA_NDSIZE7             (6UL<<16)							/* Next Descriptor Set Size */
	#define DMA_TWAIT               (1UL<<15)       					/* Wait for Trigger */
	#define DMA_FLOW                (((v)&7)<<12)   					/* Next Operation */
	#define DMA_FLOW_STOP			(0UL<<12)   						/* Next Operation */
	#define DMA_FLOW_AUTO			(1UL<<12)   						/* Next Operation */
	#define DMA_FLOW_DSCLIST		(4UL<<12)   						/* Next Operation */
	#define DMA_FLOW_DSCARRAY		(5UL<<12)   						/* Next Operation */
	#define DMA_FLOW_DODLIST 		(6UL<<12)   						/* Next Operation */
	#define DMA_FLOW_DODARRAY		(7UL<<12)   						/* Next Operation */
	#define DMA_MSIZE(v)            (((v)&7)<<8)    					/* Memory Transfer Word Size */
	#define DMA_MSIZE8              (0UL<<8)	    					/* Memory Transfer Word Size */
	#define DMA_MSIZE16             (1UL<<8)	    					/* Memory Transfer Word Size */
	#define DMA_MSIZE32             (2UL<<8)	    					/* Memory Transfer Word Size */
	#define DMA_MSIZE64             (3UL<<8)	    					/* Memory Transfer Word Size */
	#define DMA_MSIZE128            (4UL<<8)	    					/* Memory Transfer Word Size */
	#define DMA_MSIZE256            (5UL<<8)	    					/* Memory Transfer Word Size */
	#define DMA_PSIZE(v)			(((v)&7)<<4)						/* Peripheral Transfer Word Size */
	#define DMA_PSIZE8				(0UL<<4)							/* Peripheral Transfer Word Size */
	#define DMA_PSIZE16				(1UL<<4)							/* Peripheral Transfer Word Size */
	#define DMA_PSIZE32				(2UL<<4)							/* Peripheral Transfer Word Size */
	#define DMA_PSIZE64				(3UL<<4)							/* Peripheral Transfer Word Size */
	#define DMA_CADDR            	(1UL<<3)        					/* Use Current Address */
	#define DMA_SYNC             	(1UL<<2)        					/* Synchronize Work Unit Transitions */
	#define DMA_WNR              	(1UL<<1)        					/* Write/Read Channel Direction */
	#define DMA_EN					(1UL<<0)        					/* DMA Channel Enable */

	#define DMA_STAT_TWAIT				(1UL<<20)							/* Trigger Wait Status */
	//#define DMA_STAT_FIFOFILL               16  							/* FIFO Fill Status */
	//#define DMA_STAT_MBWID                  14  							/* Memory Bus Width */
	//#define DMA_STAT_PBWID                  12  							/* Peripheral Bus Width */
	//#define DMA_STAT_RUN                     8  							/* Run Status */
	//#define DMA_STAT_ERRC                    4  							/* Error Cause */
	#define DMA_STAT_PIRQ    			(1UL<<2) 							/* Peripheral Interrupt Request */
	#define DMA_STAT_IRQERR  			(1UL<<1) 							/* Error Interrupt */
	#define DMA_STAT_IRQDONE 			(1UL<<0) 							/* Work Unit/Row Done Interrupt */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_CRC_TypeDef
	 * \brief  Cyclic Redundancy Check Unit
	*/

	struct S_CRC
	{
		BF_RW32 CTL;                           /*!< Control Register */
		BF_RW32 DCNT;                          /*!< Data Word Count Register */
		BF_RW32 DCNTRLD;                       /*!< Data Word Count Reload Register */
    												BF_RO8                  z__RESERVED0[8];
		BF_RW32 COMP;                          /*!< Data Compare Register */
		BF_RW32 FILLVAL;                       /*!< Fill Value Register */
		BF_RW32 DFIFO;                         /*!< Data FIFO Register */
		BF_RW32 INEN;                          /*!< Interrupt Enable Register */
		BF_RW32 INEN_SET;                      /*!< Interrupt Enable Set Register */
		BF_RW32 INEN_CLR;                      /*!< Interrupt Enable Clear Register */
		BF_RW32 POLY;                          /*!< Polynomial Register */
    												BF_RO8                  z__RESERVED1[16];
		BF_RW32 STAT;                          /*!< Status Register */
		BF_RW32 DCNTCAP;                       /*!< Data Count Capture Register */
    												BF_RO8                  z__RESERVED2[4];
		BF_RW32 RESULT_FIN;                    /*!< CRC Final Result Register */
		BF_RW32 RESULT_CUR;                    /*!< CRC Current Result Register */
	};

	typedef S_CRC S_CRC0, S_CRC1;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_RTC_TypeDef
	 * \brief  Real Time Clock
	*/

	struct S_RTC
	{
		BF_RW32 CLK;                           /*!< RTC Clock register */
		BF_RW32 ALM;                           /*!< RTC Alarm register */
		BF_RW32 IEN;                           /*!< RTC Interrupt Enable Register */
		BF_RO32 STAT;                          /*!< RTC Status Register */
		BF_RW32 STPWTCH;                       /*!< RTC Stop Watch Register */
    												BF_RO8                  z__RESERVED0[4];
		BF_RW32 INIT;                          /*!< RTC Initialization Register */
		BF_RO32 INITSTAT;                      /*!< RTC Initialization Status Register */
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_TRNG_TypeDef
	 * \brief  
	*/


	struct S_TRNG
	{
		union 
		{
			BF_WO32 INPUT[2];                      /*!< TRNG Input Registers */
			BF_RO32 OUTPUT[4];                     /*!< TRNG Output Registers */
		};
	    
		union
		{
			BF_WO32 INTACK;                        /*!< TRNG Interrupt Acknowledge Register */
			BF_RO32 STAT;                          /*!< TRNG Status Register */
		};

		BF_RW32 CTL;                           /*!< TRNG Control Register */
		BF_RW32 CFG;                           /*!< TRNG Configuration Register */
		BF_RW32 ALMCNT;                        /*!< TRNG Alarm Counter Register */
		BF_RW32 FROEN;                         /*!< TRNG FRO Enable Register */
		BF_RW32 FRODETUNE;                     /*!< TRNG FRO De-tune Register */
		BF_RW32 ALMMSK;                        /*!< TRNG Alarm Mask Register */
		BF_RW32 ALMSTP;                        /*!< TRNG Alarm Stop Register */
		BF_RW32 LFSR_L;                        /*!< TRNG LFSR Access Registers */
		BF_RW32 LFSR_M;                        /*!< TRNG LFSR Access Registers */
		BF_RW32 LFSR_H;                        /*!< TRNG LFSR Access Registers */
		BF_RW32 CNT;                           /*!< TRNG Counter Access Register */
		BF_WO32 KEY[6];                        /*!< Post-Process Key Registers */
    												BF_RO8                  z__RESERVED0[4];
		BF_RO32 MONOBITCNT;                    /*!< TRNG Monobit Test Result Register */
	    
		union 
		{
			BF_WO32 V[2];                          /*!< TRNG Post-Process aVa Value Registers */
			BF_RO32 POKER[4];                      /*!< TRNG Poker Test Result Registers */
		};

		BF_RW32 TEST;                          /*!< TRNG Test Register */
		BF_RW32 BLKCNT;                        /*!< TRNG Block Count Register */
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*!
	 * \struct ADI_SCB_TypeDef
	 * \brief  System Cross Bar
	*/

	struct S_SCB
	{
    												BF_RO8                  z__RESERVED0[20512];
		BF_RW32 IB7_SYNC;                      /*!< DDR Interface Block Sync Mode */
    												BF_RO8                  z__RESERVED1[250076];
		BF_RW32 MST00_RQOS;                    /*!< DDE0 Read Quality of Service */
		BF_RW32 MST00_WQOS;                    /*!< DDE0 Write Quality of Service */
    												BF_RO8                  z__RESERVED2[4088];
		BF_RW32 MST01_RQOS;                    /*!< DDE1 Read Quality of Service */
		BF_RW32 MST01_WQOS;                    /*!< DDE1 Write Quality of Service */
    												BF_RO8                  z__RESERVED3[4088];
		BF_RW32 MST02_RQOS;                    /*!< DDE2 Read Quality of Service */
		BF_RW32 MST02_WQOS;                    /*!< DDE2 Write Quality of Service */
    												BF_RO8                  z__RESERVED4[4088];
		BF_RW32 MST03_RQOS;                    /*!< DDE3 Read Quality of Service */
		BF_RW32 MST03_WQOS;                    /*!< DDE3 Write Quality of Service */
    												BF_RO8                  z__RESERVED5[4088];
		BF_RW32 MST04_RQOS;                    /*!< DDE4 Read Quality of Service */
		BF_RW32 MST04_WQOS;                    /*!< DDE4 Write Quality of Service */
    												BF_RO8                  z__RESERVED6[4088];
		BF_RW32 MST05_RQOS;                    /*!< DDE5 Read Quality of Service */
		BF_RW32 MST05_WQOS;                    /*!< DDE5 Write Quality of Service */
    												BF_RO8                  z__RESERVED7[4088];
		BF_RW32 MST06_RQOS;                    /*!< DDE6 Read Quality of Service */
		BF_RW32 MST06_WQOS;                    /*!< DDE6 Write Quality of Service */
    												BF_RO8                  z__RESERVED8[4088];
		BF_RW32 MST07_RQOS;                    /*!< DDE7 Read Quality of Service */
		BF_RW32 MST07_WQOS;                    /*!< DDE7 Write Quality of Service */
    												BF_RO8                  z__RESERVED9[4088];
		BF_RW32 MST08_RQOS;                    /*!< DDE8 Read Quality of Service */
		BF_RW32 MST08_WQOS;                    /*!< DDE8 Write Quality of Service */
    												BF_RO8                 z__RESERVED10[4088];
		BF_RW32 MST09_RQOS;                    /*!< DDE9 Read Quality of Service */
		BF_RW32 MST09_WQOS;                    /*!< DDE9 Write Quality of Service */
    												BF_RO8                 z__RESERVED11[4088];
		BF_RW32 MST10_RQOS;                    /*!< DDE10 Read Quality of Service */
		BF_RW32 MST10_WQOS;                    /*!< DDE10 Write Quality of Service */
    												BF_RO8                 z__RESERVED12[4088];
		BF_RW32 MST11_RQOS;                    /*!< DDE11 Read Quality of Service */
		BF_RW32 MST11_WQOS;                    /*!< DDE11 Write Quality of Service */
    												BF_RO8                 z__RESERVED13[4088];
		BF_RW32 MST12_RQOS;                    /*!< DDE12 Read Quality of Service */
		BF_RW32 MST12_WQOS;                    /*!< DDE12 Write Quality of Service */
    												BF_RO8                 z__RESERVED14[4088];
		BF_RW32 MST13_RQOS;                    /*!< DDE13 Read Quality of Service */
		BF_RW32 MST13_WQOS;                    /*!< DDE13 Write Quality of Service */
    												BF_RO8                 z__RESERVED15[4088];
		BF_RW32 MST14_RQOS;                    /*!< DDE14 Read Quality of Service */
		BF_RW32 MST14_WQOS;                    /*!< DDE14 Write Quality of Service */
    												BF_RO8                 z__RESERVED16[4088];
		BF_RW32 MST15_RQOS;                    /*!< DDE15 Read Quality of Service */
		BF_RW32 MST15_WQOS;                    /*!< DDE15 Write Quality of Service */
    												BF_RO8                 z__RESERVED17[4088];
		BF_RW32 MST16_RQOS;                    /*!< DDE16 Read Quality of Service */
		BF_RW32 MST16_WQOS;                    /*!< DDE16 Write Quality of Service */
    												BF_RO8                 z__RESERVED18[4088];
		BF_RW32 MST17_RQOS;                    /*!< DDE17 Read Quality of Service */
		BF_RW32 MST17_WQOS;                    /*!< DDE17 Write Quality of Service */
    												BF_RO8                 z__RESERVED19[4088];
		BF_RW32 MST22_RQOS;                    /*!< USB0 Read Quality of Service */
		BF_RW32 MST22_WQOS;                    /*!< USB0 Write Quality of Service */
    												BF_RO8                 z__RESERVED20[4088];
		BF_RW32 MST25_RQOS;                    /*!< MSI0 Read Quality of Service */
		BF_RW32 MST25_WQOS;                    /*!< MSI0 Write Quality of Service */
    												BF_RO8                 z__RESERVED21[3864];
		BF_RW32 IB6_SYNC;                      /*!< Crypto Modules Sync Mode */
    												BF_RO8                 z__RESERVED22[220];
		BF_RW32 MST26_RQOS;                    /*!< Crypto Modules Read Quality of Service */
		BF_RW32 MST26_WQOS;                    /*!< Crypto Modules Write Quality of Service */
    												BF_RO8                 z__RESERVED23[4088];
		BF_RW32 MST23_RQOS;                    /*!< Memory Interface Read Quality of Service */
		BF_RW32 MST23_WQOS;                    /*!< Memory Interface Write Quality of Service */
    												BF_RO8                 z__RESERVED24[4088];
		BF_RW32 MST24_RQOS;                    /*!< MMR Read Quality of Service */
		BF_RW32 MST24_WQOS;                    /*!< MMR Write Quality of Service */
    												BF_RO8                 z__RESERVED25[4088];
		BF_RW32 MST18_RQOS;                    /*!< DDE18 Read Quality of Service */
		BF_RW32 MST18_WQOS;                    /*!< DDE18 Write Quality of Service */
    												BF_RO8                 z__RESERVED26[4088];
		BF_RW32 MST19_RQOS;                    /*!< DDE19 Read Quality of Service */
		BF_RW32 MST19_WQOS;                    /*!< DDE19 Write Quality of Service */
    												BF_RO8                 z__RESERVED27[4088];
		BF_RW32 MST20_RQOS;                    /*!< DDE20 Read Quality of Service */
		BF_RW32 MST20_WQOS;                    /*!< DDE20 Write Quality of Service */
    												BF_RO8                 z__RESERVED28[4088];
		BF_RW32 MST21_RQOS;                    /*!< DDE21 Read Quality of Service */
		BF_RW32 MST21_WQOS;                    /*!< DDE21 Write Quality of Service */
    												BF_RO8                 z__RESERVED29[4088];
		BF_RW32 MST27_RQOS;                    /*!< ETR Read Quality of Service */
		BF_RW32 MST27_WQOS;                    /*!< ETR Write Quality of Service */
    												BF_RO8                 z__RESERVED30[4088];
		BF_RW32 MST28_RQOS;                    /*!< SPIHP0 Read Quality of Service */
		BF_RW32 MST28_WQOS;                    /*!< SPIHP0 Write Quality of Service */
    												BF_RO8                 z__RESERVED31[4088];
		BF_RW32 MST29_RQOS;                    /*!< DAP Read Quality of Service */
		BF_RW32 MST29_WQOS;                    /*!< DAP Write Quality of Service */
    												BF_RO8                 z__RESERVED32[405272];
		BF_RW32 MST30_SYNC;                    /*!< Interface Block IB4 Sync Mode */
    												BF_RO8                 z__RESERVED33[4092];
		BF_RW32 MST31_SYNC;                    /*!< Interface Block IB5 Sync Mode */
	}; 

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_SPI
	{
							BF_RO32                  z__RESERVED0;
		BF_RW32 CTL;                           /*!< Control Register */
		BF_RW32 RXCTL;                         /*!< Receive Control Register */
		BF_RW32 TXCTL;                         /*!< Transmit Control Register */
		BF_RW32 CLK;                           /*!< Clock Rate Register */
		BF_RW32 DLY;                           /*!< Delay Register */
		BF_RW32 SLVSEL;                        /*!< Slave Select Register */
		BF_RW32 RWC;                           /*!< Received Word Count Register */
		BF_RW32 RWCR;                          /*!< Received Word Count Reload Register */
		BF_RW32 TWC;                           /*!< Transmitted Word Count Register */
		BF_RW32 TWCR;                          /*!< Transmitted Word Count Reload Register */
							BF_RO32                  z__RESERVED1;
		BF_RW32 IMSK;                          /*!< Interrupt Mask Register */
		BF_RW32 IMSK_CLR;                      /*!< Interrupt Mask Clear Register */
		BF_RW32 IMSK_SET;                      /*!< Interrupt Mask Set Register */
							BF_RO32                  z__RESERVED2;
		BF_RW32 STAT;                       /*!< Status Register */
		BF_RW32 _ILAT;                      /*!< Masked Interrupt Condition Register (_ILAT to avoid conflict with legacy macro) */
		BF_RW32 ILAT_CLR;                   /*!< Masked Interrupt Clear Register */
							BF_RO32                  z__RESERVED3;
		union
		{
			BF_RW32	D;                      /*!< Receive FIFO Data Register */
			BF_RW16	W;
			BF_RW8	B;
		}
		RFIFO;
							BF_RO32                  z__RESERVED4;
		BF_RW32 TFIFO;                      /*!< Transmit FIFO Data Register */
							BF_RO32                  z__RESERVED5;
		BF_RW32 MMRDH;                         /*!< Memory Mapped Read Header */
		BF_RW32 MMTOP;                         /*!< SPI Memory Top Address */ 	
	};

	typedef S_SPI S_SPI0, S_SPI1, S_SPI2;

	#define SPI_MMSE			(1UL<<31)       /* Memory-Mapped SPI Enable */
	#define SPI_MM_DIS 			(0UL<<31)       /* Memory-Mapped SPI Enable MMSE: Hardware automated access of memory-mapped SPI memory disabled. */
	#define SPI_MM_EN  			(1UL<<31)       /* Memory-Mapped SPI Enable MMSE: Hardware-automated access of memory-mapped SPI memory enabled. */
	#define SPI_MMWEM			(1UL<<30)       /* Memory Mapped Write Error Mask */
	#define SPI_WEM_UNMSK 		(0UL<<30)       /* Memory Mapped Write Error Mask MMWEM: Write error response returned upon write attempts to memory-mapped SPI memory */
	#define SPI_WEM_MSK   		(1UL<<30)       /* Memory Mapped Write Error Mask MMWEM: Write error response masked (not returned) upon write attempts to memory-mapped SPI memory */
	#define SPI_SOSI			(1UL<<22)       /* Start on MOSI */
	#define SPI_STMISO 			(0UL<<22)       /* Start on MOSI SOSI: Start on MISO (DIOM) or start on SPIQ3 (QSPI) */
	#define SPI_STMOSI 			(1UL<<22)       /* Start on MOSI SOSI: Start on MOSI */
	#define SPI_MIO_DIS 		(0UL<<20)       /* Multiple I/O Mode: No MIOM (disabled) */
	#define SPI_MIO_DUAL		(1UL<<20)       /* Multiple I/O Mode: dual I/O mode (DIOM) operation */
	#define SPI_MIO_QUAD		(2UL<<20)       /* Multiple I/O Mode: quad I/O mode (QIOM) operation */
	#define SPI_FMODE			(1UL<<18)       /* Fast-Mode Enable */
	#define SPI_FAST_DIS 		(0UL<<18)       /* Fast-Mode Enable FMODE: Disable*/
	#define SPI_FAST_EN  		(1UL<<18)       /* Fast-Mode Enable FMODE: Enable */
	#define SPI_FCWM(v)			(((v)&3)<<16)	/* Flow Control Watermark */
	#define SPI_FIFO0			(0UL<<16)       /* Flow Control Watermark FCWM: TFIFO empty or RFIFO full */
	#define SPI_FIFO1			(1UL<<16)       /* Flow Control Watermark FCWM: TFIFO 75% or more empty, or RFIFO 75% or more full */
	#define SPI_FIFO2			(2UL<<16)       /* Flow Control Watermark FCWM: TFIFO 50% or more empty, or RFIFO 50% or more full */
	#define SPI_FCPL			(1UL<<15)       /* Flow Control Polarity */
	#define SPI_FLOW_LO			(0UL<<15)       /* Flow Control Polarity FCPL: Active-low RDY */
	#define SPI_FLOW_HI			(1UL<<15)       /* Flow Control Polarity FCPL: Active-high RDY*/
	#define SPI_FCCH			(1UL<<14)       /* Flow Control Channel Selection */
	#define SPI_FLOW_RX			(0UL<<14)       /* Flow Control Channel Selection FCCH: Flow control on RX buffer*/
	#define SPI_FLOW_TX			(1UL<<14)       /* Flow Control Channel Selection FCCH: Flow control on TX buffer*/
	#define SPI_FCEN			(1UL<<13)       /* Flow Control Enable */
	#define SPI_FLOW_DIS		(0UL<<13)       /* Flow Control Enable FCEN: Disable*/
	#define SPI_FLOW_EN 		(1UL<<13)       /* Flow Control Enable FCEN: Enable */
	#define SPI_LSBF			(1UL<<12)       /* Least Significant Bit First */
	#define SPI_MSB_FIRST		(0UL<<12)       /* Least Significant Bit First LSBF: MSB sent/received first (big endian)	*/
	#define SPI_LSB_FIRST		(1UL<<12)       /* Least Significant Bit First LSBF: LSB sent/received first (little endian)*/
	#define SPI_SIZE8			(0UL<<9)		/* Word Transfer Size: 8-bit word */
	#define SPI_SIZE16			(1UL<<9)		/* Word Transfer Size: 16-bit word */
	#define SPI_SIZE32			(2UL<<9)		/* Word Transfer Size: 32-bit word */
	#define SPI_EMISO			(1UL<<8)		/* Enable MISO */
	#define SPI_SELST			(1UL<<7)		/* Slave Select Polarity Between Transfers */
	#define SPI_DEASSRT_SSEL	(0UL<<7)		/* Slave Select Polarity Between Transfers SELST: De-assert slave select (high) */
	#define SPI_ASSRT_SSEL  	(1UL<<7)		/* Slave Select Polarity Between Transfers SELST: Assert slave select (low)		*/
	#define SPI_ASSEL			(1UL<<6)		/* Slave Select Pin Control */
	#define SPI_SW_SSEL			(0UL<<6)		/* Slave Select Pin Control ASSEL: Software Slave Select Control */
	#define SPI_HW_SSEL			(1UL<<6)		/* Slave Select Pin Control ASSEL: Hardware Slave Select Control */
	#define SPI_CPOL			(1UL<<5)		/* Clock Polarity */
	#define SPI_SCKHI			(0UL<<5)		/* Clock Polarity CPOL: Active-high SPI CLK*/
	#define SPI_SCKLO			(1UL<<5)		/* Clock Polarity CPOL: Active-low SPI CLK */
	#define SPI_CPHA			(1UL<<4)		/* Clock Phase */
	#define SPI_SCKMID			(0UL<<4)		/* Clock Phase CPHA: SPI CLK toggles from middle*/
	#define SPI_SCKBEG			(1UL<<4)		/* Clock Phase CPHA: SPI CLK toggles from start */
	#define SPI_ODM				(1UL<<3)        /* Open Drain Mode */
	#define SPI_PSSE			(1UL<<2)        /* Protected Slave Select Enable */
	#define SPI_MSTR			(1UL<<1)        /* Master/Slave */
	#define SPI_SLAVE 			(0UL<<1)        /* Master/Slave MSTR: Slave */
	#define SPI_MASTER			(1UL<<1)        /* Master/Slave MSTR: Master */
	#define SPI_EN				(1UL<<0)        /* Enable */ 
													
	#define SPI_TFF				(1UL<<23)       /* SPI_TFIFO Full */
	#define SPI_RFE				(1UL<<22)       /* SPI_RFIFO Empty */
	#define SPI_FCS				(1UL<<20)       /* Flow Control Stall Indication */
	#define SPI_TFS_MASK		(7UL<<16)       /* SPI_TFIFO Status */
	#define SPI_TFS_FULL		(0UL<<16)       /* TFS: Full TFIFO */
	#define SPI_TFS_25			(1UL<<16)       /* TFS: 25% empty TFIFO */
	#define SPI_TFS_50			(2UL<<16)       /* TFS: 50% empty TFIFO */
	#define SPI_TFS_75			(3UL<<16)       /* TFS: 75% empty TFIFO */
	#define SPI_TFS_EMPTY		(4UL<<16)       /* TFS: Empty TFIFO */
	#define SPI_RFS_MASK		(7UL<<12)       /* SPI_RFIFO Status */
	#define SPI_RFS_EMPTY		(0UL<<16)       /* RFS: Empty RFIFO */
	#define SPI_RFS_25			(1UL<<16)       /* RFS: 25% full RFIFO */
	#define SPI_RFS_50			(2UL<<16)       /* RFS: 50% full RFIFO */
	#define SPI_RFS_75			(3UL<<16)       /* RFS: 75% full RFIFO */
	#define SPI_RFS_FULL		(4UL<<16)       /* RFS: Full RFIFO */
	#define SPI_TF 				(1UL<<11)       /* Transmit Finish Indication */
	#define SPI_RF 				(1UL<<10)       /* Receive Finish Indication */
	#define SPI_TS  			(1UL<<9)		/* Transmit Start */
	#define SPI_RS  			(1UL<<8)        /* Receive Start */
	#define SPI_MF  			(1UL<<7)        /* Mode Fault Indication */
	#define SPI_TC  			(1UL<<6)        /* Transmit Collision Indication */
	#define SPI_TUR 			(1UL<<5)        /* Transmit Underrun Indication */
	#define SPI_ROR 			(1UL<<4)        /* Receive Overrun Indication */
	#define SPI_TUWM			(1UL<<2)        /* Transmit Urgent Watermark Breached */
	#define SPI_RUWM			(1UL<<1)        /* Receive Urgent Watermark Breached */
	#define SPI_SPIF			(1UL<<0)        /* SPI Finished */ 

	#define RXCTL_RUWM  		(1UL<<16)   	/* Receive FIFO Urgent Watermark */
	#define RXCTL_RRWM  		(1UL<<12)   	/* Receive FIFO Regular Watermark */
	#define RXCTL_RDO   		(1UL<< 8)   	/* Receive Data Overrun */
	//#define RXCTL_RDR   		(1UL<< 4)   	/* Receive Data Request */
	#define RXCTL_RDR_DIS		(0UL<< 4)		/* RDR: Disabled */
	#define RXCTL_RDR_NE		(1UL<< 4)		/* RDR: Not empty RFIFO */
	#define RXCTL_RDR_25		(2UL<< 4)		/* RDR: 25% full RFIFO */
	#define RXCTL_RDR_50		(3UL<< 4)		/* RDR: 50% full RFIFO */
	#define RXCTL_RDR_75		(4UL<< 4)		/* RDR: 75% full RFIFO */
	#define RXCTL_RDR_FULL		(5UL<< 4)		/* RDR: Full RFIFO */ 
	#define RXCTL_RWCEN 		(1UL<< 3)   	/* Receive Word Counter Enable */
	#define RXCTL_RTI   		(1UL<< 2)   	/* Receive Transfer Initiate */
	#define RXCTL_REN   		(1UL<< 0)   	/* Receive Enable */ 
	#define TXCTL_TUWM  		(1UL<<16)   	/* FIFO Urgent Watermark */
	#define TXCTL_TRWM  		(1UL<<12)   	/* FIFO Regular Watermark */
	#define TXCTL_TDU   		(1UL<< 8)   	/* Transmit Data Under-run */
	//#define TXCTL_TDR   		(1UL<< 4)   	/* Transmit Data Request */
	#define TXCTL_TDR_DIS		(0UL<< 4)		/* TDR: Disabled */
	#define TXCTL_TDR_NF		(1UL<< 4)		/* TDR: Not full TFIFO */
	#define TXCTL_TDR_25		(2UL<< 4)		/* TDR: 25% empty TFIFO */
	#define TXCTL_TDR_50		(3UL<< 4)		/* TDR: 50% empty TFIFO */
	#define TXCTL_TDR_75		(4UL<< 4)		/* TDR: 75% empty TFIFO */
	#define TXCTL_TDR_EMPTY		(5UL<< 4)		/* TDR: Empty TFIFO */ 	
	#define TXCTL_TWCEN 		(1UL<< 3)   	/* Transmit Word Counter Enable */
	#define TXCTL_TTI   		(1UL<< 2)   	/* Transmit Transfer Initiate */
	#define TXCTL_TEN   		(1UL<< 0)   	/* Transmit Enable */ 

	#define SPI_LAGX			(1UL<<9)		/* Extended SPI Clock Lag Control */
	#define SPI_LEADX			(1UL<<8)		/* Extended SPI Clock Lead Control */
	#define SPI_STOP(v)			(((v)&0xFF)<<0)	/* Transfer delay time in multiples of SPI clock period */

	#define SPI_SSEL7			(1UL<<15)		/* Slave Select 7 Input */
	#define SPI_SSEL6			(1UL<<14)		/* Slave Select 6 Input */
	#define SPI_SSEL5			(1UL<<13)		/* Slave Select 5 Input */
	#define SPI_SSEL4			(1UL<<12)		/* Slave Select 4 Input */
	#define SPI_SSEL3			(1UL<<11)		/* Slave Select 3 Input */
	#define SPI_SSEL2			(1UL<<10)		/* Slave Select 2 Input */
	#define SPI_SSEL1			(1UL<<9)		/* Slave Select 1 Input */
	#define SPI_SSE7			(1UL<<7)		/* Slave Select 7 Enable */
	#define SPI_SSE6			(1UL<<6)		/* Slave Select 6 Enable */
	#define SPI_SSE5			(1UL<<5)		/* Slave Select 5 Enable */
	#define SPI_SSE4			(1UL<<4)		/* Slave Select 4 Enable */
	#define SPI_SSE3			(1UL<<3)		/* Slave Select 3 Enable */
	#define SPI_SSE2			(1UL<<2)		/* Slave Select 2 Enable */
	#define SPI_SSE1			(1UL<<1)		/* Slave Select 1 Enable */

	#define SPI_CMDPINS			(1UL<<29)       /* Pins Used for Command: 0 - Use only one pin: MOSI (overrides SPI_CTL.MIOM bits); 1 - Use pins specified by SPI_CTL.MIOM bits */
	#define SPI_CMDSKIP			(1UL<<28)       /* Command Skip Enable */
	#define SPI_WRAP			(1UL<<27)       /* SPI Memory Wrap Indicator */
	#define SPI_MERGE			(1UL<<26)       /* Merge Enable */
	#define SPI_TRIDMY(v)		(((v)&3)<<24)   /* Tristate Dummy Timing */
	#define SPI_TRIDMY_IMMED	(0UL<<24)		/* Tristate Dummy Timing: Tristate outputs immediately */
	#define SPI_TRIDMY_4BITS	(1UL<<24)		/* Tristate Dummy Timing: Tristate outputs after 4 bits of dummy/mode are transmitted */
	#define SPI_TRIDMY_8BITS	(2UL<<24)		/* Tristate Dummy Timing: Tristate outputs after 8 bits of dummy/mode are transmitted */
	#define SPI_TRIDMY_NEVER	(3UL<<24)		/* Tristate Dummy Timing: Never tristate outputs (previously specified output state is held) */
	#define SPI_MODE(v)			(((v)&0xFF)<<16)/* Mode Field */
	#define SPI_DMYSIZE(v)		(((v)&7)<<12)   /* Bytes of Dummy/Mode */
	#define SPI_ADRPINS			(1UL<<11)       /* Pins Used for Address: 0 - Use only one pin: MOSI (overrides SPI_CTL.MIOM bits); 1 - Use pins specified by SPI_CTL.MIOM bits */
	#define SPI_ADRSIZE(v)		(((v)&7)<<8)    /* Bytes of Read Address */
	#define SPI_OPCODE(v)		(((v)&0xFF)<<0) /* Read Opcode */


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_PORT
	{
		BF_RW32	FER;                           /*!< Port x Function Enable Register */
		BF_RW32	FER_SET;                       /*!< Port x Function Enable Set Register */
		BF_RW32	FER_CLR;                       /*!< Port x Function Enable Clear Register */
		BF_RW32	DATA;                          /*!< Port x GPIO Data Register */
		BF_RW32	DATA_SET;                      /*!< Port x GPIO Data Set Register */
		BF_RW32	DATA_CLR;                      /*!< Port x GPIO Data Clear Register */
		BF_RW32	DIR;                           /*!< Port x GPIO Direction Register */
		BF_RW32	DIR_SET;                       /*!< Port x GPIO Direction Set Register */
		BF_RW32	DIR_CLR;                       /*!< Port x GPIO Direction Clear Register */
		BF_RW32	INEN;                          /*!< Port x GPIO Input Enable Register */
		BF_RW32	INEN_SET;                      /*!< Port x GPIO Input Enable Set Register */
		BF_RW32	INEN_CLR;                      /*!< Port x GPIO Input Enable Clear Register */
		BF_RW32	MUX;                           /*!< Port x Multiplexer Control Register */
		BF_RW32	DATA_TGL;                      /*!< Port x GPIO Input Enable Toggle Register */
		BF_RW32	POL;                           /*!< Port x GPIO Polarity Invert Register */
		BF_RW32	POL_SET;                       /*!< Port x GPIO Polarity Invert Set Register */
		BF_RW32	POL_CLR;                       /*!< Port x GPIO Polarity Invert Clear Register */
		BF_RW32	LOCK;                          /*!< Port x GPIO Lock Register */
		BF_RW32	TRIG_TGL;                      /*!< Port x GPIO Trigger Toggle Register */ 	

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

		inline void	SetFER(u16 m)		{ FER_SET = m; }
		inline void	ClrFER(u16 m)		{ FER_CLR = m; }

		inline void	SetMUX(byte pin, byte v) { MUX = (MUX & ~(3UL<<pin)) | ((v&3)<<pin); }

		inline void	ClearTriggerIRQ(u32 m)	{ ((S_PINT*)(((u32)this & ~0x1FF)|(((u32)this & 0x1FF)<<1)|0x1000))->LATCH = m; }
	};

	typedef S_PORT S_PORTA, S_PORTB, S_PORTC, S_PIO, S_PIOA, S_PIOB, S_PIOC;

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

#pragma diag(push)
#pragma diag(suppress: 1967)

	MK_PTR(L1DM 	,	ADI_L1DM_BASE       ); /*!<  Pointer to Data Memory Unit (L1DM) */
	MK_PTR(L1IM 	,	ADI_L1IM_BASE       ); /*!<  Pointer to Instruction Memory Unit (L1IM) */
	MK_PTR(ICU  	,	ADI_ICU_BASE        ); /*!<  Pointer to Interrupt Controller (ICU) */
	MK_PTR(TMR  	,	ADI_TMR_BASE        ); /*!<  Pointer to Core Timer (TMR) */
	MK_PTR(PF   	,	ADI_PF_BASE         ); /*!<  Pointer to Performance Monitor (PF) */
	MK_PTR(OPT  	,	ADI_OPT_BASE        ); /*!<  Pointer to Optional Feature  (OPT) */
	MK_PTR(BP   	,	ADI_BP_BASE         ); /*!<  Pointer to Branch Predictor (BP) */
	MK_PTR(RCU  	,	ADI_RCU0_BASE       ); /*!<  Pointer to Reset Control Unit (RCU0) */
	MK_PTR(TRU  	,	ADI_TRU0_BASE       ); /*!<  Pointer to Trigger Routing Unit (TRU0) */
	MK_PTR(CGU  	,	ADI_CGU0_BASE       ); /*!<  Pointer to Clock Generation Unit (CGU0) */
	MK_PTR(DPM  	,	ADI_DPM0_BASE       ); /*!<  Pointer to Dynamic Power Management (DPM0) */
	MK_PTR(SEC  	,	ADI_SEC0_BASE       ); /*!<  Pointer to System Event Controller (SEC0) */
	MK_PTR(SPU  	,	ADI_SPU0_BASE       ); /*!<  Pointer to System Protection Unit (SPU0) */
	MK_PTR(SMPU0	,	ADI_SMPU0_BASE      ); /*!<  Pointer to System Memory Protection Unit (SMPU0) */
	MK_PTR(SMPU1	,	ADI_SMPU1_BASE      ); /*!<  Pointer to System Memory Protection Unit (SMPU1) */
	MK_PTR(L2CTL	,	ADI_L2CTL0_BASE     ); /*!<  Pointer to L2 Memory Controller (L2CTL0) */
	MK_PTR(SWU0		,	ADI_SWU0_BASE       ); /*!<  Pointer to System WatchPoint Unit (SWU0) */
	MK_PTR(SWU1		,	ADI_SWU1_BASE       ); /*!<  Pointer to System WatchPoint Unit (SWU1) */
	MK_PTR(SWU2		,	ADI_SWU2_BASE       ); /*!<  Pointer to System Watchpoint Unit (SWU2) */
	MK_PTR(SWU3		,	ADI_SWU3_BASE       ); /*!<  Pointer to System WatchPoint Unit (SWU3) */
	MK_PTR(SWU4		,	ADI_SWU4_BASE       ); /*!<  Pointer to System WatchPoint Unit (SWU4) */
	MK_PTR(SWU5		,	ADI_SWU5_BASE       ); /*!<  Pointer to System WatchPoint Unit (SWU5) */
	MK_PTR(SWU6		,	ADI_SWU6_BASE       ); /*!<  Pointer to System WatchPoint Unit (SWU6) */
	MK_PTR(SWU7		,	ADI_SWU7_BASE       ); /*!<  Pointer to System WatchPoint Unit (SWU7) */
	MK_PTR(PADS 	,	ADI_PADS0_BASE      ); /*!<  Pointer to Pads Controller (PADS0) */
	MK_PTR(PINT0	,	ADI_PINT0_BASE      ); /*!<  Pointer to PINT (PINT0) */
	MK_PTR(PINT1	,	ADI_PINT1_BASE      ); /*!<  Pointer to PINT (PINT1) */
	MK_PTR(PINT2	,	ADI_PINT2_BASE      ); /*!<  Pointer to PINT (PINT2) */
	MK_PTR(TIMER	,	ADI_TIMER0_BASE     ); /*!<  Pointer to General-Purpose Timer Block (TIMER0) */
	MK_PTR(WDOG		,	ADI_WDOG0_BASE      ); /*!<  Pointer to Watch Dog Timer Unit (WDOG0) */
	MK_PTR(WDT		,	ADI_WDOG0_BASE      ); /*!<  Pointer to Watch Dog Timer Unit (WDOG0) */
	MK_PTR(TAPC		,	ADI_TAPC0_BASE      ); /*!<  Pointer to Test Access Port Controller (TAPC0) */
	MK_PTR(TWI		,	ADI_TWI0_BASE       ); /*!<  Pointer to 2-Wire Interface (TWI0) */
	MK_PTR(SPORT0	,	ADI_SPORT0_BASE     ); /*!<  Pointer to Serial Port (SPORT0) */
	MK_PTR(SPORT1	,	ADI_SPORT1_BASE     ); /*!<  Pointer to Serial Port (SPORT1) */
	MK_PTR(SPI0  	,	ADI_SPI0_BASE       ); /*!<  Pointer to Serial Peripheral Interface (SPI0) */
	MK_PTR(SPI1  	,	ADI_SPI1_BASE       ); /*!<  Pointer to Serial Peripheral Interface (SPI1) */
	MK_PTR(SPI2  	,	ADI_SPI2_BASE       ); /*!<  Pointer to Serial Peripheral Interface (SPI2) */
	MK_PTR(SPIHP	,	ADI_SPIHP0_BASE     ); /*!<  Pointer to SPI Host Port (SPIHP0) */
	MK_PTR(SMC		,	ADI_SMC0_BASE       ); /*!<  Pointer to Static Memory Controller (SMC0) */
	MK_PTR(UART0  	,	ADI_UART0_BASE      ); /*!<  Pointer to UART (UART0) */
	MK_PTR(UART1  	,	ADI_UART1_BASE      ); /*!<  Pointer to UART (UART1) */
	MK_PTR(EPPI	  	,	ADI_EPPI0_BASE      ); /*!<  Pointer to Parallel Peripheral Interface (EPPI0) */
	MK_PTR(CNT		,	ADI_CNT0_BASE       ); /*!<  Pointer to CNT (CNT0) */
	MK_PTR(MSI		,	ADI_MSI0_BASE       ); /*!<  Pointer to Media Services Interface (MSI0) */
	MK_PTR(DMA		,	ADI_DMA0_BASE       ); /*!<  Pointer to DMA Channel (DMA0) */
	 //MK_PTR(DMA0   ,	ADI_DMA0_BASE       ); /*!<  Pointer to DMA Channel (SPORT0_A_DMA) */
	 //MK_PTR(DMA1   ,	ADI_DMA1_BASE       ); /*!<  Pointer to DMA Channel (DMA1) */
	 //MK_PTR(DMA1   ,	ADI_DMA1_BASE       ); /*!<  Pointer to DMA Channel (SPORT0_B_DMA) */
	 //MK_PTR(DMA2   ,	ADI_DMA2_BASE       ); /*!<  Pointer to DMA Channel (DMA2) */
	 //MK_PTR(DMA2   ,	ADI_DMA2_BASE       ); /*!<  Pointer to DMA Channel (SPORT1_A_DMA) */
	 //MK_PTR(DMA3   ,	ADI_DMA3_BASE       ); /*!<  Pointer to DMA Channel (DMA3) */
	 //MK_PTR(DMA3   ,	ADI_DMA3_BASE       ); /*!<  Pointer to DMA Channel (SPORT1_B_DMA) */
	 //MK_PTR(DMA4   ,	ADI_DMA4_BASE       ); /*!<  Pointer to DMA Channel (DMA4) */
	 //MK_PTR(DMA4   ,	ADI_DMA4_BASE       ); /*!<  Pointer to DMA Channel (SPI0_TXDMA) */
	 //MK_PTR(DMA5   ,	ADI_DMA5_BASE       ); /*!<  Pointer to DMA Channel (DMA5) */
	 //MK_PTR(DMA5   ,	ADI_DMA5_BASE       ); /*!<  Pointer to DMA Channel (SPI0_RXDMA) */
	 //MK_PTR(DMA6   ,	ADI_DMA6_BASE       ); /*!<  Pointer to DMA Channel (DMA6) */
	 //MK_PTR(DMA6   ,	ADI_DMA6_BASE       ); /*!<  Pointer to DMA Channel (SPI1_TXDMA) */
	 //MK_PTR(DMA7   ,	ADI_DMA7_BASE       ); /*!<  Pointer to DMA Channel (DMA7) */
	 //MK_PTR(DMA7   ,	ADI_DMA7_BASE       ); /*!<  Pointer to DMA Channel (SPI1_RXDMA) */
	 //MK_PTR(DMA8   ,	ADI_DMA8_BASE       ); /*!<  Pointer to DMA Channel (DMA8) */
	 //MK_PTR(DMA8   ,	ADI_DMA8_BASE       ); /*!<  Pointer to DMA Channel (SPI2_TXDMA) */
	 //MK_PTR(DMA9   ,	ADI_DMA9_BASE       ); /*!<  Pointer to DMA Channel (DMA9) */
	 //MK_PTR(DMA9   ,	ADI_DMA9_BASE       ); /*!<  Pointer to DMA Channel (SPI2_RXDMA) */
	 //MK_PTR(DMA10  ,	ADI_DMA10_BASE      ); /*!<  Pointer to DMA Channel (DMA10) */
	 //MK_PTR(DMA10  ,	ADI_DMA10_BASE      ); /*!<  Pointer to DMA Channel (UART0_TXDMA) */
	 //MK_PTR(DMA11  ,	ADI_DMA11_BASE      ); /*!<  Pointer to DMA Channel (DMA11) */
	 //MK_PTR(DMA11  ,	ADI_DMA11_BASE      ); /*!<  Pointer to DMA Channel (UART0_RXDMA) */
	 //MK_PTR(DMA12  ,	ADI_DMA12_BASE      ); /*!<  Pointer to DMA Channel (DMA12) */
	 //MK_PTR(DMA12  ,	ADI_DMA12_BASE      ); /*!<  Pointer to DMA Channel (UART1_TXDMA) */
	 //MK_PTR(DMA13  ,	ADI_DMA13_BASE      ); /*!<  Pointer to DMA Channel (DMA13) */
	 //MK_PTR(DMA13  ,	ADI_DMA13_BASE      ); /*!<  Pointer to DMA Channel (UART1_RXDMA) */
	 //MK_PTR(DMA14  ,	ADI_DMA14_BASE      ); /*!<  Pointer to DMA Channel (DMA14) */
	 //MK_PTR(DMA14  ,	ADI_DMA14_BASE      ); /*!<  Pointer to DMA Channel (EPPI0_CH0_DMA) */
	 //MK_PTR(DMA15  ,	ADI_DMA15_BASE      ); /*!<  Pointer to DMA Channel (DMA15) */
	 //MK_PTR(DMA15  ,	ADI_DMA15_BASE      ); /*!<  Pointer to DMA Channel (EPPI0_CH1_DMA) */
	 //MK_PTR(DMA16  ,	ADI_DMA16_BASE      ); /*!<  Pointer to DMA Channel (DMA16) */
	 //MK_PTR(MDMA0S  ,	ADI_DMA16_BASE      ); /*!<  Pointer to DMA Channel (MDMA0_SRC) */ 
	 //MK_PTR(DMA17  ,	ADI_DMA17_BASE      ); /*!<  Pointer to DMA Channel (DMA17) */
	 //MK_PTR(MDMA0D  ,	ADI_DMA17_BASE      ); /*!<  Pointer to DMA Channel (MDMA0_DST) */
	 //MK_PTR(DMA18  ,	ADI_DMA18_BASE      ); /*!<  Pointer to DMA Channel (DMA18) */
	 //MK_PTR(DMA18  ,	ADI_DMA18_BASE      ); /*!<  Pointer to DMA Channel (MDMA1_SRC) */
	 //MK_PTR(DMA19  ,	ADI_DMA19_BASE      ); /*!<  Pointer to DMA Channel (DMA19) */
	 //MK_PTR(DMA19  ,	ADI_DMA19_BASE      ); /*!<  Pointer to DMA Channel (MDMA1_DST) */
	 //MK_PTR(DMA20  ,	ADI_DMA20_BASE      ); /*!<  Pointer to DMA Channel (DMA20) */
	 //MK_PTR(DMA20  ,	ADI_DMA20_BASE      ); /*!<  Pointer to DMA Channel (MDMA2_SRC) */
	 //MK_PTR(DMA21  ,	ADI_DMA21_BASE      ); /*!<  Pointer to DMA Channel (DMA21) */
	 //MK_PTR(DMA21  ,	ADI_DMA21_BASE      ); /*!<  Pointer to DMA Channel (MDMA2_DST) */
	MK_PTR(CRC0   ,	ADI_CRC0_BASE       ); /*!<  Pointer to Cyclic Redundancy Check Unit (CRC0) */
	MK_PTR(CRC1   ,	ADI_CRC1_BASE       ); /*!<  Pointer to Cyclic Redundancy Check Unit (CRC1) */
	MK_PTR(RTC	  ,	ADI_RTC0_BASE       ); /*!<  Pointer to Real Time Clock (RTC0) */
	MK_PTR(TRNG	,	ADI_TRNG0_BASE      ); /*!<  Pointer to  (TRNG0) */
	MK_PTR(SCB   ,	ADI_SCB0_BASE       ); /*!<  Pointer to System Cross Bar (SCB0) */ 	
		
	MK_PTR(PORTA,	ADI_PORTA_BASE);
	MK_PTR(PORTB,	ADI_PORTB_BASE);
	MK_PTR(PORTC,	ADI_PORTC_BASE);

	MK_PTR(PIOA,	ADI_PORTA_BASE);
	MK_PTR(PIOB,	ADI_PORTB_BASE);
	MK_PTR(PIOC,	ADI_PORTC_BASE);


#pragma diag(pop)


	inline void ResetWDT()		{ WDOG->Reset();	}
	inline void DisableWDT()	{ WDOG->Disable();	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	inline bool RamCheck(const void *ptr)
	{
		u32 v = (u32)ptr;

		return ((v&~0x107FFF) == 0x11800000 || (v&~0xFFFFF) == 0x8000000);

	};

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


} // namespace HW


#define	PID_SEC0_ERR				SEC0_ERR_IRQn                      /*!< Fault Interrupt */
#define	PID_CGU0_EVT				CGU0_EVT_IRQn                      /*!< Event */
#define	PID_WDOG0_EXP				WDOG0_EXP_IRQn                     /*!< Expiration */
#define	PID_L2CTL0_ECC_ERR			L2CTL0_ECC_ERR_IRQn                /*!< ECC Error */
#define	PID_C0_DBL_FAULT			C0_DBL_FAULT_IRQn                  /*!< Core 0 Double Fault */
#define	PID_C0_HW_ERR				C0_HW_ERR_IRQn                     /*!< Core 0 Hardware Error */
#define	PID_C0_NMI_L1_PARITY_ERR	C0_NMI_L1_PARITY_ERR_IRQn          /*!< Core 0 Unhandled NMI or L1 Memory Parity Error */
#define	PID_L2CTL0_EVT				L2CTL0_EVT_IRQn                    /*!< L2 Event (Scrub or Initialization) */
#define	PID_TIMER0_TMR0				TIMER0_TMR0_IRQn                   /*!< Timer 0 */
#define	PID_TIMER0_TMR1				TIMER0_TMR1_IRQn                   /*!< Timer 1 */
#define	PID_TIMER0_TMR2				TIMER0_TMR2_IRQn                   /*!< Timer 2 */
#define	PID_TIMER0_TMR3				TIMER0_TMR3_IRQn                   /*!< Timer 3 */
#define	PID_TIMER0_TMR4				TIMER0_TMR4_IRQn                   /*!< Timer 4 */
#define	PID_TIMER0_TMR5				TIMER0_TMR5_IRQn                   /*!< Timer 5 */
#define	PID_TIMER0_TMR6				TIMER0_TMR6_IRQn                   /*!< Timer 6 */
#define	PID_TIMER0_TMR7				TIMER0_TMR7_IRQn                   /*!< Timer 7 */
#define	PID_TIMER0_STAT				TIMER0_STAT_IRQn                   /*!< Status */
#define	PID_PINT0_BLOCK				PINT0_BLOCK_IRQn                   /*!< Pin Interrupt Block */
#define	PID_PINT1_BLOCK				PINT1_BLOCK_IRQn                   /*!< Pin Interrupt Block */
#define	PID_PINT2_BLOCK				PINT2_BLOCK_IRQn                   /*!< Pin Interrupt Block */
#define	PID_EPPI0_STAT				EPPI0_STAT_IRQn                    /*!< Status */
#define	PID_EPPI0_CH0_DMA			EPPI0_CH0_DMA_IRQn                 /*!< Channel 0 DMA */
#define	PID_EPPI0_CH1_DMA			EPPI0_CH1_DMA_IRQn                 /*!< Channel 1 DMA */
#define	PID_DMAC_ERROR				DMAC_ERROR_IRQn                    /*!< DDE Aggregated Error */
#define	PID_CNT0_STAT				CNT0_STAT_IRQn                     /*!< Status */
#define	PID_SPORT0_A_STAT			SPORT0_A_STAT_IRQn                 /*!< Channel A Status */
#define	PID_SPORT0_A_DMA			SPORT0_A_DMA_IRQn                  /*!< Channel A DMA */
#define	PID_SPORT0_B_STAT			SPORT0_B_STAT_IRQn                 /*!< Channel B Status */
#define	PID_SPORT0_B_DMA			SPORT0_B_DMA_IRQn                  /*!< Channel B DMA */
#define	PID_SPORT1_A_STAT			SPORT1_A_STAT_IRQn                 /*!< Channel A Status */
#define	PID_SPORT1_A_DMA			SPORT1_A_DMA_IRQn                  /*!< Channel A DMA */
#define	PID_SPORT1_B_STAT			SPORT1_B_STAT_IRQn                 /*!< Channel B Status */
#define	PID_SPORT1_B_DMA			SPORT1_B_DMA_IRQn                  /*!< Channel B DMA */
#define	PID_SPI0_ERR				SPI0_ERR_IRQn                      /*!< Error */
#define	PID_SPI0_STAT				SPI0_STAT_IRQn                     /*!< Status */
#define	PID_SPI0_TXDMA				SPI0_TXDMA_IRQn                    /*!< TX DMA Channel */
#define	PID_SPI0_RXDMA				SPI0_RXDMA_IRQn                    /*!< RX DMA Channel */
#define	PID_SPI1_ERR				SPI1_ERR_IRQn                      /*!< Error */
#define	PID_SPI1_STAT				SPI1_STAT_IRQn                     /*!< Status */
#define	PID_SPI1_TXDMA				SPI1_TXDMA_IRQn                    /*!< TX DMA Channel */
#define	PID_SPI1_RXDMA				SPI1_RXDMA_IRQn                    /*!< RX DMA Channel */
#define	PID_SPI2_ERR				SPI2_ERR_IRQn                      /*!< Error */
#define	PID_SPI2_STAT				SPI2_STAT_IRQn                     /*!< Status */
#define	PID_SPI2_TXDMA				SPI2_TXDMA_IRQn                    /*!< TX Channel (non-DMA) */
#define	PID_SPI2_RXDMA				SPI2_RXDMA_IRQn                    /*!< RX Channel (non-DMA) */
#define	PID_UART0_STAT				UART0_STAT_IRQn                    /*!< Status */
#define	PID_UART0_TXDMA				UART0_TXDMA_IRQn                   /*!< Transmit DMA */
#define	PID_UART0_RXDMA				UART0_RXDMA_IRQn                   /*!< Receive DMA */
#define	PID_UART1_STAT				UART1_STAT_IRQn                    /*!< Status */
#define	PID_UART1_TXDMA				UART1_TXDMA_IRQn                   /*!< Transmit DMA */
#define	PID_UART1_RXDMA				UART1_RXDMA_IRQn                   /*!< Receive DMA */
#define	PID_MDMA0_SRC				MDMA0_SRC_IRQn                     /*!< Memory DMA Stream 0 Source */
#define	PID_MDMA0_DST				MDMA0_DST_IRQn                     /*!< Memory DMA Stream 0 Destination */
#define	PID_MDMA1_SRC				MDMA1_SRC_IRQn                     /*!< Memory DMA Stream 1 Source / CRC0 Input Channel */
#define	PID_MDMA1_DST				MDMA1_DST_IRQn                     /*!< Memory DMA Stream 1 Destination / CRC0 Output Channel */
#define	PID_MDMA2_SRC				MDMA2_SRC_IRQn                     /*!< Memory DMA Stream 2 Source / CRC1 Input Channel */
#define	PID_MDMA2_DST				MDMA2_DST_IRQn                     /*!< Memory DMA Stream 2 Destination / CRC1 Output Channel */
#define	PID_RTC0_EVT				RTC0_EVT_IRQn                      /*!< Event */
#define	PID_TWI0_DATA				TWI0_DATA_IRQn                     /*!< Data Interrupt */
#define	PID_CRC0_DCNTEXP			CRC0_DCNTEXP_IRQn                  /*!< Datacount expiration */
#define	PID_CRC0_ERR				CRC0_ERR_IRQn                      /*!< Error */
#define	PID_CRC1_DCNTEXP			CRC1_DCNTEXP_IRQn                  /*!< CRC1 Data Count Expiration */
#define	PID_CRC1_ERR				CRC1_ERR_IRQn                      /*!< CRC1 Error */
#define	PID_PKTE0_IRQ				PKTE0_IRQ_IRQn                     /*!< Security Packet Engine Interrupt */
#define	PID_PKIC0_IRQ				PKIC0_IRQ_IRQn                     /*!< Public Key Interrupt */
#define	PID_OTPC0_ERR				OTPC0_ERR_IRQn                     /*!< Dual-bit error */
#define	PID_MSI0_STAT				MSI0_STAT_IRQn                     /*!< Status */
#define	PID_SMPU0_ERR				SMPU0_ERR_IRQn                     /*!< SMPU Error (DMA L2) */
#define	PID_SMPU1_ERR				SMPU1_ERR_IRQn                     /*!< SMPU Error (DMC) */
#define	PID_SPU0_INT				SPU0_INT_IRQn                      /*!< Interrupt */
#define	PID_USB0_STAT				USB0_STAT_IRQn                     /*!< Status/FIFO Data Ready */
#define	PID_USB0_DATA				USB0_DATA_IRQn                     /*!< DMA Status/Transfer Complete */
#define	PID_TRU0_SLV0				TRU0_SLV0_IRQn                     /*!< TRU0 Slave 0 */
#define	PID_TRU0_SLV1				TRU0_SLV1_IRQn                     /*!< TRU0 Slave 1 */
#define	PID_TRU0_SLV2				TRU0_SLV2_IRQn                     /*!< TRU0 Slave 2 */
#define	PID_TRU0_SLV3				TRU0_SLV3_IRQn                     /*!< TRU0 Slave 3 */
#define	PID_CGU0_ERR				CGU0_ERR_IRQn                      /*!< Error */
#define	PID_DPM0_EVT				DPM0_EVT_IRQn                      /*!< Event */
#define	PID_SPIHP0_ERR				SPIHP0_ERR_IRQn                    /*!< Error */
#define	PID_SOFT0_INT				SOFT0_INT_IRQn                     /*!< Software-Driven Interrupt 0 */
#define	PID_SOFT1_INT				SOFT1_INT_IRQn                     /*!< Software-Driven Interrupt 1 */
#define	PID_SOFT2_INT				SOFT2_INT_IRQn                     /*!< Software-Driven Interrupt 2 */
#define	PID_SOFT3_INT				SOFT3_INT_IRQn                     /*!< Software-Driven Interrupt 3 */
#define	PID_CAN0_RX					CAN0_RX_IRQn                       /*!< Receive */
#define	PID_CAN0_TX					CAN0_TX_IRQn                       /*!< Transmit */
#define	PID_CAN0_STAT				CAN0_STAT_IRQn                     /*!< Status */
#define	PID_CAN1_RX					CAN1_RX_IRQn                       /*!< Recieve */
#define	PID_CAN1_TX					CAN1_TX_IRQn                       /*!< Transmit */
#define	PID_CAN1_STAT				CAN1_STAT_IRQn                     /*!< Status */
#define	PID_CTI0_C0_EVT				CTI0_C0_EVT_IRQn                   /*!< Core 0 CTI Event (CTI0) */
#define	PID_SWU0_EVT				SWU0_EVT_IRQn                      /*!< Event (L1) */
#define	PID_SWU1_EVT				SWU1_EVT_IRQn                      /*!< Event (Core L2) */
#define	PID_SWU2_EVT				SWU2_EVT_IRQn                      /*!< Event (DMA L2) */
#define	PID_SWU3_EVT				SWU3_EVT_IRQn                      /*!< Event (MMR) */
#define	PID_SWU4_EVT				SWU4_EVT_IRQn                      /*!< Event (DMC) */
#define	PID_SWU5_EVT				SWU5_EVT_IRQn                      /*!< Event (SMC) */
#define	PID_SWU6_EVT				SWU6_EVT_IRQn                      /*!< Event (SPIF) */
#define	PID_SWU7_EVT				SWU7_EVT_IRQn                      /*!< Event (OTP) */
#define	PID_TAPC_KEYFAIL			TAPC_KEYFAIL_IRQn                  /*!< User Key Fail Interrupt */ 


#define	SPORT0_A_DMA  				SPORT0_A_DMA_CHANn                /*!< Channel A DMA */
#define	SPORT0_B_DMA  				SPORT0_B_DMA_CHANn                /*!< Channel B DMA */
#define	SPORT1_A_DMA  				SPORT1_A_DMA_CHANn                /*!< Channel A DMA */
#define	SPORT1_B_DMA  				SPORT1_B_DMA_CHANn                /*!< Channel B DMA */
#define	SPI0_TX_DMA    				SPI0_TXDMA_CHANn                  /*!< TX DMA Channel */
#define	SPI0_RX_DMA    				SPI0_RXDMA_CHANn                  /*!< RX DMA Channel */
#define	SPI1_TX_DMA    				SPI1_TXDMA_CHANn                  /*!< TX DMA Channel */
#define	SPI1_RX_DMA    				SPI1_RXDMA_CHANn                  /*!< RX DMA Channel */
#define	SPI2_TX_DMA    				SPI2_TXDMA_CHANn                  /*!< TX Channel (non-DMA) */
#define	SPI2_RX_DMA    				SPI2_RXDMA_CHANn                  /*!< RX Channel (non-DMA) */
#define	UART0_TX_DMA   				UART0_TXDMA_CHANn                 /*!< Transmit DMA */
#define	UART0_RX_DMA   				UART0_RXDMA_CHANn                 /*!< Receive DMA */
#define	UART1_TX_DMA   				UART1_TXDMA_CHANn                 /*!< Transmit DMA */
#define	UART1_RX_DMA   				UART1_RXDMA_CHANn                 /*!< Receive DMA */
#define	EPPI0_CH0_DMA 				EPPI0_CH0_DMA_CHANn               /*!< Channel 0 DMA */
#define	EPPI0_CH1_DMA 				EPPI0_CH1_DMA_CHANn               /*!< Channel 1 DMA */
#define	MDMA0_SRC     				MDMA0_SRC_CHANn                   /*!< Memory DMA Stream 0 Source */
#define	MDMA0_DST     				MDMA0_DST_CHANn                   /*!< Memory DMA Stream 0 Destination */
#define	MDMA1_SRC     				MDMA1_SRC_CHANn                   /*!< Memory DMA Stream 1 Source / CRC0 Input Channel */
#define	MDMA1_DST     				MDMA1_DST_CHANn                   /*!< Memory DMA Stream 1 Destination / CRC0 Output Channel */
#define	MDMA2_SRC     				MDMA2_SRC_CHANn                   /*!< Memory DMA Stream 2 Source / CRC1 Input Channel */
#define	MDMA2_DST     				MDMA2_DST_CHANn                   /*!< Memory DMA Stream 2 Destination / CRC1 Output Channel */


#undef MK_PTR
#undef MKPID

#endif // BF706_H__16_07_2024__15_40
