#include "core.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define BAUD_RATE_DIVISOR 	3
#define FLASH_START_ADDR	0x00000000
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

#elif defined(__ADSPBF70x__)

#define PIO_MAINLOOP	HW::PIOA
#define PIO_TST			HW::PIOB

#define PIN_MAINLOOP	13
#define PIN_TST			5

#define MAINLOOP	(1UL<<PIN_MAINLOOP)
#define TST			(1UL<<PIN_TST)		

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "ADSP\AFP_imp.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
