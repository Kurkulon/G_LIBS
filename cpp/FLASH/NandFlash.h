#ifndef NANDFLASH_H__24_02_2024__15_36
#define NANDFLASH_H__24_02_2024__15_36

#pragma once

#include "EMAC\trap_def.h"
#include "list.h"
#include "CRC\CRC16.h"


//#define FLWB_LEN (3072)
//#define FLRB_LEN 1536

enum  
{
	NANDFL_STAT_NONE = 0,
	NANDFL_STAT_BUSY,				// занят неизвестной для компьютера операцией (например записью в память)
	NANDFL_STAT_STOP,
	NANDFL_STAT_PAUSE,
	NANDFL_STAT_RESUME,
	NANDFL_STAT_ERASE,
	NANDFL_STAT_UNERASE,
	NANDFL_STAT_READ_SESSION_START,
	NANDFL_STAT_READ_SESSION_IDLE,
	NANDFL_STAT_READ_SESSION_FIND, // поиск вектора по несвязанной области
	NANDFL_STAT_READ_SESSION_ERROR,
	NANDFL_STAT_READ_SESSION_READY,
	NANDFL_STAT_READ_VECTOR_START,
	NANDFL_STAT_READ_VECTOR_IDLE,
	NANDFL_STAT_READ_VECTOR_FIND, // поиск вектора по несвязанной области
	NANDFL_STAT_READ_VECTOR_ERROR,
	NANDFL_STAT_READ_VECTOR_READY
};


//enum flash_save_repeat_type
//{
//	FLASH_SAVE_REPEAT_NONE = 0,
//	FLASH_SAVE_REPEAT_NORMAL = 2,	// одна перезапись, вторая - в новый блок
//	FLASH_SAVE_REPEAT_HIGH = 4,	// .... 3 в новый блок
//};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//struct UNIBUF;// : public PtrItem<UNIBUF>;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

__packed struct NandVecData
{
	__packed struct Hdr
	{
		u16 		session;
		u16 		device;
		RTC_type	rtc;
		byte		flags;		// флажки
		u64			prVecAdr;	// адрес предыдущего блядовектора
		u16			dataLen;
		u16			crc;		// CRC16 всей этой хрени
	} h;

	byte		data[/*FLWB_LEN*/]; // Последние 2 байта CRC16
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct NANDFLRB
{
	NANDFLRB *next;

	bool	ready;
	bool	vecStart; // Найти начало вектора и считать hdr

	bool	useAdr;
	u64		adr;

	NandVecData::Hdr hdr;

	u16		maxLen;
	u16		len;
	u16		crc;

	byte	*data;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

__packed struct NandFileDsc
{
	u16			session;	// file number
	i64			size;		//если 0 то сессия немного порченная
	RTC_type	start_rtc;	//если 0 то сессия немного порченная
	RTC_type	stop_rtc;  
	u32			startPage; 
	u32			lastPage;
	byte		flags;		// 1 - Сессия удалена

	void		SetErased()	{ flags |= 1; }
	void		ClrErased()	{ flags &= ~1; }
	bool		Erased()	{ return flags & 1; }
};	

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

extern MB*			NandFlash_AllocWB(u32 minLen);
extern NANDFLRB*	NandFlash_AllocRB();
extern void			NandFlash_FreeRB(NANDFLRB* b);
extern bool			NandFlash_RequestRead(NANDFLRB* b);
extern bool			NandFlash_RequestWrite(Ptr<MB> &mb, u16 devID, bool updateCRC);
inline u16			NandFlash_GetDeviceID() { extern u16 deviceID; return deviceID; }
extern void			NandFlash_StartSendSession();
extern NandFileDsc* NandFlash_GetSessionInfo(u16 session, u64 adr);

extern void NAND_Idle();
extern void NandFlash_FullErase();
extern void NandFlash_UnErase();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

extern void NandFlash_Init();
extern bool NandFlash_Reset();
extern void NandFlash_Update();
extern bool NandFlash_Busy();

extern bool NandFlash_WriteEnable();
extern void NandFlash_WriteDisable();
extern byte NandFlash_Status();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

extern u32 NandFlash_Vectors_Errors_Get();
extern u32 NandFlash_Vectors_Saved_Get();
extern u32 NandFlash_Vectors_Recieved_Get();
extern u32 NandFlash_Vectors_Rejected_Get();
extern u32 NandFlash_Session_Get();

extern u64 NandFlash_Current_Adress_Get();
extern u64 NandFlash_Full_Size_Get();
extern u16 NandFlash_Chip_Mask_Get();
extern u64 NandFlash_Used_Size_Get();
extern u16 NandFlash_BlockErr_Get();
extern u16 NandFlash_PageErr_Get();

extern bool NandFlash_SendStatus(u32 progress, byte status);

extern u32 NandFlash_ErrECC_Get();
extern u32 NandFlash_CorrectedErrECC_Get();
extern u32 NandFlash_ParityErrECC_Get();


//extern i64 FLASH_Empty_Size_Get();


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // NANDFLASH_H__24_02_2024__15_36
