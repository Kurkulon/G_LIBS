#include "spi.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define FLASHSPI_IMP_V2

#define BAUD_RATE_DIVISOR 	3
#define FLASH_START_ADR		0x00000000
#define BUFFER_SIZE			0x2000

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

#endif

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

//#define MAIN_LOOP_PIN_TGL()		{ HW::PIOF->NOT(PF4);}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef CPU_BF592

static u16 SPI_CS_MASK[] = { PF8 };

static S_SPIM	spi(0, HW::PIOF, SPI_CS_MASK, ArraySize(SPI_CS_MASK), SCLK);

#elif defined(CPU_BF706)

static u16 SPI_CS_MASK[] = { PB15 };

static S_SPIM	spi(2, HW::PIOB, SPI_CS_MASK, ArraySize(SPI_CS_MASK), SCLK);

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
