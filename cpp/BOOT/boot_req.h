#ifndef BOOT_REQ_H__11_02_2024__23_05
#define BOOT_REQ_H__11_02_2024__23_05

#pragma once

#include "types.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef _ADI_COMPILER
#pragma pack(1)
#endif

__packed struct BootReqHS { u64 guid; u16 crc; } ;
__packed struct BootRspHS { u64 guid; u16 crc; };

#ifdef _ADI_COMPILER
#pragma pack()
#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct BootReqMes_old
{
	u32 len;
	
	union
	{
		struct { u32 func; u32 len;								u16 align; u16 crc;	}	F1; // Get CRC
		struct { u32 func;										u16 align; u16 crc;	}	F2; // Exit boot loader
		struct { u32 func; u32 padr; u32 plen; u32 pdata[1];						}	F3; // Programm page
	};

};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct BootRspMes_old
{
	u32 len;

	union
	{
		struct { u32 func; u32 pageLen;	u32 len;	u16 sCRC;	u16 crc; }	F1; // Get CRC
		struct { u32 func;							u16 align;	u16 crc; } 	F2; // Exit boot loader
		struct { u32 func; u32 padr;	u32 status; u16 align;	u16 crc; } 	F3; // Programm page
	};
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

union BootReqV1
{
	enum { VERSION = 1, FUNC_MAX = 4 };

	struct SF0 { u16 adr; u16 rw;										u16 align; u16 crc;			} F0;	// запрос версии протокола и индентификатора железа
	struct SF1 { u16 adr; u16 rw; u32 len;								u16 align; u16 crc;			} F1;	// запрос контрольной суммы и длины программы во флэш-памяти
	struct SF2 { u16 adr; u16 rw; u32 padr; u32 plen; u32 pdata[1]; /* последние 2 байта CRC */		} F2;	// запись страницы во флэш
	struct SF3 { u16 adr; u16 rw;										u16 align;	u16 crc;		} F3;	// старт основной программы
	struct SF4 { u16 adr; u16 rw; u32 timeOutMS;						u16 align;	u16 crc;		} F4;	// установить таймаут выхода из загрузчика
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

union BootRspV1
{
	enum { VERSION = 1, FUNC_MAX = 4 };

	struct SF0 { u16 adr; u16 rw; u16 ver; u16 maxFunc; u64 guid; u32 startAdr; u16 pageLen;	u16 crc; } F0;	// запрос версии протокола и индентификатора железа
	struct SF1 { u16 adr; u16 rw; u32 flashLen;  u16 flashCRC;									u16 crc; } F1;	// запрос контрольной суммы и длины программы во флэш-памяти
	struct SF2 { u16 adr; u16 rw; u16 res;														u16 crc; } F2;	// запись страницы во флэш
	struct SF3 { u16 adr; u16 rw;													u16 align;	u16 crc; } F3;	// старт основной программы
	struct SF4 { u16 adr; u16 rw;													u16 align;	u16 crc; } F4;	// установить таймаут выхода из загрузчика
};

#endif // BOOT_REQ_H__11_02_2024__23_05


