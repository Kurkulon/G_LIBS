#ifndef TIME_H__11_02_2024__23_08
#define TIME_H__11_02_2024__23_08

#include "types.h"

#ifdef WIN32
#include <windows.h>
#else
#include "core.h"
#endif

#if defined(CPU_SAME53) || defined(CPU_XMC48) || defined(WIN32)

#define RTC_type RTC

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

__packed struct RTC
{
	__packed union
	{
		__packed struct
		{
			u32 msec:10;     // mili second value - [0,999] 
			u32 sec:6;     // Second value - [0,59] 
			u32 min:6;     // Minute value - [0,59] 
			u32 hour:5;    // Hour value - [0,23] 
		};

		u32 time;
	};
	__packed union
	{
		__packed struct
		{
			u32 day:5;    // Day of the month value - [1,31] 
			u32 mon:4;     // Month value - [1,12] 
			u32 year:12;    // Year value - [0,4095] 
		};

		u32 date;
	};

	inline void operator=(const RTC &r) { time = r.time; date = r.date; }
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

extern bool SetTime(const RTC &t);
extern void GetTime(RTC *t);

extern RTC timeBDC;

#endif // #if defined(CPU_SAME53) || defined(CPU_XMC48)

extern void Init_time(u32 mck);
extern void RTT_Init();


#ifdef WIN32	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

extern LARGE_INTEGER queryPerformanceFrequency;

#define US2CTM(x) (((u32)(x) * queryPerformanceFrequency.LowPart + 500000UL) / 1000000UL)
#define MS2CTM(x) (((u32)(x) * queryPerformanceFrequency.LowPart + 500UL) / 1000UL)

inline u32 GetCYCCNT() 
{ 
	LARGE_INTEGER t;

	QueryPerformanceCounter(&t);

	return t.LowPart; 
}

#elif defined(CORTEX_M4)

inline u32 GetCYCCNT() { return CM4::DWT->CYCCNT; }

#define NS2CTM(x) NS2CLK(x)
#define US2CTM(x) US2CLK(x)
#define MS2CTM(x) MS2CLK(x)

#elif defined(CORETYPE_LPC8XX)

inline u32 GetCYCCNT() { return ~HW::WKT->COUNT; }

#define NS2CTM(x) (((x)*75+50000)/100000)
#define US2CTM(x) (((x)*75+50)/100)
#define MS2CTM(x) ((x)*750)

#elif defined(ADSP_BLACKFIN)

#define GetCYCCNT GetCycles32

#define NS2CTM(x) NS2CCLK(x)
#define US2CTM(x) US2CCLK(x)
#define MS2CTM(x) MS2CCLK(x)

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline u32 GetMilliseconds()
{
#ifndef WIN32
	extern volatile u32 msec;
	return msec;
#else
	return GetTickCount();
#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline word GetMillisecondsLow()
{
#ifndef WIN32
	extern volatile u32 msec;
	return (u16)msec;
#else
	return (u16)(GetTickCount());
#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef ADSP_BLACKFIN

struct TM32
{
	u32 pt;

	//TM32() : pt(0) {}
	bool Check(u32 v) { u32 t = GetMilliseconds(); if ((u32)(t - pt) >= v) { pt = t; return true; } else { return false; }; }
	bool Timeout(u32 v) { return (u32)(GetMilliseconds() - pt) >= v; }
	void Reset() { pt = GetMilliseconds(); }
};

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct CTM32
{
	u32 pt;

	//TM32() : pt(0) {}
	bool Check(u32 v) { u32 t = GetCYCCNT(); if ((u32)(t - pt) >= v) { pt = t; return true; } else { return false; }; }
	bool Timeout(u32 v) { return (u32)(GetCYCCNT() - pt) >= v; }
	void Reset() { pt = GetCYCCNT(); }
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef ADSP_BLACKFIN

struct TM32
{
	u64 pt;

	bool Check(u32 v) { u64 t = GetCycles64(); if ((u32)(t - pt) >= MS2CTM(v)) { pt = t; return true; } else { return false; }; }
	bool Timeout(u32 v) { return (u64)(GetCycles64() - pt) >= MS2CTM(v); }
	void Reset() { pt = GetMilliseconds(); }
};

struct CTM64
{
	u64 pt;

	//TM32() : pt(0) {}
	bool Check(u64 v) { u64 t = GetCycles64(); if ((u64)(t - pt) >= v) { pt = t; return true; } else { return false; }; }
	bool Timeout(u64 v) { return (u64)(GetCycles64() - pt) >= v; }
	void Reset() { pt = GetCycles64(); }
};

#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct Dbt
{
//	bool	stat;
	u32		pt;
	u32		dt;

	Dbt(u32 t = 500) : pt(0), dt(t) {}

	bool Check(bool c)
	{
		if (!c)
		{ 
			pt = GetMilliseconds(); 
		} 
		else if ((GetMilliseconds() - pt) < dt)
		{ 
			c = false; 
		}; 

		return c;
	}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct Deb
{
	bool	stat;
	u32		pt;
	u32		dt;

	Deb(bool s = false, u32 t = 500) : stat(s), pt(0), dt(t) {}

	bool Check(bool c)
	{
		if (stat == c)
		{ 
			pt = GetMilliseconds(); 
		} 
		else if ((GetMilliseconds() - pt) >= dt)
		{ 
			stat = c; 
		}; 

		return stat;
	}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // TIME_H__11_02_2024__23_08
