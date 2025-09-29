#if !defined(NANDFLASH_IMP_V2_H__18_09_2025__18_28) && defined(NANDFLASH_IMP_VERSION)
#define NANDFLASH_IMP_V2_H__18_09_2025__18_28

#pragma once

#include "NandFlash.h"
#include "EMAC\vector.h"
#include "list.h"
#include "EMAC\trap.h"
#include "PointerCRC.h"
#include "SEGGER_RTT\SEGGER_RTT.h"
#include "CRC\CRC_CCITT_DMA.h"

#include "FLASH\NandFlash_def.h"
#include <MEM\mem.h>

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if defined(NAND_READ_CRC_SOFT) || defined(NAND_WRITE_CRC_SOFT)
#include "CRC\CRC16_CCIT.h"
#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if !defined(NAND_READ_CRC_SOFT) && !defined(NAND_READ_CRC_HW) && !defined(NAND_READ_CRC_PIO)
#define NAND_READ_CRC_PIO
#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if !defined(NAND_WRITE_CRC_SOFT) && !defined(NAND_WRITE_CRC_HW) && !defined(NAND_WRITE_CRC_PIO)
#define NAND_WRITE_CRC_HW
#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef NAND_ECC_CHECK

#include "FLASH\Nand_ECC.h"

static const bool check_ecc = true;					
#else
static const bool check_ecc = false;					
#endif

#ifdef NAND_ECC_SPARE
static const bool check_ecc_spare = true;					
#else
static const bool check_ecc_spare = false;					
#endif

#ifdef NAND_ECC_PAGE
static bool check_ecc_page = true;					
#else
static const bool check_ecc_page = false;					
#endif

#ifdef NAND_ECC_PAGEBUF
static bool check_ecc_pagebuf = true;					
#else
static const bool check_ecc_pagebuf = false;					
#endif

#if defined(NAND_ECC_PAGE) || defined(NAND_ECC_PAGEBUF)
#define NAND_ECC_PAGE_PAGEBUF
#undef NAND_VERIFY_WRITEPAGE
#endif

#ifdef NAND_VERIFY_WRITEPAGE
static const bool verifyWritePage = true;			// Проверка записаной страницы, путём чтения страницы и сравнения с буфером
#else
static const bool verifyWritePage = false;		// Проверка записаной страницы, путём чтения страницы и сравнения с буфером
#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//#include "extern_def.h"

#include "i2c.h"
#include "spi.h"

//#pragma diag_suppress 550,177

//#pragma O3
//#pragma Otime

//extern u16 CRC_CCITT_DMA(const void *data, u32 len, u16 init);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//static u32 FRAM_SPI_SESSIONS_ADR = 0;
//static u32 FRAM_I2C_SESSIONS_ADR = 0;

//static bool FRAM_SPI_PRESENT = false;
//static bool FRAM_I2C_PRESENT = false;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef WIN32

#pragma push
#pragma O3
#pragma Otime

#else


#endif

static bool __memcmp(ConstDataPointer s, ConstDataPointer d, u32 len)
{
	while (len > 3)
	{
		if (*s.d++ != *d.d++) return false;
		len -= 4;
	};

	while (len > 0)
	{
		if (*s.b++ != *d.b++) return false;
		len -= 1;
	};

	return true;
}

#ifndef WIN32

#pragma pop

#else

#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//#define FLASH_SAVE_BUFFER_SIZE		8400
//#define FLASH_READ_BUFFER_SIZE		8400

//static PtrFLWB flashPtrFLWB_Buffer[16];


List<ListItem> ListItem::freeItemList;
static ListItem listItems[LIST_ITEMS_NUM];

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//void UNIBUF::_FreeCallBack() {  }

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//List<UNIBUF> UNIBUF::freeBufList;
//static UNIBUF flashWriteBuffer[FLASH_WRITE_BUFFER_NUM];
//static NANDFLRB flashReadBuffer[FLASH_READ_BUFFER_NUM];

//List<PtrFLWB> PtrFLWB::freePtrList;

//static List<FLWB> freeFlWrBuf;
static ListRef<MB> writeFlBuf;

static List<NANDFLRB> freeFlRdBuf;
static List<NANDFLRB> readFlBuf;

struct PageBuffer { /*PageBuffer *next;*/ u32 page; u32 prevPage; byte data[NAND_PAGE_SIZE]; SpareArea spare; };

//static PageBuffer _pageBuf[3];

//static List<PageBuffer> freePageBuffer;

static ListPtr<MB> readyPageBuffer;

#ifdef NAND_ECC_PAGEBUF
static ListPtr<MB> eccPageBuffer;
#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//static SpareArea spareRead;
//static SpareArea spareWrite;
//static SpareArea spareErase;

static u32 pagesRead = 0;
static u32 pagesReadOK = 0;
static u32 pagesReadErr = 0;

//static u64 adrLastVector = ~0;
static u32 lastSessionBlock = ~0;
static u32 lastSessionPage = ~0;

static bool cmdCreateNextFile = false;
static bool cmdFullErase = false;
static bool cmdSendSession = false;

static bool writeFlashEnabled = false;
static bool flashFull = false;
static bool flashEmpty = false;

static bool testWriteFlash = false;

static SessionInfo lastSessionInfo;

u16 deviceID = 0;

static u64 flashUsedSize = 0;
static u64 flashStartAdr = 0;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

__packed struct NVV // NonVolatileVars  
{
	u32 timeStamp;

	NandFileDsc f;

	u16 index;

	u32 prevFilePage;

	u32 badBlocks[8];
	u32 pageError[8];

	void Init();
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void NVV::Init()
{
	timeStamp = 0;
	index = 0;
	prevFilePage = ~0;

	f.session = 0;
	f.size = 0;
	f.startPage = 0;
	f.lastPage = 0;
	GetTime(&f.start_rtc);
	GetTime(&f.stop_rtc);
	f.flags = 0;

	for (u32 i = 0; i < ArraySize(badBlocks); i++)
	{
		badBlocks[i] = 0;
		pageError[i] = 0;
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

__packed struct NVSI // NonVolatileSessionInfo  
{
	u32 timeStamp;

	NandFileDsc f;

	u16 crc;

	void UpdateCRC() { crc = GetCRC16(this, sizeof(timeStamp) + sizeof(f)); }
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static NVV nvv;

static NVSI nvsi[128];

static byte buf[sizeof(nvv)*2+4];

byte savesCount = 0;

byte savesSessionsCount = 0;

byte eraseSessionsCount = 0;

//static TWI	twi;

static void NandFlash_SaveVars();

static bool loadVarsOk = false;
static bool loadSessionsOk = false;

bool fram_I2C_flashVarsOk = false;
bool fram_SPI_flashVarsOk = false;
bool fram_I2C_loadSessionsOk = false;
bool fram_SPI_loadSessionsOk = false;

u32 verifyFlashErrors = 0;

inline void SaveParams() { savesCount = 1; }

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


const SessionInfo* GetLastSessionInfo()
{
	return &lastSessionInfo;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

enum flash_status_type
{
	NANDFL_STAT_WAIT = 0,
	NANDFL_STAT_WRITE,
	NANDFL_STAT_READ,
//	NANDFL_STAT_ERASE
};

static byte flashStatus = NANDFL_STAT_WAIT;

enum NandState
{
	NAND_STATE_WAIT = 0,
	NAND_STATE_WRITE_START,
	NAND_STATE_READ_START,
	NAND_STATE_FULL_ERASE_START,
	NAND_STATE_FULL_ERASE_0,
	NAND_STATE_CREATE_FILE,
	NAND_STATE_SEND_SESSION
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static NandState nandState = NAND_STATE_WAIT;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static byte lastFlashStatus = NANDFL_STAT_NONE;
static u32 lastFlashProgress = ~0;
static u32 lastFlashTime = 0;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//static const NandMemSize *nandSZ;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static u32 invalidBlocks = 0;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//static ComPort com1;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

byte	FLADR::chipValidNext[NAND_MAX_CHIP]; // Если чип битый, то по индексу находится следующий хороший чип
byte	FLADR::chipValidPrev[NAND_MAX_CHIP]; // Если чип битый, то по индексу находится предыдущий хороший чип
u32		FLADR::chipOffsetNext[NAND_MAX_CHIP]; // Если чип битый, то по индексу находится смещение адреса на следующий хороший чип
u32		FLADR::chipOffsetPrev[NAND_MAX_CHIP]; // Если чип битый, то по индексу находится смещение адреса на предыдущий хороший чип
byte 	FLADR::COL_BITS;
byte 	FLADR::PAGE_BITS;		
byte 	FLADR::BLOCK_BITS;		
//byte 	FLADR::COL_OFF;		// = 0
byte 	FLADR::PAGE_OFF;	// = COL_BITS;
byte 	FLADR::CHIP_OFF;	// = PAGE_OFF + PAGE_BITS
byte 	FLADR::BLOCK_OFF;	// = CHIP_OFF + NAND_CHIP_BITS 		
u32 	FLADR::COL_MASK;
u32 	FLADR::PAGE_MASK;		
u32 	FLADR::CHIP_MASK;		
u32 	FLADR::BLOCK_MASK;		
u32 	FLADR::RAWPAGE_MASK;	
u32 	FLADR::RAWBLOCK_MASK;	
u64 	FLADR::RAWADR_MASK;
u32 	FLADR::pg; //enum { pg = (1<<NAND_COL_BITS) };

#ifdef NAND_ECC_CHECK
u16		FLADR::spareSize;
#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void	FLADR::InitVaildTables(u16 mask)
{
	u32 blocksize = 1UL << CHIP_OFF;

	for(byte chip = 0; chip < NAND_MAX_CHIP; chip++)
	{
		chipValidNext[chip] = 0;
		chipValidPrev[chip] = 0;

		u32 offset = 0;

		for (byte i = 0; i < NAND_MAX_CHIP; i++)
		{
			byte cn = chip+i; if (cn >= NAND_MAX_CHIP) cn = 0;

			if (mask & (1<<cn))
			{
				chipValidNext[chip] = cn;
				chipOffsetNext[chip] = offset;
				break;
			};

			offset += blocksize;
		};

		offset = 0;

		for (byte i = 0; i < NAND_MAX_CHIP; i++)
		{
			byte cp = chip-i; if (cp >= NAND_MAX_CHIP) cp = NAND_MAX_CHIP - 1;

			if (mask & (1<<cp))
			{
				chipValidPrev[chip] = cp;
				chipOffsetPrev[chip] = offset;
				break;
			};
			
			offset += blocksize;
		};
	};
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool NandFlash_SendStatus(u32 progress, byte status)
{
	lastFlashProgress = progress;
	lastFlashStatus = status;
	lastFlashTime = GetMilliseconds();
	
	return TRAP_MEMORY_SendStatus(lastFlashProgress, lastFlashStatus);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void FLASH_UpdateStatus()
{
	u32 t = GetMilliseconds();
	u32 dt = t - lastFlashTime;

	if (dt >= 500)
	{
		TRAP_MEMORY_SendStatus(lastFlashProgress, lastFlashStatus);

		lastFlashProgress = ~0;
		lastFlashStatus = NANDFL_STAT_NONE;
		lastFlashTime = t;
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void InitFlashBuffer()
{
	//for (byte i = 0; i < ArraySize(flashWriteBuffer); i++)
	//{
	//	freeFlWrBuf.Add(&flashWriteBuffer[i]);
	//};

	//for (byte i = 0; i < ArraySize(flashReadBuffer); i++)
	//{
	//	freeFlRdBuf.Add(&flashReadBuffer[i]);
	//};

	//for (byte i = 0; i < ArraySize(_pageBuf); i++)
	//{
	//	freePageBuffer.Add(&_pageBuf[i]);
	//};
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MB* NandFlash_AllocWB(u32 minLen)
{
	MB* mb = AllocMemBuffer(minLen+sizeof(NandVecData::Hdr)+2);

	if (mb != 0)
	{
		mb->dataOffset = sizeof(NandVecData::Hdr);
	};

	return mb;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//void FreeFlashWriteBuffer(FLWB* b)
//{
//	b->Free();
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

NANDFLRB* NandFlash_AllocRB()
{
	return freeFlRdBuf.Get();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void NandFlash_FreeRB(NANDFLRB* b)
{
	freeFlRdBuf.Add(b);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool NandFlash_RequestRead(NANDFLRB* b)
{
	if ((b != 0)/* && (b->data != 0) && (b->maxLen > 0)*/)
	{
		b->ready = false;
		b->len = 0;

		readFlBuf.Add(b);

		return true;
	}
	else
	{
		return false;
	};
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void NandFlash_FullErase()
{
	cmdFullErase = true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void NandFlash_UnErase()
{
	for (u16 n = 0; n < ArraySize(nvsi); n++)
	{
		NVSI &d = nvsi[n];

		if (d.f.size != 0) d.f.ClrErased();
	};
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void NandFlash_InitNVSI()
{
	for (u16 n = 0; n < ArraySize(nvsi); n++)
	{
		nvsi[n].f.size = 0;
	};
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void NAND_NextSession()
{
	cmdCreateNextFile = true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void ResetNandState()
{
	nandState = NAND_STATE_WAIT;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static NandState GetNandState()
{
	return nandState;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct EraseBlock
{
	enum {	WAIT = 0,ERASE_START,ERASE_0,ERASE_1,ERASE_2,ERASE_3,ERASE_4,ERASE_5 };

	SpareArea spare;
	
	FLADR er;

	byte state;
	bool force;		// стереть полюбасу
	bool check;		// Проверить результат стирания
	u16	 errBlocks;
	u16  errErase;

#ifdef NAND_ECC_SPARE
	u32		eccErrCount;
	u32		eccCorrErrCount;
	u32		eccParityErrCount;
#endif

	EraseBlock() : er(~0), state(WAIT), force(false), check(true), errBlocks(0), errErase(0) {}

	void Start(const FLADR &rd, bool frc, bool chk);
	bool Update();
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void EraseBlock::Start(const FLADR &rd, bool frc, bool chk)
{
	er = rd;

	force = frc;
	check = chk;

	errBlocks = 0;

#ifdef NAND_ECC_SPARE
	eccErrCount = 0;
	eccCorrErrCount = 0;
	eccParityErrCount = 0;
#endif

	state = ERASE_START;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool EraseBlock::Update()
{
	switch(state)
	{
		case WAIT:
			
			return false;

		case ERASE_START:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++		
																																
			NAND_Chip_Select(er.GetChip());

			if (force)
			{
				NAND_CmdEraseBlock(er.GetBlock());																						
																															
				state = ERASE_2;																				
			}
			else
			{
				NAND_CmdReadPage(er.pg, er.GetBlock(), 0);																					
																																	
				state = ERASE_0;																						
			};
																																
			break;																												
																																
		case ERASE_0:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++				
																																
			if(!NAND_BUSY())																									
			{																													
#ifdef NAND_ECC_SPARE
				NAND_ReadDataDMA(&spare, spare.ecc_page - (byte*)&spare);	
#else
				NAND_ReadDataDMA(&spare, spare.CRC_SKIP);	
#endif
				state = ERASE_1;																					
			};																													
																																
			break;																												
																																
		case ERASE_1:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++				
																																
			if (NAND_CheckDataComplete())																							
			{	
#ifdef NAND_ECC_SPARE
				u32 err = 0;

				Nand_ECC_Corr((byte*)&spare, spare.ecc_spare - (byte*)&spare, NAND_ECC_LEN, spare.ecc_spare, &err, &eccCorrErrCount, &eccParityErrCount);

				if (err != 0)
				{
					__nop();
				};

				eccErrCount += err;
#endif
				if (spare.validPage != 0xFFFF && spare.validBlock != 0xFFFF)									
				{																												
					errBlocks += 1;	
					nvv.badBlocks[er.GetChip()] += 1;
																																
					er.NextBlock();	

					state = ERASE_START; 
				}	
				else if (spare.badPages != 0xFFFF)
				{
					errBlocks += 1;	
																																
					NAND_CmdWritePage(er.pg, er.GetBlock(), 0);																			
																																
					NAND_WRITE(0); NAND_WRITE(0); NAND_WRITE(0); NAND_WRITE(0); //*(u32*)FLD = 0;		// spareErase.validPage = 0; spareErase.validBlock = 0;																
																																
					NAND_CmdWritePage2();																							
																																
					state = ERASE_3;																				
				}
				else																											
				{																												
					NAND_CmdEraseBlock(er.GetBlock());																						
																																
					state = ERASE_2;																				
				};																												
			};																													
																																
			break;																												
																																
																																
		case ERASE_2:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++				
																																
			if(!NAND_BUSY())																									
			{					
				byte status = NAND_CmdReadStatus();

				if ((status & (NAND_SR_FAIL|NAND_SR_RDY)) != NAND_SR_RDY && check) // erase error																	
				{				
					errErase += 1;
					errBlocks += 1;	
					nvv.badBlocks[er.GetChip()] += 1;

					NAND_CmdWritePage(er.pg, er.GetBlock(), 0);																			
																																
					NAND_WRITE(0);NAND_WRITE(0);NAND_WRITE(0);NAND_WRITE(0);//*(u32*)FLD = 0;		// spareErase.validPage = 0; spareErase.validBlock = 0;																
																																
					NAND_CmdWritePage2();																							
																																
					state = ERASE_3;																				
				}																												
				else																											
				{																												
					//er_block = wr.block;														
					//er_chip = wr.chip;																							
																																
					state = WAIT;		

					return false;
				};																												
			};																													
																																
			break;																												
																																
		case ERASE_3:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++				
																																
			if(!NAND_BUSY())																									
			{																													
				er.NextBlock();	

				state = ERASE_START; 
			};

			break;
	};

	return true;
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct Write
{
	enum {	WAIT = 0,				CRC_START,				CRC_UPDATE,				VECTOR_UPDATE,			WRITE_START,		WRITE_BUFFER,			
			WRITE_PAGE,				WRITE_PAGE_0,			WRITE_PAGE_1,			WRITE_PAGE_2,			WRITE_PAGE_3,			
			WRITE_PAGE_4,			WRITE_PAGE_5,			WRITE_PAGE_6,			WRITE_PAGE_7,			WRITE_PAGE_ERR,		WRITE_PAGE_VRF,
			WRITE_PAGE_8,			ERASE,			
			WRITE_CREATE_FILE_0,	WRITE_CREATE_FILE_1,	WRITE_CREATE_FILE_2,	WRITE_CREATE_FILE_3,	WRITE_CREATE_FILE_4

#ifdef NAND_ECC_PAGE_PAGEBUF
			,WRITE_ECC_0	
#endif
		};

	FLADR wr;

	//u16		wr_cur_col;
	//u32 	wr_cur_pg;

	byte	wr_pg_error;
	u16		wr_count;
	NandVecData *vector;
	byte*	wr_data	;
	void*	wr_ptr	;

	u64		prWrAdr;
	u32		rcvVec ;
	u32		rejVec ;
	u32		errVec ;

	RTC_type lastRTC;

	Ptr<MB> curWrBuf;

	SpareArea spare;

	byte state;

	bool createFile;

	EraseBlock eraseBlock;

#ifdef NAND_ECC_CHECK
	u32		eccErrCount;
	u32		eccCorrErrCount;
	u32		eccParityErrCount;
#endif

	u16		ver_index;

	u16		tryVerifyCount;

	u32	wrBuf[NAND_PAGE_SIZE/4];

	struct RDBUF
	{
#if defined(NAND_VERIFY_WRITEPAGE) || defined(NAND_ECC_PAGE_PAGEBUF)

	#define NAND_WRITE_RDBUF

		u32	data[(NAND_PAGE_SIZE)/4];
#endif
		SpareArea spare; 
	};

	Ptr<MB> rdPtr;

	bool Start();
	bool Update();
	void Vector_Make(NandVecData *vd, u16 size);
	void Finish();
	void Init();
	void Init(u32 bl, u32 file, u32 prfile);

	void SaveSession();

//	void BufWriteData(void *data, u16 len) { NAND_CopyDataDMA(data, (byte*)wrBuf+wr.col, len); }

	void CreateNextFile() { state = WRITE_CREATE_FILE_0; }
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static Write write;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Write::Init()
{
	wr.SetRawPage(nvv.f.lastPage);

	spare.v1.file = nvv.f.session;  

	spare.v1.prev = nvv.prevFilePage;		

	spare.v1.start = nvv.f.startPage;		
	spare.v1.fpn = 0;	

	spare.v1.vectorCount = 0;

	spare.v1.vecFstOff = ~0;
	spare.v1.vecFstLen = 0;

	spare.v1.vecLstOff = ~0;
	spare.v1.vecLstLen = 0;

	spare.v1.fbb = 0;		
	spare.v1.fbp = 0;		

	spare.v1.chipMask = nandSize.mask;

	GetTime(&lastRTC);

#ifdef NAND_ECC_CHECK
	eccErrCount = 0;
	eccCorrErrCount = 0;
	eccParityErrCount = 0;
#endif

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Write::Init(u32 bl, u32 file, u32 prfile)
{
	wr.SetRawBlock(bl);

	spare.v1.file = file;  
//	spare.v1.lpn = wr.GetRawPage();

	spare.v1.prev = prfile;		

	spare.v1.start = wr.GetRawPage();		
	spare.v1.fpn = 0;	

	spare.v1.vectorCount = 0;

	spare.v1.vecFstOff = ~0;
	spare.v1.vecFstLen = 0;

	spare.v1.vecLstOff = ~0;
	spare.v1.vecLstLen = 0;

	spare.v1.fbb = 0;		
	spare.v1.fbp = 0;		

	spare.v1.chipMask = nandSize.mask;

	GetTime(&lastRTC);

#ifdef NAND_ECC_CHECK
	eccErrCount = 0;
	eccCorrErrCount = 0;
	eccParityErrCount = 0;
#endif

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Write::SaveSession()
{
	nvv.index = (nvv.index+1) & 127;

	NandFileDsc &s = nvv.f;
	NVSI &d = nvsi[nvv.index];

	d.f = nvv.f;

	d.crc = GetCRC16(&d.f, sizeof(d.f));

	s.session = spare.v1.file;
	s.startPage = s.lastPage = wr.GetRawPage();
	s.size = 0;
	GetTime(&s.start_rtc);
	s.stop_rtc = s.start_rtc;
	s.flags = 0;

	SaveParams();
	savesSessionsCount = 1;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Write::Vector_Make(NandVecData *vd, u16 size)
{
	vd->h.session = spare.v1.file;
//	vd->h.device = 0;
	GetTime(&vd->h.rtc);

	lastRTC = vd->h.rtc;

	vd->h.prVecAdr = prWrAdr; 
	vd->h.flags = 0;
	vd->h.dataLen = size;
	vd->h.crc = GetCRC16(&vd->h, sizeof(vd->h) - sizeof(vd->h.crc));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool Write::Start()
{
#ifdef WIN32

	static TM32 tm;

	if (tm.Check(100))
	{
		Printf(20, 0, 0xF0, "Session num  %-5hu",	nvv.f.session				);
		Printf(20, 1, 0xF0, "Session size %09llX",	nvv.f.size					);
		Printf(20, 2, 0xF0, "Raw adr      %09llX",	wr.GetRawAdr()				);
		Printf(20, 3, 0xF0, "rcvVec       %lu",		rcvVec						);
		Printf(20, 4, 0xF0, "Writed Vec   %lu",		write.spare.v1.vectorCount	);
		Printf(20, 5, 0xF0, "GetTickCount %hu",		GetMillisecondsLow()		);
	};

#endif

	if (!writeFlashEnabled || flashFull)
	{
		curWrBuf = writeFlBuf.Get();

		if (curWrBuf.Valid())
		{
			rcvVec += 1;
			rejVec += 1;

			curWrBuf.Free();
		};

		return false;
	}
	else 
	{
		if (lastRTC.time == timeBDC.time) return false;

		curWrBuf = writeFlBuf.Get();

		if (curWrBuf.Valid())
		{
			rcvVec += 1;

			//GetTime(&lastRTC);

			vector = (NandVecData*)((byte*)curWrBuf->GetDataPtr()-sizeof(NandVecData::Hdr));
			vector->h.dataLen = curWrBuf->len;

			state = (vector->h.flags) ? CRC_START : VECTOR_UPDATE;

			return true;
		}
		else
		{
			return false;
		};
	};
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Write::Finish()
{
	if (curWrBuf.Valid())
	{
		nvv.f.size += vector->h.dataLen + sizeof(vector->h);
		nvv.f.stop_rtc = vector->h.rtc;
		nvv.f.lastPage = spare.v1.rawPage;

		SaveParams();

		curWrBuf.Free();
	};
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool Write::Update()
{
//	u32 t;

	switch(state)
	{
		u16 crc;

		case WAIT:	return false;

		case CRC_START:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	
		{
#ifdef NAND_WRITE_CRC_HW

			if (CRC_CCITT_DMA_Async(vector->data, vector->h.dataLen, 0xFFFF))
			{
				state = CRC_UPDATE;
			};

			break;
		};

		case CRC_UPDATE:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++		
		{
			if (CRC_CCITT_DMA_CheckComplete(&crc))

#elif defined(NAND_WRITE_CRC_PIO)

			if (CRC_CCITT_PIO(vector->data, vector->h.dataLen, &crc))

#elif defined(NAND_WRITE_CRC_SOFT)

			crc = GetCRC16_CCIT_refl(vector->data, vector->h.dataLen);
#endif
			{
				DataPointer p(vector->data);

				p.b += vector->h.dataLen;

				*p.w = crc;

				vector->h.dataLen += 2;

				state = VECTOR_UPDATE;
			};

			break;
		};

		case VECTOR_UPDATE:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++		
		{
			Vector_Make(vector, vector->h.dataLen);

			wr_data = (byte*)vector;
			wr_count = vector->h.dataLen + sizeof(vector->h);

			prWrAdr = wr.GetRawAdr();

			if (spare.v1.vecFstOff == 0xFFFF)
			{
				spare.v1.vecFstOff = wr.GetCol();
				spare.v1.vecFstLen = wr_count;
			};

			spare.v1.vecLstOff = wr.GetCol();
			spare.v1.vecLstLen = wr_count;

			spare.v1.vectorCount += 1;

			state = WRITE_START;
		};

		case WRITE_START:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	
		{
			register u16 c = wr.pg - wr.GetCol();

			if (wr.GetCol() == 0 && wr_count >= wr.pg) // писать сразу во флеш
			{
				wr_ptr = wr_data;
				wr_count -= wr.pg;
				wr_data += wr.pg;

#ifdef NAND_ECC_PAGE_PAGEBUF
				state = WRITE_ECC_0; ver_index = 0;
#else
				state = WRITE_PAGE;
#endif
			}
			else // писать в буфер
			{
				if (wr_count < c ) c = wr_count;

				NAND_CopyDataDMA(wr_data, (byte*)wrBuf+wr.GetCol(), c);	// BufWriteData(wr_data, c);

				wr.SetCol(wr.GetCol() + c);
				wr_data += c;
				wr_count -= c;

				state = WRITE_BUFFER;
			};

			break;
		};

		case WRITE_BUFFER:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			if (NAND_CheckCopyComplete())
			{
				if (wr.GetCol() == 0)
				{
					wr_ptr = wrBuf;
//					wr.col = 0;

#ifdef NAND_ECC_PAGE_PAGEBUF
					state = WRITE_ECC_0; ver_index = 0;
#else
					state = WRITE_PAGE;
#endif
				}
				else if (wr_count > 0)
				{
					state = WRITE_START;
				}
				else
				{
					Finish();

					state = WAIT;

					return false;
				};
			};

			break;

#ifdef NAND_ECC_PAGE_PAGEBUF

		case WRITE_ECC_0:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	
		{
			byte *ecc_calc = spare.ecc_page + (ver_index/NAND_ECC_LEN)*3;

			Nand_ECC_Calc((byte*)wr_ptr+ver_index, 256, NAND_ECC_LEN, ecc_calc);

			ver_index += 256;

			if (ver_index >= wr.pg) state = WRITE_PAGE;

			break;
		};

#endif

		case WRITE_PAGE:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			if(eraseBlock.er.GetRawBlock() != wr.GetRawBlock())	// новый блок
			{
				wr_pg_error = 0;

				eraseBlock.Start(wr, forceEraseWrite, true);

	            state = ERASE;

				break;
			};

		case WRITE_PAGE_0:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			NAND_Chip_Select(wr.GetChip());

			NAND_CmdWritePage(0, wr.GetBlock(), wr.GetPage());

			NAND_WriteDataDMA(wr_ptr, wr.pg);
			
			state = WRITE_PAGE_1;

			break;

		case WRITE_PAGE_1:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			if (NAND_CheckDataComplete())
			{
				spare.validPage = ~0;
				spare.validBlock = ~0;
				spare.badPages = ~0;
				spare.v1.rawPage = wr.GetRawPage();
				spare.v1.chipMask = nandSize.mask;

				spare.v1.UpdateCRC();

#ifdef NAND_ECC_CHECK

				Nand_ECC_Calc((byte*)&spare, spare.ecc_spare - (byte*)&spare, NAND_ECC_LEN, spare.ecc_spare);

				NAND_WriteDataDMA(&spare, wr.spareSize);
#else
				NAND_WriteDataDMA(&spare, sizeof(spare));
#endif
				state = WRITE_PAGE_2;
			};

			break;

		case WRITE_PAGE_2:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			if (NAND_CheckDataComplete())
			{
				NAND_CmdWritePage2();

				state = WRITE_PAGE_3;
			};

			break;

		case WRITE_PAGE_3:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			if(!NAND_BUSY())
			{
				byte status = NAND_CmdReadStatus();

				if ((status & (NAND_SR_FAIL|NAND_SR_RDY)) != NAND_SR_RDY) // program error
				{
					spare.v1.fbp += 1;
					nvv.pageError[wr.GetChip()] += 1;

					NAND_CmdWritePage(wr.pg, wr.GetBlock(), wr.GetPage());

					NAND_WRITE(0);NAND_WRITE(0);//*(u16*)FLD = 0; // spareErase.validPage = 0;

					NAND_CmdWritePage2();

					state = WRITE_PAGE_4;
				}
//#ifdef NAND_ECC_CHECK
				else	//if (verifyWritePage || verifySpare || check_ecc_spare || check_ecc_page)
				{
					tryVerifyCount = 3;
					
					goto write_page_verify_start;

					//NAND_CmdReadPage((verifyWritePage || check_ecc_page) ? 0 : wr.pg, wr.GetBlock(), wr.GetPage());

					//state = WRITE_PAGE_6;
				};
//#else
//				else //if (verifyWritePage || verifySpare)
//				{
//					NAND_CmdReadPage((verifyWritePage) ? 0 : wr.pg, wr.GetBlock(), wr.GetPage());
//					
//					state = WRITE_PAGE_6;
//				}
//#endif
//				else
//				{
//					state = WRITE_PAGE_8;
//				};
			};

			break;

		case WRITE_PAGE_4:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++			

			if(!NAND_BUSY())																									
			{																													
				if (wr_pg_error == 2) // пометить блок как имеющий сбойные страницы
				{
					NAND_CmdWritePage(wr.pg + sizeof(spare.validPage) + sizeof(spare.validBlock), wr.GetBlock(), 0);

					NAND_WRITE(0);NAND_WRITE(0);//*(u16*)FLD = 0; // spare.badPages = 0;

					NAND_CmdWritePage2();

					state = WRITE_PAGE_5;
				}
				else
				{
					state = WRITE_PAGE;																			
				};

				wr_pg_error += 1;

				wr.NextPage();		
			};																													

			break;

		case WRITE_PAGE_5:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++			
																																
			if(!NAND_BUSY())																									
			{																													
				state = WRITE_PAGE;																			
			};
																																
			break;	

		write_page_verify_start:

			NAND_CmdReadPage((verifyWritePage || check_ecc_page) ? 0 : wr.pg, wr.GetBlock(), wr.GetPage());

			state = WRITE_PAGE_6;

			break;

		case WRITE_PAGE_6:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			if (!rdPtr.Valid())
			{
				rdPtr = AllocMemBuffer(sizeof(RDBUF));

				if (!rdPtr.Valid()) break;
			};

			if(!NAND_BUSY())
			{
				RDBUF &p = *((RDBUF*)rdPtr->GetDataPtr());

#ifdef NAND_ECC_CHECK
	
	#ifdef NAND_WRITE_RDBUF

				NAND_ReadDataDMA(p.data, wr.pg+wr.spareSize);
	#else
				NAND_ReadDataDMA(&p.spare, wr.spareSize);
	#endif

#else

	#ifdef NAND_WRITE_RDBUF

				NAND_ReadDataDMA(p.data, sizeof(p));
	#else
				NAND_ReadDataDMA(&p.spare, sizeof(p.spare));
	#endif

#endif
				state = WRITE_PAGE_7;
			};

			break;	

		case WRITE_PAGE_7:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++			

			if (NAND_CheckDataComplete())
			{
				RDBUF &p = *((RDBUF*)rdPtr->GetDataPtr());

				SpareArea &sp = p.spare; //*((SpareArea*)(rdBuf+wr.pg/4));

				if (sp.v1.rawPage != wr.GetRawPage())
				{
					if (tryVerifyCount > 0)
					{
						tryVerifyCount -= 1;

						goto write_page_verify_start;
					}
					else
					{
						DEBUG_ASSERT(0);
					};
				};

#ifdef NAND_ECC_SPARE

				u32 corrErr = 0, err = 0, parErr = 0;

				Nand_ECC_Corr((byte*)&sp, sp.ecc_spare - (byte*)&sp, NAND_ECC_LEN, sp.ecc_spare, &err, &corrErr, &parErr);

				eccErrCount			+= err;
				eccCorrErrCount		+= corrErr;
				eccParityErrCount	+= parErr;

				if (err != 0)
#else
				if (!__memcmp(&spare, &sp, sizeof(spare)))
#endif
				{
					state = WRITE_PAGE_ERR;
				}
				else if (verifyWritePage || check_ecc_page)
				{
					state = WRITE_PAGE_VRF;
					ver_index = 0;
				}
				else
				{
					state = WRITE_PAGE_8;
				};
			};

			break;

		case WRITE_PAGE_VRF:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++			
		{
#ifdef NAND_WRITE_RDBUF

			RDBUF &p = *((RDBUF*)rdPtr->GetDataPtr());

	#ifdef NAND_ECC_PAGE_PAGEBUF

			SpareArea &sp = p.spare; // *((SpareArea*)(rdBuf+wr.pg/4));

			byte *ecc_read = sp.ecc_page + (ver_index/NAND_ECC_LEN)*3;

			u32 corrErr = 0, err = 0, parErr = 0;

			Nand_ECC_Corr((byte*)p.data + ver_index, 256, NAND_ECC_LEN, ecc_read, &err, &corrErr, &parErr);

			eccErrCount			+= err;
			eccCorrErrCount		+= corrErr;
			eccParityErrCount	+= parErr;

			if (err != 0)
	#else
			if (!__memcmp((byte*)wr_ptr + ver_index, (byte*)p.data + ver_index, 256))
	#endif
			{
				state = WRITE_PAGE_ERR;
			}
			else
			{
				ver_index += 256;

				if (ver_index >= wr.pg) state = WRITE_PAGE_8;
			};
#else
			state = WRITE_PAGE_8;
#endif
			break;
		};

		case WRITE_PAGE_ERR:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++			
		{
			verifyFlashErrors += 1;

			spare.v1.fbp += 1;

			NAND_CmdWritePage(wr.pg, wr.GetBlock(), wr.GetPage());

			NAND_WRITE(0);NAND_WRITE(0);//*(u16*)FLD = 0; // spareErase.validPage = 0;

			NAND_CmdWritePage2();

			state = WRITE_PAGE_4;

			break;
		};

		case WRITE_PAGE_8:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (wr_count == 0)
			{
				Finish();

				state = (createFile) ? WRITE_CREATE_FILE_1 : WAIT;
			}
			else
			{
				state = WRITE_START;
			};

			wr.NextPage();

			spare.v1.fpn += 1;

			spare.v1.vecFstOff = ~0;
			spare.v1.vecFstLen = 0;

			spare.v1.vecLstOff = ~0;
			spare.v1.vecLstLen = 0;

			break;	

		case ERASE:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++			
					
			if (!eraseBlock.Update())
			{
				wr = eraseBlock.er;

				spare.v1.fbb += eraseBlock.errBlocks;

#ifdef NAND_ECC_CHECK
				eccErrCount			+= eraseBlock.eccErrCount;
				eccCorrErrCount		+= eraseBlock.eccCorrErrCount;
				eccParityErrCount	+= eraseBlock.eccParityErrCount;
#endif
				state = WRITE_PAGE_0;																			
			};
																																
			break;																												

		case WRITE_CREATE_FILE_0:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			if (wr.GetCol() > 0)
			{
				wr_ptr = wrBuf;
				wr_count = 0;

				byte *p = (byte*)wrBuf;

				p += wr.GetCol();

				for (u16 i = wr.GetCol()&3; i > 0; i--)
				{
					*(p++) = 0xff;
				};

				u32 *d = wrBuf + (wr.GetCol()+3)/4;

				for (u16 i = (sizeof(wrBuf)-wr.GetCol())/4; i > 0; i--)
				{
					*(d++) = ~0;
				};

				createFile = true;

#ifdef NAND_ECC_PAGE_PAGEBUF
				state = WRITE_ECC_0; ver_index = 0;
#else
				state = WRITE_PAGE;
#endif
				break;
			};

		case WRITE_CREATE_FILE_1:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			spare.v1.file += 1;  		

			spare.v1.prev = spare.v1.start;		

			if (wr.GetPage() > 0)
			{
				wr.NextBlock();

				//if (wr.overflow != 0)
				//{
				//	flashFull = true;
				//};
			};

			spare.v1.start = wr.GetRawPage();		
			spare.v1.fpn = 0;
			spare.v1.vectorCount = 0;

			//wr_prev_pg = -1;
			//wr_prev_col = 0;

			spare.v1.fbb = 0;		
			spare.v1.fbp = 0;		

			spare.v1.chipMask = nandSize.mask;	

			SaveSession();

			createFile = false;

			state = WAIT;

			return false;
	};

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct ReadSpare
{
	enum { WAIT = 0, START, READ_1, READ_2, READ_3 };

	SpareArea	*spare;
	FLADR		*rd;

#ifdef NAND_ECC_SPARE
	u32			eccErrCount;
	u32			eccCorrErrCount;
	u32			eccParityErrCount;
#endif

	u32			blockTryCount;
	u32			pageTryCount;
	u32			badBlocks[8];
	u16			badPages;
	u16			badSpare;

	byte		state;

	ReadSpare() : spare(0), rd(0) {}

	bool Start(SpareArea *sp, FLADR *frd);
	bool Update();
	void ClearBadBlocks() { for (u32 i = 0; i < ArraySize(badBlocks); i++) badBlocks[i] = 0; }
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool ReadSpare::Start(SpareArea *sp, FLADR *frd)
{
	if (sp == 0 || frd == 0)
	{
		return false;
	};

	spare = sp;
	rd = frd;

	blockTryCount = 32;
	pageTryCount = 128;

#ifdef NAND_ECC_SPARE
	eccErrCount = 0;
	eccCorrErrCount = 0;
	eccParityErrCount = 0;
#endif

	state = START;

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool ReadSpare::Update()
{
	switch(state)
	{
		case WAIT:

			return false;

		case START:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			NAND_Chip_Select(rd->GetChip());

			NAND_CmdReadPage(rd->pg, rd->GetBlock(), rd->GetPage());

			state = READ_1;

			break;

		case READ_1:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			if(!NAND_BUSY())
			{
#ifdef NAND_ECC_SPARE
				NAND_ReadDataDMA(spare, rd->spareSize);
#else
				NAND_ReadDataDMA(spare, sizeof(*spare));
#endif
				state = READ_2;
			};

			break;

		case READ_2:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			if (NAND_CheckDataComplete())
			{
#ifdef NAND_ECC_SPARE
				u32 err = 0;

				Nand_ECC_Corr((byte*)spare, spare->ecc_spare - (byte*)spare, NAND_ECC_LEN, spare->ecc_spare, &err, &eccCorrErrCount, &eccParityErrCount);

				if (err != 0)
				{
					__nop();
				};

				eccErrCount += err;
#endif
				if (spare->validBlock != 0xFFFF)
				{
					badBlocks[rd->GetChip()] += 1;

					if (blockTryCount > 0)
					{
						blockTryCount--;

						rd->NextBlock();

						NAND_Chip_Select(rd->GetChip());
						NAND_CmdReadPage(rd->pg, rd->GetBlock(), rd->GetPage());

						state = READ_1;
					}
					else
					{
						state = WAIT;
					};
				}				
				else if (spare->validPage != 0xFFFF)
				{
					badPages++;

					if (pageTryCount > 0)
					{
						pageTryCount--;

						rd->NextPage();

						NAND_Chip_Select(rd->GetChip());
						NAND_CmdReadPage(rd->pg, rd->GetBlock(), rd->GetPage());

						state = READ_1;
					}
					else
					{
						state = WAIT;
					}
				}
				else
				{
					spare->v1.CheckCRC();
#ifdef _DEBUG
					u32 rp = rd->GetRawPage();

					if (spare->v1.crc == 0 && spare->v1.rawPage != rp) __breakpoint(0);
#endif
					if (spare->v1.fpn != 0xFFFFFFFF && spare->v1.vectorCount != 0xFFFFFFFF && spare->v1.crc != 0) badSpare++;

					state = WAIT;

					return false;
				};
			};

			break;
	};

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct Read2
{
	enum {	WAIT = 0, READ_START, READ_PAGE, FIND_3, FLUSH_PAGES, CRC_START, CRC_UPDATE};

	NANDFLRB *curRdBuf;

	FLADR	rd;
	FLADR	padr;

	ReadSpare readSpare;

	byte*	rd_data;
	u16		rd_count;
	u16		findTryCount;

#ifdef NAND_ECC_PAGE_PAGEBUF
	u16		ecc_count;
#endif

	u32 	sparePage;
	u32 	prevSparePage;

	Ptr<MB> page; //PageBuffer *page;
	Ptr<MB> rdPB; //PageBuffer *rdPB;

#ifdef NAND_ECC_CHECK
	u32		eccErrCount;
	u32		eccCorrErrCount;
	u32		eccParityErrCount;
#endif

	u32 	initFlushSparePage;

#ifdef NAND_ECC_PAGEBUF
	Ptr<MB> eccPB;

	byte statePageECC;

	void UpdatePageECC();
#endif

	bool	vecStart;
	bool	cmdFlushPageBuffer;

	void FlushPageBuffer(u32 page) { cmdFlushPageBuffer = true; initFlushSparePage = page; }

	byte state;
	byte statePage;

	Read2() :  curRdBuf(0), rd_data(0), rd_count(0), sparePage(~0), prevSparePage(~0), page(0), rdPB(0),
#ifdef NAND_ECC_CHECK
				eccErrCount(0), eccCorrErrCount(0), eccParityErrCount(0),
#endif
#ifdef NAND_ECC_PAGEBUF
				eccPB(0), statePageECC(0),
#endif
		vecStart(false), state(WAIT), statePage(0)  {}
	
	bool Start();
	bool Update();
	bool UpdateFull() { bool c = Update(); return  c || statePage != 0; }
	void End() { curRdBuf->ready = true; curRdBuf = 0; state = WAIT; }
	void UpdatePage();
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static Read2 read;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool Read2::Start()
{
	if ((curRdBuf = readFlBuf.Get()) != 0)
	{
		if (curRdBuf->useAdr) 
		{ 
			rd.SetRawAdr(curRdBuf->adr); 
			FlushPageBuffer(rd.GetRawPage()); 

			state = FLUSH_PAGES;
		}
		else
		{
			state = READ_START;
		};

		vecStart = curRdBuf->vecStart;

		if (vecStart)
		{
			rd_data = (byte*)&curRdBuf->hdr;
			rd_count = sizeof(curRdBuf->hdr);
			curRdBuf->len = 0;	
			curRdBuf->crc = 0xFFFF;
		}
		else
		{
			rd_data = curRdBuf->data;
			rd_count = curRdBuf->maxLen;
			curRdBuf->len = 0;	
		};

		findTryCount = 1024;

		return true;
	};

	return false;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Read2::UpdatePage()
{
	switch(statePage)
	{
		case 0:	

			if (cmdFlushPageBuffer)
			{
				statePage = 6;

				break;
			};

			if (/*state < CRC_START &&*/ sparePage != ~0ul)
			{
				rdPB = AllocMemBuffer(sizeof(PageBuffer)); //freePageBuffer.Get();

				if (rdPB.Valid())
				{
					PageBuffer &p = *((PageBuffer*)rdPB->GetDataPtr());

					padr.SetRawPage(sparePage);

					readSpare.Start(&p.spare, &padr);	

					statePage++;
				};
			};
			
			break;

		case 1:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (!readSpare.Update())
			{
				PageBuffer &p = *((PageBuffer*)rdPB->GetDataPtr());

				p.prevPage	= prevSparePage;
				p.page		= sparePage = padr.GetRawPage();

#ifdef NAND_ECC_CHECK
				eccErrCount			+= readSpare.eccErrCount;
				eccCorrErrCount		+= readSpare.eccCorrErrCount;
				eccParityErrCount	+= readSpare.eccParityErrCount;
#endif
				if (!readPageCheckSpareCRC || p.spare.v1.crc == 0)
				{
					NAND_CmdRandomRead(0);
					statePage++;
				}
				else
				{
					statePage = 4;
				};
			};

			break;

		case 2:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			
			if(!NAND_BUSY())
			{
				PageBuffer &p = *((PageBuffer*)rdPB->GetDataPtr());

				NAND_ReadDataDMA(p.data, padr.pg);

				statePage++;
			};

			break;

		case 3:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			
			if(NAND_CheckDataComplete())
			{
#ifdef NAND_ECC_PAGE

				ecc_count = 0;
				statePage++;
#else
				statePage += 2;
#endif
			};

			break;

		case 4:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		{
#ifdef NAND_ECC_PAGE

			PageBuffer &p = *((PageBuffer*)rdPB->GetDataPtr());

			byte *ecc_read = p.spare.ecc_page + (ecc_count/NAND_ECC_LEN)*3;

			u32 err = 0;

			Nand_ECC_Corr(p.data+ecc_count, 256, NAND_ECC_LEN, ecc_read, &err, &eccCorrErrCount, &eccParityErrCount);

			if (err != 0)
			{
				__nop();
			};

			eccErrCount += err;

			ecc_count += 256;

			if (ecc_count >= padr.pg) statePage++;

			break;
#else
			statePage++;
#endif
		};

		case 5:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			
#ifdef NAND_ECC_PAGEBUF
			eccPageBuffer.Add(rdPB);
#else
			readyPageBuffer.Add(rdPB); 
#endif
			rdPB.Free();

			prevSparePage = sparePage;

			sparePage++;

			if (cmdFlushPageBuffer)
			{
				cmdFlushPageBuffer = false;
				
				statePage++;
			}
			else
			{
				statePage = 0;	
			};

			break;

		case 6:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			
			if (page.Valid())
			{
				page.Free(); //freePageBuffer.Add(page); page = 0;
			};

			if (rdPB.Valid())
			{
				rdPB.Free(); //freePageBuffer.Add(rdPB);
			};
				
			while ((rdPB = readyPageBuffer.Get()).Valid())
			{
				rdPB.Free(); //freePageBuffer.Add(rdPB);
			};

#ifdef NAND_ECC_PAGEBUF

			if (eccPB.Valid())
			{
				eccPB.Free(); //freePageBuffer.Add(eccPB);
			};
				
			while ((eccPB = eccPageBuffer.Get()).Valid())
			{
				eccPB.Free(); //freePageBuffer.Add(eccPB);
			};

			statePageECC = 0;
#endif 

			sparePage = initFlushSparePage;
			prevSparePage = ~0;

			cmdFlushPageBuffer = false;

			statePage = 0;	

			break;
	};
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef NAND_ECC_PAGEBUF

void Read2::UpdatePageECC()
{
	switch(statePageECC)
	{
		case 0:	

			eccPB = eccPageBuffer.Get();

			if (eccPB.Valid())
			{
				ecc_count = 0;

				statePageECC++;
			};

			break;

		case 1:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		{
			PageBuffer &p = *((PageBuffer*)eccPB->GetDataPtr());

			byte *ecc_read = p.spare.ecc_page + (ecc_count/NAND_ECC_LEN)*3;

			u32 err = 0;

			Nand_ECC_Corr(p.data+ecc_count, 256, NAND_ECC_LEN, ecc_read, &err, &eccCorrErrCount, &eccParityErrCount);

			if (err != 0)
			{
				__nop();
			};

			eccErrCount += err;

			ecc_count += 256;

			if (ecc_count >= rd.pg)
			{
				readyPageBuffer.Add(eccPB);
				eccPB.Free();
				statePageECC = 0;
			};

			break;
		};
	};
}

#endif // #ifdef NAND_ECC_PAGEBUF

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool Read2::Update()
{
	UpdatePage();

#ifdef NAND_ECC_PAGEBUF
	UpdatePageECC();
#endif

	switch(state)
	{
		case WAIT:	return false;

		case READ_START:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (!page.Valid())
			{
				page = readyPageBuffer.Get();

				if (page.Valid())
				{
					PageBuffer &p = *((PageBuffer*)page->GetDataPtr());

					if (!readPageCheckSpareCRC || p.spare.v1.crc == 0)
					{
						u16 col = rd.GetCol();

						rd.SetRawPage(p.page);
						rd.SetCol(col);
					}
					else 
					{
						page.Free(); //freePageBuffer.Add(page); page = 0;

						if (findTryCount > 0)
						{
							findTryCount--;
						}
						else
						{
							// Вектора кончились
							state = FIND_3;
						};
					};
				};
			};

			if (page.Valid())
			{
				PageBuffer &p = *((PageBuffer*)page->GetDataPtr());

				u16 c = rd.pg - rd.GetCol();

				if (rd_count < c) c = rd_count;

				NAND_CopyDataDMA(p.data+rd.GetCol(), rd_data, c);

				rd_count -= c;
				rd.raw += c;
				rd_data += c;
				curRdBuf->len += c;

				state = READ_PAGE;
			};

			break;

		case READ_PAGE:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			if (NAND_CheckCopyComplete())
			{
				PageBuffer &p = *((PageBuffer*)page->GetDataPtr());

				if (rd.GetRawPage() != p.page)
				{ 
					page.Free(); //freePageBuffer.Add(page); page = 0;
				};

				if (rd_count == 0)
				{
					if (vecStart)
					{
						curRdBuf->hdr.crc = GetCRC16(&curRdBuf->hdr, sizeof(curRdBuf->hdr));

						if (curRdBuf->hdr.crc == 0)
						{
							rd_data = curRdBuf->data;
							rd_count = (curRdBuf->hdr.dataLen > curRdBuf->maxLen) ? curRdBuf->maxLen : curRdBuf->hdr.dataLen;
							curRdBuf->len = 0;	
							vecStart = false;

							if (rd_data == 0 || rd_count == 0)
							{
								End();

								return false;
							}
							else
							{
								state = READ_START;
							};
						}
						else if (findTryCount == 0)
						{
							// Вектора кончились
							state = FIND_3;
						}
						else
						{
							// Искать вектор

							findTryCount -= 1;

							if (page.Valid() && (readPageCheckSpareCRC || p.spare.v1.crc == 0))
							{
								SpareArea &spare = p.spare;

								if (spare.v1.vecFstOff == 0xFFFF || spare.v1.vecLstOff == 0xFFFF || rd.GetCol() > spare.v1.vecLstOff)
								{
									rd.NextPage();

									page.Free(); //freePageBuffer.Add(page); page = 0;
								}
								else if (rd.GetCol() <= spare.v1.vecFstOff)
								{
									rd.SetCol(spare.v1.vecFstOff);
								}
								else if (rd.GetCol() <= (spare.v1.vecFstOff+spare.v1.vecFstLen))
								{
									rd.SetCol(spare.v1.vecFstOff+spare.v1.vecFstLen);
								}
								else if (rd.GetCol() <= spare.v1.vecLstOff)
								{
									rd.SetCol(spare.v1.vecLstOff);
								};
							};

							rd_data = (byte*)&curRdBuf->hdr;
							rd_count = sizeof(curRdBuf->hdr);
							curRdBuf->len = 0;	

							state = READ_START;
						};
					}
					else if (curRdBuf->data != 0 && curRdBuf->len != 0)
					{
						#ifdef NAND_READ_CRC_SOFT
							curRdBuf->crc = GetCRC16_CCIT_refl(curRdBuf->data, curRdBuf->len, curRdBuf->crc);
							End();
						#elif defined(NAND_READ_CRC_HW) || defined(NAND_READ_CRC_PIO)
							state =  CRC_START; 
						#else
							curRdBuf->crc = 0;
							End();
						#endif
					}
					else
					{
						End();
					};
				}
				else
				{
					state = READ_START;
				};
			};

			break;

		case FIND_3:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			curRdBuf->len = 0;
			curRdBuf->hdr.dataLen = 0;

			End();

			break;
		
		case FLUSH_PAGES:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			if (!cmdFlushPageBuffer)
			{
				state = READ_START;
			};

			break;

		case CRC_START:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

		#ifdef NAND_READ_CRC_HW

			if (/*statePage == 0 &&*/ CRC_CCITT_DMA_Async(curRdBuf->data, curRdBuf->len, curRdBuf->crc))
			{
				state = CRC_UPDATE;
			};
	
		#elif defined(NAND_READ_CRC_PIO)

			if (CRC_CCITT_PIO(curRdBuf->data, curRdBuf->len, &curRdBuf->crc, curRdBuf->crc))
			{
				End();
			};

		#endif		

			break;

		#ifdef NAND_READ_CRC_HW

			case CRC_UPDATE:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++		
			{
				if (CRC_CCITT_DMA_CheckComplete(&curRdBuf->crc))
				{
					End();
				};

				break;
			};

		#endif

	};

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//static void ReadSpareNow(SpareArea *spare, FLADR *rd, bool crc)
//{
//	//SpareArea *spare;
//	//FLADR *rd;
//
//	NAND_Chip_Select(rd->chip);
//
//	NAND_CmdReadPage(rd->pg, rd->block, rd->page);
//
//	while(!NAND_BUSY());
//	while(NAND_BUSY());
//
//	NAND_ReadDataDMA(spare, sizeof(*spare));
//
//	while (!NAND_CheckDataComplete());
//
//	if (crc)
//	{
//		spare->crc = GetCRC16((void*)&spare->file, sizeof(*spare) - spare->CRC_SKIP);
//	};
//}
//
////+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//static void ReadDataNow(void *dst, u16 len, FLADR *rd)
//{
//	NAND_Chip_Select(rd->chip);
//
//	NAND_CmdReadPage(rd->col, rd->block, rd->page);
//
//	while(!NAND_BUSY());
//	while(NAND_BUSY());
//
//	NAND_ReadDataDMA(dst, len);
//
//	while (!NAND_CheckDataComplete());
//}
//
////+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//static void ReadVecHdrNow(NandVecData::Hdr *h, FLADR *rd)
//{
//	ReadDataNow(h, sizeof(*h), rd);
//
//	h->crc = GetCRC16(h, sizeof(*h));
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void InitSessions()
{
//	__breakpoint(0);

	write.Init();

	if (nvv.f.size > 0)
	{
//		NAND_NextSession();

		write.CreateNextFile();

		while (write.Update()) ;
	};

	u32 ms = 0, me = 0, ls = ~0;
	u32 sp = 0;

	bool bm = false, bl = false;

	//FLADR sb(0), eb(-1); // free start and end block

	for (u16 i = 128, ind = nvv.index; i > 0; i--, ind = (ind-1)&127)
	{
		NandFileDsc &f = nvsi[ind].f;

		if (f.size == 0) continue;

		if (bl)
		{
			if (f.lastPage < f.startPage)
			{
				f.size = 0;
			}
			else
			{
				if (f.lastPage < ls)
				{
					ls = f.startPage;

					if (bm)
					{
						if (f.lastPage <= me)
						{
							f.size = 0;
						}
						else if (f.startPage < me)
						{
							f.startPage = me+1;
						};
					};
				}
				else
				{
					f.size = 0;
				};
			};
		}
		else
		{
			if (f.lastPage < f.startPage)
			{
				bl = true;
				ls = f.startPage;
				sp = 0;
			}
			else
			{
				sp = f.startPage;
			};

			if (!bm)
			{
				bm = true;
				ms = sp;
				me = f.lastPage;
			}
			else if (f.lastPage < ms)
			{
				ms = sp;
			}
			else if (f.lastPage > me)
			{
				f.startPage = me+1;
				me = f.lastPage;
			}
			else
			{
				f.size = 0;
			};
		};



	}; // 	for (u16 i = 128, ind = nvv.index; i > 0; i--, ind = (ind-1)&127)
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void InitSessionsNew()
{
	//__breakpoint(0);

	SpareArea spare;
	ReadSpare rdspr;

	write.Init();

	if (nvv.f.size > 0)
	{
		write.CreateNextFile();

		while (write.Update()) ;
	};

	FLADR firstSessionAdr = write.wr;

	u16 firstSessionNum = 0;
	bool firstSessionValid = false;

	u16 count = 100;

	while (count--)
	{
		rdspr.Start(&spare, &firstSessionAdr);	while (rdspr.Update());

		if (spare.v1.crc == 0)
		{ 
			firstSessionNum = spare.v1.file; 
			firstSessionValid = true; 
			break; 
		};
	};

	bool c = false;

	flashUsedSize = 0;
	flashStartAdr = write.wr.GetRawAdr();

	for (u16 i = 128, ind = nvv.index; i > 0; i--, ind = (ind-1)&127)
	{
		NandFileDsc &f = nvsi[ind].f;

		if (firstSessionValid && f.session == firstSessionNum)
		{
			f.startPage = firstSessionAdr.GetRawPage();
			//f.size = ???;

			firstSessionValid = false;
			c = true;
		}
		else if (c)
		{
			f.size = 0;
		}
		else if (f.size != 0)
		{
			FLADR adr(f.startPage);

			count = 100;

			while (count--)
			{
				rdspr.Start(&spare, &adr);	while (rdspr.Update());

				if (spare.v1.crc == 0 || spare.v1.start == ~0 || spare.v1.fpn == ~0 || spare.v1.rawPage == ~0) break;

				adr.NextPage();
			};

			if (spare.v1.crc != 0 || f.session != spare.v1.file)
			{
				f.size = 0;
			};
		};

		if (!f.Erased()) flashUsedSize += f.size;
	}; 

	u64 fullSize = NandFlash_Full_Size_Get();

	if (flashUsedSize > fullSize) flashUsedSize = fullSize;

	NAND_Chip_Disable();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool NandFlash_AddSessionInfo(NandFileDsc *dsc)
{
	if (dsc == 0) return false;

	if (NandFlash_GetSessionInfo(dsc->session, 0) != 0) return false;

	u16 ind = nvv.index;

	for (u16 i = 128; i > 0; i--)
	{
		NandFileDsc &s = nvsi[ind].f;

		if (s.size == 0) { nvsi[ind].f = *dsc; return true; };

		ind = (ind-1)&127;
	};

	return 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool UpdateBlackBoxSendSessions()
{
	//enum {	WAIT = 0, FIND_LAST_USED_BLOCK, READ_START, READ_1, READ_2, READ_PAGE,READ_PAGE_1,FIND_START,FIND_1,FIND_2,FIND_3,FIND_4, READ_END};

	static byte state = 0;

	static FLADR rd;
	static FLADR adr;

	static SpareArea spare;

	static ReadSpare readSpare;

	static NANDFLRB flrb;

	static u32	firstSessionBlock = ~0;
	static u32	firstSessionLastBlock = ~0;
	static u16	firstSessionNum = ~0;
	static bool firstSessionValid = false;
	static u64	firstSessionStartAdr = 0;
	static u64	firstSessionLastAdr = 0;
	static bool firstSessionSended = false;

	//static u32	curFileFirstBlock = ~0;
//	static u32	curFileLastBlock = ~0;
	static u16	curFileNum = ~0;
	static bool	curFileValid = false;
	static u64	curFileStartAdr = ~0;
//	static u64	curFileEndAdr = ~0;
	//static u32	curFileFPN = ~0;
//	static bool	sendedFileRes = false;
	static u16	sendedFileNum = 0;

	static u32	lastSessionBlock = ~0;
	//static u16	lastSessionNum = ~0;
	//static bool lastSessionValid = false;

	static u32	findFileLastBlock = ~0;
	static u64	findFileStartAdr = 0;
	static u64	findFileEndAdr = 0;
	static u16	findFileNum = ~0;

	//static u32 initFileNum = 0;
	//static u32 initFileStartPage = 0;
	static u32 prgrss = 0;

	static RTC start_rtc;
	static RTC stop_rtc;

	static TM32 tm;

	static u32 count = 0;
	static u32 countFindVec = 0;

	static bool findVector = false;

	const u32 * const bb = readSpare.badBlocks;

	bool result = true;

	switch (state)
	{
		case 0: // Найти последний не битый блок с векторами

			if (cmdSendSession)
			{
				prgrss = 0;

				state++;
			}
			else
			{
				return false;
			};

			break;

		case 1: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


			rd.SetRawBlock(0);

			readSpare.ClearBadBlocks();

			readSpare.Start(&spare, &rd);

			firstSessionBlock = ~0;
			firstSessionLastBlock = ~0;
			firstSessionNum = ~0;
			firstSessionValid = false;
			firstSessionStartAdr = 0;
			firstSessionLastAdr = 0;
			firstSessionSended = false;

//			curFileFirstBlock = ~0;
//			curFileLastBlock = ~0;
			curFileNum = ~0;
			curFileValid = false;
			curFileStartAdr = ~0;
//			curFileEndAdr = ~0;
//			curFileFPN = ~0;
//			sendedFileRes = false;
			sendedFileNum = 0;

			lastSessionBlock = ~0;
			//lastSessionNum = ~0;

			findVector = false;

			count = (rd.RAWBLOCK_MASK+1)*2;

			state++;

			break;

		case 2: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (!readSpare.Update())
			{
				if (spare.v1.crc == 0)
				{
					if (!firstSessionValid)
					{
						if (firstSessionBlock == ~0)
						{
							firstSessionBlock = rd.GetRawBlock();
							firstSessionNum = spare.v1.file;
							firstSessionStartAdr = rd.GetRawAdr();
						}
						else if (firstSessionNum != spare.v1.file)
						{
							firstSessionLastBlock = lastSessionBlock;
							firstSessionValid = true;
							firstSessionLastAdr = curFileStartAdr = rd.GetRawAdr();
//							curFileFPN = spare.v1.fpn;
						};
					}
					else
					{
						if (curFileNum != spare.v1.file)
						{
							findVector = true;

							findFileNum = curFileNum;
							findFileStartAdr = curFileStartAdr;
							//findFileEndAdr = rd.GetRawAdr();
							findFileLastBlock = lastSessionBlock;

							curFileStartAdr	= rd.GetRawAdr();
//							curFileFPN = spare.v1.fpn;
						};
					};

					lastSessionBlock	= rd.GetRawBlock();
					curFileNum			= spare.v1.file;
					curFileValid		= true;
//					curFileEndAdr		= rd.GetRawAdr();

					rd.NextBlock();
				}
				else if (spare.v1.start == ~0 || spare.v1.fpn == ~0 || spare.v1.rawPage == ~0)
				{
					rd.NextBlock();
				}
				else
				{
					rd.NextPage(); count++;
				};

				if (findVector)
				{
					state++;
				}
				else if (count == 0 || rd.CheckOverflow())
				{
					if (!firstSessionValid && firstSessionBlock != ~0)
					{
						findFileNum			= firstSessionNum;
						findFileStartAdr		= firstSessionStartAdr;
						findFileLastBlock		= lastSessionBlock;
						firstSessionLastBlock = lastSessionBlock;
						firstSessionValid		= true;
						firstSessionLastAdr	= curFileStartAdr;
						
						state++;
					}
					else
					{
						state = 9;
					};
				}
				else 
				{
					readSpare.Start(&spare, &rd);

					count--;
				};
			};

			break;

		case 3: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			findVector = false;

			flrb.data = 0;
			flrb.maxLen = 0;
			flrb.vecStart = true;
			flrb.useAdr = true;
			flrb.adr = findFileStartAdr;

			NandFlash_RequestRead(&flrb);

			state++;

			break;

		case 4: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (read.Start())
			{
				state++;
			};
				
			break;

		case 5: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (!read.UpdateFull())
			{
				if (flrb.ready && flrb.hdr.session == findFileNum && flrb.hdr.crc == 0)
				{
					start_rtc = flrb.hdr.rtc;
				};

				adr.SetRawBlock(findFileLastBlock+1);

				findFileEndAdr = adr.GetRawAdr();

				adr.SetRawBlock(findFileLastBlock);

				flrb.data = 0;
				flrb.maxLen = 0;
				flrb.vecStart = true;
				flrb.useAdr = true;
				flrb.adr = adr.GetRawAdr();

				NandFlash_RequestRead(&flrb);

				countFindVec = 10;

				state++;
			};
				
			break;

		case 6: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (read.Start())
			{
				state++;
			};
				
			break;

		case 7: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (!read.UpdateFull())
			{
				if (flrb.ready && flrb.hdr.session == findFileNum && flrb.hdr.crc == 0)
				{
					stop_rtc = flrb.hdr.rtc;

					state++;
				}
				else if (countFindVec > 0)
				{
					countFindVec--;
	
					adr.PrevBlock(); 

					flrb.data = 0;
					flrb.maxLen = 0;
					flrb.vecStart = true;
					flrb.useAdr = true;
					flrb.adr = adr.GetRawAdr();
					
					NandFlash_RequestRead(&flrb);

					state--;
				}
				else
				{
					state++;
				};
			};
				
			break;

		case 8: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		{
			u64 xsize = (findFileEndAdr - findFileStartAdr) & rd.RAWADR_MASK;

			NandFileDsc *sd = NandFlash_GetSessionInfo(findFileNum, findFileStartAdr);

			NandFileDsc dsc;
			dsc.flags		= 0;
			dsc.SetErased();

			if (sd != 0) dsc.flags = sd->flags;

			if (TRAP_MEMORY_SendSession(sendedFileNum = findFileNum, xsize, findFileStartAdr, start_rtc, stop_rtc, dsc.flags))
			{
				if (firstSessionValid && sendedFileNum == firstSessionNum) firstSessionSended = true;

				if (sd == 0)
				{
					dsc.session		= findFileNum;
					dsc.size		= xsize;
					dsc.start_rtc	= start_rtc;
					dsc.stop_rtc	= stop_rtc;

					adr.SetRawAdr(findFileStartAdr);
					dsc.startPage	= adr.GetRawPage();
					adr.SetRawAdr(findFileEndAdr);
					dsc.lastPage	= adr.GetRawPage();

					NandFlash_AddSessionInfo(&dsc);
				};

				if (count == 0 || rd.CheckOverflow())
				{
					state++;
				}
				else
				{
					readSpare.Start(&spare, &rd);

					count--;
					
					state = 2;
				};
			};
				
			break;
		};

		case 9: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (firstSessionValid && !firstSessionSended)
			{
				if (curFileValid && curFileNum == firstSessionNum)
				{
					firstSessionStartAdr = curFileStartAdr;
				};

				findFileNum = firstSessionNum;
				findFileStartAdr = firstSessionStartAdr;
				findFileEndAdr = firstSessionLastAdr;
				findFileLastBlock = firstSessionLastBlock;

				firstSessionSended = true;

				state = 3;
			}
			else if (curFileValid && curFileNum != sendedFileNum)
			{
				findFileNum = curFileNum;
				findFileStartAdr = curFileStartAdr;
				findFileEndAdr = rd.GetRawAdr();
				findFileLastBlock = lastSessionBlock;
				
				state = 3;
			}
			else
			{
				state++;
			};

			break;

		case 10: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (TRAP_MEMORY_SendStatus(~0, NANDFL_STAT_READ_SESSION_READY))
			{
				state++;
			};

			break;

		case 11: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (TRAP_TRACE_PrintString("NAND chip mask: 0x%02hX; Bad Blocks: %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu", nandSize.mask, bb[0], bb[1], bb[2], bb[3], bb[4], bb[5], bb[6], bb[7]))
			{
				cmdSendSession = false;

				state = 0;
			};

			break;
	};

	if (tm.Check(100))
	{
		prgrss = rd.GetRawBlock() * (0x100000000/(rd.RAWBLOCK_MASK+1));

		TRAP_MEMORY_SendStatus(prgrss, NANDFL_STAT_READ_SESSION_IDLE);
	};

	return result;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void NandFlash_StartSendSession()
{
	cmdSendSession = true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool UpdateSendSession()
{
	enum {	WAIT = 0, READ_START, READ_1, READ_2, READ_PAGE,READ_PAGE_1,FIND_START,FIND_1,FIND_2,FIND_3,FIND_4, READ_END};

	static byte i = WAIT;
	
	static u16 ind = 0;
	static u32 prgrss = 0;
	static u16 count = 0;
//	static u32 lp = 0;
//	static u32 sum = 0;
	static FLADR a(0);
	static TM32 tm;

	NandFileDsc &s = nvsi[ind].f;

	__packed const u32* const pe = nvv.pageError;
	__packed const u32* const bb = nvv.badBlocks;

	switch (i)
	{
		case 0:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (cmdSendSession)
			{
				ind = nvv.index;
				prgrss = 0;
				count = 128;

				i++;
			}
			else
			{
				return false;
			};

			break;

		case 1:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (s.size > 0 /*&& !s.Erased()*/)
			{
				if (!TRAP_MEMORY_SendSession(s.session, s.size, (u64)s.startPage * FLADR::pg, s.start_rtc, s.stop_rtc, s.flags))
				{
					break;
				};
			};

			ind = (ind-1)&127;

			prgrss += 0x100000000/128;

			count--;

			i++;

			break;

		case 2:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (NandFlash_SendStatus(prgrss, NANDFL_STAT_READ_SESSION_IDLE))
			{
				if (count > 0)
				{
					i = 1;
				}
				else
				{
					tm.Reset(); count = 3;

					i++;
				};
			};

			break;

		case 3:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (tm.Check(50))
			{
				NandFlash_SendStatus(~0, NANDFL_STAT_READ_SESSION_IDLE);

				if (count > 0) count--; else count = 3, i++;
			};

			break;

		case 4:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (tm.Check(50))
			{
				NandFlash_SendStatus(~0, NANDFL_STAT_READ_SESSION_READY);

				if (count > 0) count--; else  i++;
			};

			break;

		case 5:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (TRAP_TRACE_PrintString("NAND chip mask: 0x%02hX; Bad Blocks: %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu", nandSize.mask, bb[0], bb[1], bb[2], bb[3], bb[4], bb[5], bb[6], bb[7]))
			{
				i++;
			};

			break;

		case 6:

			if (TRAP_TRACE_PrintString("NAND Page Error: %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu", pe[0],pe[1],pe[2],pe[3],pe[4],pe[5],pe[6],pe[7]))
			{
				i++;
			};

			break;

		case 7:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			const byte* p = nandSize.chipDataBusMask;

			if (TRAP_TRACE_PrintString("NAND DataBus Mask: 0x%02hX, 0x%02hX, 0x%02hX, 0x%02hX, 0x%02hX, 0x%02hX, 0x%02hX, 0x%02hX", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]))
			{
				cmdSendSession = false;	i = 0;
			};

			break;
	};

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

NandFileDsc* NandFlash_GetSessionInfo(u16 session, u64 adr)
{
	u16 ind = nvv.index;

	NandFileDsc *s = 0;

	for (u16 i = 128; i > 0; i--)
	{
		s = &nvsi[ind].f;

		if (s->session == session)
		{
			return s;
		};

		ind = (ind-1)&127;
	};

	return 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static FLADR NAND_Idle_er(0);
static EraseBlock NAND_Idle_eraseBlock;

void NAND_Idle()
{
//	register u32 t;

	static i32 t = 0;
	static i32 eb = 0;
	static TM32 tm;
	
	FLADR &er = NAND_Idle_er;
	EraseBlock &eraseBlock = NAND_Idle_eraseBlock;

	//static bool blackBox = false;
	static byte blackBoxCount = 0;

	switch (nandState)
	{
		case NAND_STATE_WAIT: 

			if (cmdCreateNextFile)
			{
				write.CreateNextFile();
				cmdCreateNextFile = false;
				nandState = NAND_STATE_CREATE_FILE;

				break;
			};

			if (cmdFullErase)
			{
				cmdFullErase = false;
				eraseSessionsCount = 1;

#ifdef WIN32
				nandState = NAND_STATE_FULL_ERASE_START;
#endif

				break;
			};

			if (cmdSendSession)
			{
				nandState = NAND_STATE_SEND_SESSION;

				break;
			};

			if (write.Start())
			{
				nandState = NAND_STATE_WRITE_START;
			}
			else if (read.Start())
			{
				nandState = NAND_STATE_READ_START;
			}
			else if (read.statePage != 0)
			{
				read.UpdatePage();
			};

			FLASH_UpdateStatus();

			break;

		case NAND_STATE_WRITE_START:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (!write.Update())
			{
				nandState = NAND_STATE_WAIT;
			};

			break;

		case NAND_STATE_READ_START:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (!read.Update())
			{
				nandState = NAND_STATE_WAIT;
			};

			break;

		case NAND_STATE_FULL_ERASE_START:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			t = eb = nandSize.fl >> er.CHIP_OFF; // blocks count

			er.SetRawAdr(0);

			eraseBlock.Start(er, true, true);

			nandState = NAND_STATE_FULL_ERASE_0;

			break;

		case NAND_STATE_FULL_ERASE_0:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (!eraseBlock.Update())
			{
				invalidBlocks += eraseBlock.errBlocks;

				t -= eraseBlock.errBlocks;
				t -= 1;

				write.errVec = t;

				if (t > 0)
				{
					er.NextBlock();

					eraseBlock.Start(er, true, true);

					if (tm.Check(500)) 
					{ 
						NandFlash_SendStatus((eb-t)*((u64)0x100000000)/eb, NANDFL_STAT_BUSY); 
#ifdef WIN32
						Printf(0, 3, 0xF0, "%9li", t);
#endif
					};
				}
				else
				{
					flashEmpty = true;

					nvv.Init();
					NandFlash_InitNVSI();

					write.Init();

					//adrLastVector = ~0;

					NandFlash_SendStatus(~0, NANDFL_STAT_ERASE);
					nandState = NAND_STATE_WAIT;
					eraseSessionsCount = 1;
				};
			};

			//if (tm.Check(200) && IsComputerFind() && EmacIsConnected()) TRAP_MEMORY_SendStatus(lastFlashProgress, lastFlashStatus);

			break;

		case NAND_STATE_CREATE_FILE:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (!write.Update())
			{
				nandState = NAND_STATE_WAIT;
			};

			break;

		case NAND_STATE_SEND_SESSION:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (blackBoxCount >= 10)
			{
				if (!UpdateBlackBoxSendSessions())
				{
					blackBoxCount = 0;

					nandState = NAND_STATE_WAIT;
				};
			}
			else
			{
				if (!UpdateSendSession())
				{
					blackBoxCount += 1;

					nandState = NAND_STATE_WAIT;
				};
			};

			break;
	};
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32 NandFlash_Vectors_Errors_Get()
{
	return write.errVec;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32 NandFlash_Vectors_Saved_Get()
{
	return write.spare.v1.vectorCount;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32 NandFlash_Vectors_Recieved_Get()
{
	return write.rcvVec;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32 NandFlash_Vectors_Rejected_Get()
{
	return write.rejVec;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32 NandFlash_Session_Get()
{
	return write.spare.v1.file;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u64 NandFlash_Current_Adress_Get()
{
	return write.wr.GetRawAdr();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u64 NandFlash_Full_Size_Get()
{
	return nandSize.fl;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u64 NandFlash_Used_Size_Get()
{
	u64 t = write.wr.raw - flashStartAdr;

	t += flashUsedSize;

	if (t > nandSize.fl) t = nandSize.fl;

	return t;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//i64 FLASH_Empty_Size_Get()
//{
//	return NandFlash_Full_Size_Get() - NandFlash_Used_Size_Get();
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16 NandFlash_Chip_Mask_Get()
{
	return nandSize.mask;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16 NandFlash_BlockErr_Get()
{
	return write.eraseBlock.errErase;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16 NandFlash_PageErr_Get()
{
	return write.spare.v1.fbp;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16 NandFlash_BlockSize_Get()
{
	return (1UL<<FLADR::BLOCK_BITS);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32 NandFlash_ErrECC_Get()
{
#ifdef NAND_ECC_CHECK
	return write.eccErrCount;
#else
	return 0;
#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32 NandFlash_CorrectedErrECC_Get()
{
#ifdef NAND_ECC_CHECK
	return write.eccCorrErrCount;
#else
	return 0;
#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32 NandFlash_ParityErrECC_Get()
{
#ifdef NAND_ECC_CHECK
	return write.eccParityErrCount;
#else
	return 0;
#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// удалить 
//bool FLASH_Erase_Full()
//{
//	//if(flash_status_operation != NANDFL_STAT_OPERATION_WAIT) return false;
//	//FRAM_Memory_Start_Adress_Set(FRAM_Memory_Current_Adress_Get());
//	return true;
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// восстановить
//bool FLASH_UnErase_Full()
//{
//	//if(flash_status_operation != NANDFL_STAT_OPERATION_WAIT) return false;
//	//FRAM_Memory_Start_Adress_Set(-1);
//	return true;
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool NandFlash_RequestWrite(Ptr<MB> &fwb, u16 devID, bool updateCRC)
{
	bool result = false;

	if (!fwb.Valid())
	{
		//		freeReqList.Add(req);
	}
	else
	{
		if (fwb->len > 0 && fwb->dataOffset >= sizeof(NandVecData::Hdr))
		{
			//fwb->dataOffset -= sizeof(NandVecData::Hdr);

			NandVecData* vd = (NandVecData*)((byte*)fwb->GetDataPtr()-sizeof(NandVecData::Hdr));

			vd->h.device = deviceID = devID;
			vd->h.flags = updateCRC;

			result = writeFlBuf.Add(fwb);
		};
	};

	return result;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool NandFlash_WriteEnable()
{
	bool result = false;

	if (!writeFlashEnabled)
	{
		nvv.f.size = 0;
		GetTime(&nvv.f.start_rtc);

		result = true;
	};

	writeFlashEnabled = true;
	flashStatus = NANDFL_STAT_WRITE;

	return result;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void NandFlash_WriteDisable()
{
	if (writeFlashEnabled && nvv.f.size > 0)
	{
		NAND_NextSession();
	};

	writeFlashEnabled = false;

	flashStatus = NANDFL_STAT_WAIT;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

byte NandFlash_Status()
{
	return flashStatus;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void NandFlash_LoadVars()
{
	SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_GREEN "Flash Load Vars ... ");

	PointerCRC p(buf);

	static DSCI2C dsc;
	static DSCSPI spi;
	static u16 romAdr = 0;
	static TM32 tm;

	NVV nv1, nv2;
	bool c1 = false;
	bool c2 = false;

	spi.adr = (ReverseDword(FRAM_SPI_SESSIONS_ADR) & ~0xFF) | 3;
	spi.alen = 4;
	spi.csnum = 1;
	spi.wdata = 0;
	spi.wlen = 0;
	spi.rdata = buf;
	spi.rlen = sizeof(buf);

	if (SPI_AddRequest(&spi))
	{
		while (!(spi.ready || tm.Check(200))) { SPI_Update(); };
	};

	for (byte i = 0; i < 2; i++)
	{
		p.CRC.w = 0xFFFF;
		p.ReadArrayB(&nv1, sizeof(nv1));
		p.ReadW();

		if (p.CRC.w == 0) { c1 = true; break; };
	};

	romAdr = ReverseWord(FRAM_I2C_SESSIONS_ADR);

	dsc.wdata = &romAdr;
	dsc.wlen = sizeof(romAdr);
	dsc.wdata2 = 0;
	dsc.wlen2 = 0;
	dsc.rdata = buf;
	dsc.rlen = sizeof(buf);
	dsc.adr = 0x50;

	if (I2C_AddRequest(&dsc))
	{
		while (!(dsc.ready || tm.Check(100))) { I2C_Update(); };
	};

	loadVarsOk = false;

	p.b = buf;

	for (byte i = 0; i < 2; i++)
	{
		p.CRC.w = 0xFFFF;
		p.ReadArrayB(&nv2, sizeof(nv2));
		p.ReadW();

		if (p.CRC.w == 0) { c2 = true; break; };
	};

	fram_SPI_flashVarsOk = c1;
	fram_I2C_flashVarsOk = c2;

	SEGGER_RTT_WriteString(0, "FRAM SPI - "); SEGGER_RTT_WriteString(0, (c1) ? (RTT_CTRL_TEXT_BRIGHT_GREEN "OK") : (RTT_CTRL_TEXT_BRIGHT_RED "ERROR"));

	SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_WHITE " ... FRAM I2C - "); SEGGER_RTT_WriteString(0, (c2) ? (RTT_CTRL_TEXT_BRIGHT_GREEN "OK\n") : (RTT_CTRL_TEXT_BRIGHT_RED "ERROR\n"));

	if (c1 && c2)
	{
		if (nv1.timeStamp > nv2.timeStamp)
		{
			c2 = false;
		}
		else if (nv1.timeStamp < nv2.timeStamp)
		{
			c1 = false;
		};
	};

	if (c1)	{ nvv = nv1; } else if (c2) { nvv = nv2; };

	loadVarsOk = c1 || c2;

	if (!c1 || !c2)
	{
		if (!loadVarsOk)
		{
			nvv.Init();
		};

		savesCount = 2;
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void NandFlash_SaveVars()
{
	const u16 sa = 0x100;

	PointerCRC p(buf);

	static DSCI2C dsc;
	static DSCSPI spi, spi2;
	static u16 romAdr = 0;

	static byte i = 0;
	static TM32 tm;

	switch (i)
	{
		case 0:

			if (tm.Check(200))
			{
				if (savesCount > 0)
				{
					savesCount--;
					i++;
				}
				else if (savesSessionsCount > 0)
				{
					savesSessionsCount--;
					i = 5;
				}
				else if (eraseSessionsCount > 0)
				{
					eraseSessionsCount--;

					for (u16 n = 0; n < ArraySize(nvsi); n++)
					{
						NVSI &d = nvsi[n];

						if (d.f.size != 0) d.f.SetErased(), d.UpdateCRC();
					};

					if (nvv.f.size != 0) nvv.f.SetErased();

					flashUsedSize = 0;

					savesCount = 1;

					i = 6;
				};
			};

			break;

		case 1:

			nvv.timeStamp = GetMilliseconds();

			for (byte j = 0; j < 2; j++)
			{
				p.CRC.w = 0xFFFF;
				p.WriteArrayB(&nvv, sizeof(nvv));
				p.WriteW(p.CRC.w);
			};

			romAdr = ReverseWord(FRAM_I2C_SESSIONS_ADR);

			dsc.wdata = &romAdr;
			dsc.wlen = sizeof(romAdr);
			dsc.wdata2 = buf;
			dsc.wlen2 = sizeof(buf);
			dsc.rdata = 0;
			dsc.rlen = 0;
			dsc.adr = 0x50;

			spi.adr = (ReverseDword(FRAM_SPI_SESSIONS_ADR) & ~0xFF)|2;
			spi.alen = 4;
			spi.csnum = 1;
			spi.wdata = buf;
			spi.wlen = sizeof(buf);
			spi.rdata = 0;
			spi.rlen = 0;

			spi2.adr = 6;
			spi2.alen = 1;
			spi2.csnum = 1;
			spi2.wdata = 0;
			spi2.wlen = 0;
			spi2.rdata = 0;
			spi2.rlen = 0;

			tm.Reset();

			i = (I2C_AddRequest(&dsc)) ? (i+1) : 0;

			break;

		case 2:

			if (dsc.ready || tm.Check(100))
			{
				tm.Reset();

				SPI_AddRequest(&spi2);

				i++;
			};

			break;

		case 3:

			if (spi2.ready || tm.Check(200))
			{
				SPI_AddRequest(&spi);

				i++;
			};

			break;

		case 4:

			if (spi.ready || tm.Check(200))
			{
				i = 0;
			};

			break;

		case 5:

			{
				NVSI &si = nvsi[nvv.index];

				si.timeStamp = GetMilliseconds();

				//u32 adr = sa+sizeof(si)*nvv.index;

				romAdr = ReverseWord(FRAM_I2C_SESSIONS_ADR+sa+sizeof(si)*nvv.index);

				dsc.wdata = &romAdr;
				dsc.wlen = sizeof(romAdr);
				dsc.wdata2 = buf;
				dsc.wlen2 = sizeof(si);
				dsc.rdata = 0;
				dsc.rlen = 0;
				dsc.adr = 0x50;

				spi.adr = (ReverseDword(FRAM_SPI_SESSIONS_ADR+sa+sizeof(si)*nvv.index) & ~0xFF) | 2;
				spi.alen = 4;
				spi.csnum = 1;
				spi.wdata = buf;
				spi.wlen = sizeof(si);
				spi.rdata = 0;
				spi.rlen = 0;

				spi2.adr = 6;
				spi2.alen = 1;
				spi2.csnum = 1;
				spi2.wdata = 0;
				spi2.wlen = 0;
				spi2.rdata = 0;
				spi2.rlen = 0;

				p.CRC.w = 0xFFFF;
				p.WriteArrayB(&si, sizeof(si)-2);
				p.WriteW(p.CRC.w);

				tm.Reset();

				i = (I2C_AddRequest(&dsc)) ? 2 : 0;
			};

			break;

		case 6:

			romAdr = ReverseWord(FRAM_I2C_SESSIONS_ADR+sa);

			dsc.wdata = &romAdr;
			dsc.wlen = sizeof(romAdr);
			dsc.wdata2 = nvsi;
			dsc.wlen2 = sizeof(nvsi);
			dsc.rdata = 0;
			dsc.rlen = 0;
			dsc.adr = 0x50;

			spi.adr = (ReverseDword(FRAM_SPI_SESSIONS_ADR+sa) & ~0xFF) | 2;
			spi.alen = 4;
			spi.csnum = 1;
			spi.wdata = nvsi;
			spi.wlen = sizeof(nvsi);
			spi.rdata = 0;
			spi.rlen = 0;

			spi2.adr = 6;
			spi2.alen = 1;
			spi2.csnum = 1;
			spi2.wdata = 0;
			spi2.wlen = 0;
			spi2.rdata = 0;
			spi2.rlen = 0;

			i = (I2C_AddRequest(&dsc)) ? 2 : 0;

			break;
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void NandFlash_LoadSessions()
{
	SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_YELLOW "Flash Load Sessions ... ");

	static DSCI2C dsc;
	static DSCSPI spi, spi2;
	static u16 romAdr = 0;
	static u32 spiAdr = 0;
	static TM32 tm;

	const u16 sa = 0x100;

	loadSessionsOk = true;
	fram_SPI_loadSessionsOk = true;
	fram_I2C_loadSessionsOk = true;

	NVSI nv1, nv2;

	for (u16 i = 0; i < ArraySize(nvsi); i++)
	{
		NVSI &si = nvsi[i];

		spiAdr = (ReverseDword(FRAM_SPI_SESSIONS_ADR + sa + sizeof(nv1) * i) & ~0xFF);

		spi.adr = spiAdr | 3;
		spi.alen = sizeof(spiAdr);
		spi.csnum = 1;
		spi.wdata = 0;
		spi.wlen = 0;
		spi.rdata = &nv1;
		spi.rlen = sizeof(nv1);

		tm.Reset();

		SPI_AddRequest(&spi);

		while (!(spi.ready || tm.Check(200))) { SPI_Update(); };

		romAdr = ReverseWord(FRAM_I2C_SESSIONS_ADR+sa+sizeof(nv2)*i);

		dsc.wdata = &romAdr;
		dsc.wlen = sizeof(romAdr);
		dsc.wdata2 = 0;
		dsc.wlen2 = 0;
		dsc.rdata = &nv2;
		dsc.rlen = sizeof(nv2);
		dsc.adr = 0x50;

		tm.Reset();

		if (I2C_AddRequest(&dsc))
		{
			while (!(dsc.ready || tm.Check(100))) { I2C_Update(); };
		};

		bool c1 = GetCRC16(&nv1, sizeof(nv1)) == 0;
		bool c2 = GetCRC16(&nv2, sizeof(nv2)) == 0;

		if (!c1)
		{
			fram_SPI_loadSessionsOk = false;
		};

		if (!c2)
		{
			fram_I2C_loadSessionsOk = false;
		};

		if (c1 && c2)
		{
			if (nv1.timeStamp > nv2.timeStamp)
			{
				c2 = false;
			}
			else if (nv2.timeStamp > nv1.timeStamp)
			{
				c1 = false;
			};
		};

		if (c1)	{ si = nv1; } else if (c2) { si = nv2; };

		if (!c1 || !c2)
		{
			if (!c1 && !c2)
			{
				loadSessionsOk = false;

				si.f.session = 0;
				si.f.size = 0;
				si.f.startPage = 0;
				si.f.lastPage = 0;
				si.f.flags = 0;
				si.f.start_rtc.date = 0;
				si.f.start_rtc.time = 0;
				si.f.stop_rtc.date = 0;
				si.f.stop_rtc.time = 0;
				si.timeStamp = 0;
				//si.crc = GetCRC16(&si, sizeof(si) - 2);
				si.UpdateCRC();

				dsc.wdata2 = &si;
				dsc.wlen2 = sizeof(si);

				spi.wdata = &si;
				spi.wlen = sizeof(si);
			}
			else if (c1)
			{
				dsc.wdata2 = &nv1;
				dsc.wlen2 = sizeof(nv1);
			}
			else if (c2)
			{
				spi.wdata = &nv2;
				spi.wlen = sizeof(nv2);
			};

			if (!c1)
			{
				spi.adr = spiAdr | 2;
				spi.alen = sizeof(spiAdr);
				spi.csnum = 1;
				spi.rdata = 0;
				spi.rlen = 0;

				spi2.adr = 6;
				spi2.alen = 1;
				spi2.csnum = 1;
				spi2.wdata = 0;
				spi2.wlen = 0;
				spi2.rdata = 0;
				spi2.rlen = 0;

				tm.Reset();

				SPI_AddRequest(&spi2);
				SPI_AddRequest(&spi);

				while (!((spi.ready && spi2.ready) || tm.Check(200))) { SPI_Update(); };
			};

			if (!c2)
			{
				dsc.wdata = &romAdr;
				dsc.wlen = sizeof(romAdr);
				dsc.rdata = 0;
				dsc.rlen = 0;
				dsc.adr = 0x50;

				tm.Reset();

				I2C_AddRequest(&dsc);

				while (!(dsc.ready || tm.Check(100))) { I2C_Update(); };
			};
		};
	};

	SEGGER_RTT_WriteString(0, "FRAM SPI - "); SEGGER_RTT_WriteString(0, (fram_SPI_loadSessionsOk) ? (RTT_CTRL_TEXT_BRIGHT_GREEN "OK") : (RTT_CTRL_TEXT_BRIGHT_RED "ERROR"));

	SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_WHITE " ... FRAM I2C - "); SEGGER_RTT_WriteString(0, (fram_I2C_loadSessionsOk) ? (RTT_CTRL_TEXT_BRIGHT_GREEN "OK\n") : (RTT_CTRL_TEXT_BRIGHT_RED "ERROR\n"));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void NandFlash_Init()
{
	NAND_Init();

#ifdef NAND_ECC_CHECK
	
	SpareArea spare;

	u16 size = sizeof(spare) - sizeof(spare.ecc_page) + (FLADR::pg/NAND_ECC_LEN)*3;

	if (FLADR::spareSize < size || !(check_ecc_page||check_ecc_pagebuf))
	{
		size -= (FLADR::pg/NAND_ECC_LEN)*3;

		#ifdef NAND_ECC_PAGE
			check_ecc_page = false;					
		#endif

		#ifdef NAND_ECC_PAGEBUF
			check_ecc_pagebuf = false;					
		#endif
	};

	FLADR::spareSize = size;

#endif

//	FRAM_SPI_PRESENT = (Get_FRAM_SPI_SessionsAdr() >= 0);
//	FRAM_I2C_PRESENT = (Get_FRAM_I2C_SessionsAdr() >= 0);
//	FRAM_SPI_SESSIONS_ADR = Get_FRAM_SPI_SessionsAdr();
//	FRAM_I2C_SESSIONS_ADR = Get_FRAM_I2C_SessionsAdr();

	NandFlash_LoadVars();

	NandFlash_LoadSessions();

	InitFlashBuffer();

	InitSessionsNew();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void NandFlash_Update()
{
	NandFlash_SaveVars();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // #if !defined(NANDFLASH_IMP_V2_H__18_09_2025__18_28) && defined(NANDFLASH_IMP_VERSION)

