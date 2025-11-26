#if !defined(TIME_IMP_H__25_09_2025__19_20) //&& !defined(_ADI_COMPILER)
#define TIME_IMP_H__25_09_2025__19_20

#pragma once

#include "time.h"
#include "core.h"

#include "SEGGER_RTT\SEGGER_RTT.h"

#ifdef WIN32

#include <windows.h>
#include <time.h>

static LARGE_INTEGER queryPerformanceFrequency = { 0, 0 };

#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef SYSTEM_TICK_TIMER_ENABLE
volatile u32 msec = 0;
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef REAL_TIME_CLOCK_ENABLE

RTC timeBDC;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static const byte daysInMonth[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define SECONDS_FROM_1900_TO_1970       2208988800UL
#define SECONDS_FROM_1972_TO_2000       883612800UL
#define SECONDS_FROM_1970_TO_1972       63072000UL
#define SECONDS_FROM_1970_TO_2000		(SECONDS_FROM_1970_TO_1972+SECONDS_FROM_1972_TO_2000)      

#define SECONDS_PER_DAY                 (24 * 60 * 60)
#define DAYS_FROM_1900_TO_1970          ( ( long ) ( SECONDS_FROM_1900_TO_1970 / SECONDS_PER_DAY ) ) 

#define MONTH_YR        ( 12 )
#define DAY_YR          ( 365 )
#define HOUR_YR         ( DAY_YR * 24 )
#define MINUTE_YR       ( HOUR_YR * 60 )
#define SECOND_YR       ( MINUTE_YR * 60 )
#define __MONTHS        ( INT_MIN / MONTH_YR )
#define __DAYS          ( INT_MIN / DAY_YR )

#define SMALLEST_YEAR_VALUE ( __MONTHS + __DAYS + __MINUTES + __SECONDS )

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void GetTime(RTC *t)
{
	if (t == 0) return;

#ifndef WIN32

	__disable_irq();

	t->date = timeBDC.date;
	t->time = timeBDC.time;

	__enable_irq();

#else

	SYSTEMTIME lt;

	GetLocalTime(&lt);

	t->msec =  lt.wMilliseconds;
	t->sec	=  lt.wSecond;
	t->min 	=  lt.wMinute;
	t->hour =  lt.wHour;
	t->day 	=  lt.wDay;
	t->mon 	=  lt.wMonth;
	t->year =  lt.wYear;

#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CheckTime(const RTC &t)
{
	if (t.time.sec > 59 || t.time.min > 59 || t.time.hour > 23 || t.date.day < 1 || (t.date.mon-1) > 11 || (t.date.year-2000) > 99) { return false; };

	byte d = daysInMonth[t.date.mon] + ((t.date.mon == 2 && (t.date.year&3) == 0) ? 1 : 0);

	if (t.date.day > d) { return false; };

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool SetTime(const RTC &t)
{
#ifndef WIN32

	if (!CheckTime(t)) { return false; };

	__disable_irq();

	timeBDC = t;

	__enable_irq();

	return true;

#else

	SYSTEMTIME lt;

	lt.wMilliseconds	=	t.msec;
	lt.wSecond			=	t.sec;
	lt.wMinute			=	t.min;
	lt.wHour			=	t.hour;
	lt.wDay				=	t.day;
	lt.wMonth			=	t.mon;
	lt.wYear			=	t.year;

	return SetLocalTime(&lt);

	return true;

#endif
}

#endif // #ifdef REAL_TIME_CLOCK_ENABLE

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if defined(SYSTEM_TICK_TIMER_ENABLE) && !defined(WIN32)

#ifdef __ADSPLPBLACKFIN__
#pragma diag(suppress:1970)
EX_INTERRUPT_HANDLER(Timer_Handler)
#elif defined(__CC_ARM)
static __irq void Timer_Handler (void)
#endif
{
	msec++;

	#ifdef REAL_TIME_CLOCK_ENABLE	//defined(CPU_SAME53) || defined(CPU_XMC48)

		if (timeBDC.time.msec < 999)
		{
			timeBDC.time.msec += 1;
		}
		else
		{
			timeBDC.time.msec = 0;

			if (timeBDC.time.sec < 59)
			{
				timeBDC.time.sec += 1;
			}
			else
			{
				timeBDC.time.sec = 0;

				if (timeBDC.time.min < 59)
				{
					timeBDC.time.min += 1;
				}
				else
				{
					timeBDC.time.min = 0;

					if (timeBDC.time.hour < 23)
					{
						timeBDC.time.hour += 1;
					}
					else
					{
						timeBDC.time.hour = 0;

						byte day = daysInMonth[timeBDC.date.mon] + ((timeBDC.date.mon == 2 && (timeBDC.date.year&3) == 0) ? 1 : 0);

	//					if ((timeBDC.dayofweek += 1) > 6) timeBDC.dayofweek = 0;

						if (timeBDC.date.day < day)
						{
							timeBDC.date.day += 1;
						}
						else
						{
							timeBDC.date.day = 1;

							if (timeBDC.date.mon < 12)
							{
								timeBDC.date.mon += 1;
							}
							else
							{
								timeBDC.date.mon = 1;

								timeBDC.date.year += 1;
							};
						};
					};
				};
			};
		};

	#endif // #ifdef REAL_TIME_CLOCK_ENABLE
}

#endif // #if defined(SYSTEM_TICK_TIMER_ENABLE) && !defined(WIN32)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if (defined(SYSTEM_TICK_TIMER_ENABLE) || defined(REAL_TIME_CLOCK_ENABLE)) && !defined(WIN32)

static void InitTimer(/*u32 cpuclk*/)
{
	SEGGER_RTT_printf(0, RTT_CTRL_TEXT_BRIGHT_CYAN "System Timer Init CPUCLK = %u MHz ... ", MCK_MHz);

	enum { freq = 1000 };

	#ifdef REAL_TIME_CLOCK_ENABLE
		timeBDC.date.day = 1;
		timeBDC.date.mon = 1;
		timeBDC.date.year = 2000;
		timeBDC.time = 0;
	#endif

	#ifdef __CC_ARM
		VectorTableInt[15] = Timer_Handler;
	#elif defined(__ADSPLPBLACKFIN__) 
		InitIVG(IVG_CORETIMER, Timer_Handler);
	#endif

	#ifdef CORTEX_M4

		CM4::SysTick->LOAD = (MCK+freq/2)/freq;
		CM4::SysTick->VAL = 0;
		CM4::SysTick->CTRL = 7;

	#elif defined(CORTEX_M0)

		CM0::SysTick->LOAD = (MCK+freq/2)/freq;
		CM0::SysTick->VAL = 0;
		CM0::SysTick->CTRL = 7;

	#elif defined(__ADSPLPBLACKFIN__)

		HW::TMR->CNTL	= TMR_PWR;
		HW::TMR->PERIOD = (CCLK+freq/2)/freq;
		HW::TMR->SCALE	= 0;
		HW::TMR->CNTL	= TMR_AUTORLD|TMR_EN|TMR_PWR;

	#endif

	#ifdef __CC_ARM
		__enable_irq();
	#endif

	SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_GREEN "OK\n");
}

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef CORETYPE_LPC8XX

static __irq void WKT_Handler()
{
	HW::WKT->CTRL = WKT_CLKSEL_IRC_750kHz|WKT_ALARMFLAG;
	HW::WKT->COUNT = ~0;
}

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void InitCycleCountTimer()
{
#ifdef CORTEX_M4

	CM4::CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

	SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_WHITE "Cycle Count Timer Init ... ");

	__dsb(15);

	if ((CM4::DWT->CTRL & DWT_CTRL_NOCYCCNT_Msk) == 0)
	{
		CM4::DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

		SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_GREEN "OK\n");
	}
	else
	{
		SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_RED "!!! ERROR !!! Cycle counter not supported\n");
	};

#elif defined(CORETYPE_LPC8XX)

	using namespace HW;

	SYSCON->SYSAHBCLKCTRL |= CLK::WKT_M;
	SYSCON->PDRUNCFG &= ~(PDRUNCFG_IRC_PD|PDRUNCFG_IRCOUT_PD);

	VectorTableExt[WKT_IRQ] = WKT_Handler;
	CM0::NVIC->CLR_PR(WKT_IRQ);
	CM0::NVIC->SET_ER(WKT_IRQ);

	HW::WKT->CTRL = WKT_CLKSEL_IRC_750kHz;
	HW::WKT->COUNT = 750000;

#elif defined(__ADSPLPBLACKFIN__)

	sysreg_write(reg_SYSCFG, sysreg_read(reg_SYSCFG)|BITM_SYSCFG_CCEN);

#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Init_time(/*u32 cpuclk*/)
{
#if (defined(SYSTEM_TICK_TIMER_ENABLE) || defined(REAL_TIME_CLOCK_ENABLE)) && !defined(WIN32)
	InitTimer(/*cpuclk*/);
#endif
	InitCycleCountTimer();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // #if !defined(TIME_IMP_H__25_09_2025__19_20) && !defined(_ADI_COMPILER)

