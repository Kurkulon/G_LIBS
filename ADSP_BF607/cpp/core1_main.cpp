#pragma default_section(STI,		"CORE1_CODE")
#pragma default_section(CODE,		"CORE1_CODE")
#pragma default_section(ALLDATA,	"CORE1_DATA")
#pragma default_section(SWITCH,		"CORE1_DATA")
#pragma default_section(VTABLE,		"CORE1_DATA")


#define REAL_TIME_CLOCK_DISABLE
#define SYSTEM_TICK_TIMER_DISABLE

#include "core.h"

#define PIO_TST			HW::PIOB
#define PIN_TST			10
#define TST			(1UL<<PIN_TST)		

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma optimize_for_space

extern "C" void core1_ctorloop()
{
	typedef void (*CTR)();

	extern CTR __ctor_table;

	CTR *p = &__ctor_table+1;

	while (*p != 0)
	{
		if (((u32)*p & 0xFFF00000) == 0xFF600000) (*p)();

		p++;
	};
}

#pragma optimize_as_cmd_line

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ConstDataPointer dp1(0);

extern "C" void core1_main()
{
	//DataPointer dp2(0);

	HW::PIOB->DirSet(TST);

	while (1)
	{
		dp1.b += 1;
		//dp2.b += 1;
		
		PIO_TST->NOT(TST);
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
