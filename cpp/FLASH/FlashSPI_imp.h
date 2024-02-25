#ifndef FLASHINT_IMP_H__24_02_2024__16_53
#define FLASHINT_IMP_H__24_02_2024__16_53

#pragma once

#include "core.h"
#include "FlashMem.h"

#include "CRC\CRC16.h"
#include "list.h"
#include "spi.h"
#include "time.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef FLASH_START_ADR
#define FLASH_START_ADR 0x10000 	
#endif

#ifndef FLASH_SECTOR_SIZE
#define FLASH_SECTOR_SIZE	4096
#endif

#ifndef FLASH_PAGE_SIZE
#define FLASH_PAGE_SIZE		256
#endif

#ifndef NUM_SECTORS
#define NUM_SECTORS 	64			/* number of sectors in the flash device */
#endif

#ifndef BAUD_RATE_DIVISOR
#define BAUD_RATE_DIVISOR 	50
#endif

#ifndef AT25_SPI_BAUD_RATE
#define AT25_SPI_BAUD_RATE 	1000000
#endif

#define PAGEDWORDS		(FLASH_PAGE_SIZE/4)
#define FLASH_START		FLASH_START_ADR
#define ISP_PAGESIZE	FLASH_PAGE_SIZE

#define SPIMODE	(CPOL|CPHA)

static char 		*pFlashDesc =		"Atmel AT25DF021";
static char 		*pDeviceCompany	=	"Atmel Corporation";

static int			gNumSectors = NUM_SECTORS;

//static u32			bufsect[(SECTOR_SIZE+3)/4];


/* flash commands */
#define SPI_WREN            (0x06)  //Set Write Enable Latch
#define SPI_WRDI            (0x04)  //Reset Write Enable Latch
#define SPI_RDID            (0x9F)  //Read Identification
#define SPI_RDSR            (0x05)  //Read Status Register
#define SPI_WRSR            (0x01)  //Write Status Register
#define SPI_READ            (0x03)  //Read data from memory
#define SPI_FAST_READ       (0x0B)  //Read data from memory
#define SPI_PP              (0x02)  //Program Data into memory
#define SPI_SE              (0x20)  //Erase one sector in memory
#define SPI_BE              (0xC7)  //Erase all memory
#define RDY_BSY 			(0x1)	//Check the write in progress bit of the SPI status register
#define WEL					(0x2)	//Check the write enable bit of the SPI status register
#define SWP					(0xC)	//Software Protection Status
#define WPP					(0x10)	//Write Protect (WP) Pin Status
#define EPE					(0x20)	//Erase/Program Error
#define SPRL				(0x80)	//Sector Protection Registers Locked

#define SPI_UPS				(0x39)  //Unprotect Sector 
#define SPI_PRS				(0x36)  //Protect Sector 


#define SPI_PAGE_SIZE		(256)

#define DELAY			15
#define TIMEOUT        35000*64

enum ERROR_CODE
{
	NO_ERR,					/* No Error */
	POLL_TIMEOUT,			/* Polling toggle bit failed */
	VERIFY_WRITE,			/* Verifying write to flash failed */
	INVALID_SECTOR,			/* Invalid Sector */
	INVALID_BLOCK,			/* Invalid Block */
	UNKNOWN_COMMAND,		/* Unknown Command */
	PROCESS_COMMAND_ERR,	/* Processing command */
	NOT_READ_ERROR,			/* Could not read memory from target */
	DRV_NOTAT_BREAK,		/* The drive was not at AFP_BreakReady */
	BUFFER_IS_NULL,			/* Could not allocate storage for the buffer */
	NO_ACCESS_SECTOR,		/* Cannot access the sector( could be locked or something is stored there that should not be touched ) */
	NUM_ERROR_CODES,
	ERROR_ERASE,
	ERROR_PROGRAM,
	NOT_WRITE_ENABLED
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class FlashSPI : public FlashMem
{
public:

	//struct FLWB
	//{
	//	u32		adr;
	//	u32 	dataLen;
	//	u32 	dataOffset;
	//	byte	data[0];
	//};

protected:
	
	enum FlashState 
	{ 
		FLASH_STATE_WAIT = 0, 
		FLASH_STATE_ERASE_START, 
		FLASH_STATE_ERASE_WAIT,
		FLASH_STATE_ERASE_WAIT_2,
		FLASH_STATE_ERASE_CHECK,
		//FLASH_STATE_WRITE_START, 
		FLASH_STATE_WRITE_PAGE, 
		FLASH_STATE_WRITE_PAGE_2, 
		FLASH_STATE_WRITE_PAGE_3, 
		FLASH_STATE_WRITE_PAGE_4, 
		FLASH_STATE_VERIFY_PAGE
	};

	FlashState flashState;

	CTM32 tm;
	byte*		flashWritePtr;
	u16			flashWriteLen;
	u32			flashWriteAdr;

	//u32 secStartAdr;
	//u32 secEndAdr;
	//u32 wadr;
	//u32 wlen;
	u32 flash_write_error;
	u32 flash_write_ok;
	//__packed u32 *wdata;

	S_SPIM	&spi;

	FLWB *flwb;
	Ptr<MB> curFlwb;
	ListPtr<MB> writeFlBuf;

	ERROR_CODE	lastError;


	u16	lastErasedBlock;

	#ifdef ADSP_CHECKFLASH
		ADI_BOOT_HEADER bh;
		u16 crc = 0;
	#endif

	byte buf[10];
	byte bufpage[SPI_PAGE_SIZE];

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	inline void ChipEnable() {	spi.ChipSelect(0, SPIMODE, BAUD_RATE_DIVISOR); }
	inline void ChipDisable() { spi.ChipDisable(); }

	void Delay(u32 us);

	ERROR_CODE GetLastError() {	return lastError; }

	void __SendSingleCommand( const int iCommand );

	void CmdWriteEnable() {	__SendSingleCommand(SPI_WREN); }
	void CmdWriteDisable(){	__SendSingleCommand(SPI_WRDI); }
	void CmdEraseSector(u32 sec);
	void CmdWriteStatusReg(byte stat);

	u32 GetNumSectors() { return gNumSectors; }
	u32 GetSectorSize() { return FLASH_SECTOR_SIZE; }
	
	void GlobalUnProtect() { CmdWriteEnable(); CmdWriteStatusReg(0); }

	u16 GetCRC16(u32 stAdr, u16 count);
	
	ERROR_CODE VerifyPage(const byte *data, u32 stAdr, u16 count );
	ERROR_CODE EraseBlock(int nBlock);
	ERROR_CODE GetSectorStartEnd( unsigned long *ulStartOff, unsigned long *ulEndOff, int nSector );
	ERROR_CODE Wait_For_WEL(void);
	ERROR_CODE Wait_For_Status( char Statusbit );

	byte ReadStatusRegister(void);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

public:

	u16 flashCRC;
	u32 flashLen;
	bool flashOK;
	bool flashChecked;
	bool flashCRCOK;

	u32 Get_WriteError() { return flash_write_error; }
	u32 Get_WriteOK() { return flash_write_ok; }

	u32		GetSectorAdrLen(u32 sadr, u32 *radr);
	void	Update();
	u32		Read(u32 addr, void *data, u32 size);
	void	InitFlashWrite() { /*state_write_flash = WRITE_INIT;*/ }
	bool	Busy() { return (flashState != FLASH_STATE_WAIT) || !writeFlBuf.Empty(); }
	bool	RequestWrite(Ptr<MB> &b);
	void	Init();
	u16		CRC16(u32 len, u32 *rlen);

	FlashSPI(S_SPIM &sp) : FlashMem(FLASH_PAGE_SIZE, FLASH_START_ADR), flashState(FLASH_STATE_WAIT), flashCRC(0), flashLen(0), flashOK(false), flashChecked(false), flashCRCOK(false),
							lastErasedBlock(~0), lastError(NO_ERR), flwb(0), flash_write_error(0), flash_write_ok(0), spi(sp) {}

#ifdef ADSP_CHECKFLASH

	void ADSP_CheckFlash();

#endif
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void FlashSPI::Init()
{
	spi.Connect(AT25_SPI_BAUD_RATE);

#ifdef ADSP_CHECKFLASH

	ADSP_CheckFlash();

#endif

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16 FlashSPI::CRC16(u32 len, u32 *rlen)
{
	u16 crc;

	if (flashChecked)
	{
		crc = flashCRC;
		len = flashLen;
	}
	else
	{
		crc = this->GetCRC16(FLASH_START_ADR, len);
	};

	if (rlen != 0) *rlen = len;

	return crc;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool FlashSPI::RequestWrite(Ptr<MB> &b)
{
	FLWB &flwb = *((FLWB*)b->GetDataPtr());

	if ((b.Valid()) && (flwb.dataLen > 0))
	{
		writeFlBuf.Add(b);

		return true;
	}
	else
	{
		return false;
	};
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32 FlashSPI::GetSectorAdrLen(u32 sadr, u32 *radr)
{
	//sadr += BOOTSIZE;

	//if (sadr >= PLANESIZE)
	//{
	//	return 0;
	//};


	//if (len != 0 && radr != 0)
	//{
	//	*radr = sadr;
	//};

	//return len;

	return 0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void FlashSPI::Delay(u32 us)
{
	for(u32 n=0; n < (us*NS2CCLK(10)); n++)	__nop();
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void FlashSPI::__SendSingleCommand( const int iCommand )
{
	ChipEnable();

	spi.WriteReadByte(iCommand);

	ChipDisable();

	Delay(1);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void FlashSPI::CmdEraseSector(u32 sec)
{
	ChipEnable();

	sec *= FLASH_SECTOR_SIZE;

	buf[0] = SPI_SE;
	buf[1] = sec >> 16;
	buf[2] = sec >> 8;
	buf[3] = sec;

	spi.WriteSyncDMA(buf, 4);

	ChipDisable();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void FlashSPI::CmdWriteStatusReg(byte stat)
{
	ChipEnable();

	buf[0] = SPI_WRSR;
	buf[1] = stat;

	spi.WriteSyncDMA(buf, 2);

	ChipDisable();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32 FlashSPI::Read(u32 addr, void *data, u32 size)
{
    buf[0] = SPI_FAST_READ;
    buf[1] = addr >> 16;
    buf[2] = addr >> 8;
    buf[3] = addr;
    buf[4] = 0;

	ChipEnable();

	spi.WriteSyncDMA(buf, 5);

	spi.ReadSyncDMA(data, size);

	ChipDisable();

	return size;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16 FlashSPI::GetCRC16(u32 stAdr, u16 count)
{
	DataCRC crc;

	crc.w = 0xFFFF;

	u16 t = 0;

    buf[0] = SPI_FAST_READ;
    buf[1] = stAdr >> 16;
    buf[2] = stAdr >> 8;
    buf[3] = stAdr;
    buf[4] = 0;

	ChipEnable();

	spi.WriteSyncDMA(buf, 5);

	for ( ; count > 0; count--)
	{
		t = spi.WriteReadByte(0);

		crc.w = tableCRC[crc.b[0] ^ t] ^ crc.b[1];
		
		HW::ResetWDT();
	};
	
	ChipDisable();

	return crc.w;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE FlashSPI::VerifyPage(const byte *data, u32 stAdr, u16 count )
{
    ERROR_CODE Result = NO_ERR;

	if ((stAdr & 0xFF) != 0 || count > 256 || count == 0)
	{
		return INVALID_BLOCK;
	};

	ChipEnable();

	static byte buf[5];

    buf[0] = SPI_PP;
    buf[1] = stAdr >> 16;
    buf[2] = stAdr >> 8;
    buf[3] = stAdr;
    buf[0] = 0;

	spi.WriteSyncDMA(buf, sizeof(buf));

	for ( ; count > 0; count--)
	{
		if (spi.WriteReadByte(0) != *data)
		{
			Result = VERIFY_WRITE;
			break;
		};

		data++;
	};

	ChipDisable();

	return Result;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE FlashSPI::EraseBlock(int nBlock)
{

	ERROR_CODE 	  ErrorCode   = NO_ERR;		//tells us if there was an error erasing flash
 	unsigned long ulSectStart = 0x0;		//stores the sector start offset
 	unsigned long ulSectEnd   = 0x0;		//stores the sector end offset(however we do not use it here)

	// Get the sector start offset
	// we get the end offset too however we do not actually use it for Erase sector
	GetSectorStartEnd( &ulSectStart, &ulSectEnd, nBlock );

	GlobalUnProtect();
	GlobalUnProtect();

	CmdWriteEnable();

	CmdEraseSector(ulSectStart);

	ErrorCode = Wait_For_Status(RDY_BSY);

	return ErrorCode;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE FlashSPI::GetSectorStartEnd( unsigned long *ulStartOff, unsigned long *ulEndOff, int nSector )
{
	u32 ulSectorSize = FLASH_SECTOR_SIZE;

	if( ( nSector >= 0 ) && ( nSector < gNumSectors ) ) // 32 sectors
	{
		*ulStartOff = nSector * ulSectorSize;
		*ulEndOff = ( (*ulStartOff) + ulSectorSize - 1 );
	}
	else
	{
		return INVALID_SECTOR;
	};

	// ok
	return NO_ERR;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

byte FlashSPI::ReadStatusRegister(void)
{
	ChipEnable();

	spi.WriteReadByte(SPI_RDSR);

	byte usStatus = spi.WriteReadByte(0);

	ChipDisable();		

	return usStatus;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE FlashSPI::Wait_For_WEL(void)
{
	volatile int n, i;
	char status_register = 0;
	ERROR_CODE ErrorCode = NO_ERR;	// tells us if there was an error erasing flash

	for(i = 0; i < 35; i++)
	{
		status_register = ReadStatusRegister();

		if( (status_register & WEL) )
		{
			ErrorCode = NO_ERR;	// tells us if there was an error erasing flash
			break;
		};

		Delay(DELAY);

		ErrorCode = POLL_TIMEOUT;	// Time out error

		HW::ResetWDT();
	};

	return ErrorCode;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE FlashSPI::Wait_For_Status( char Statusbit )
{
	volatile int n, i;
	char status_register = 0xFF;
	ERROR_CODE ErrorCode = NO_ERR;	// tells us if there was an error erasing flash

	for(i = 0; i < TIMEOUT; i++)
	{
		status_register = ReadStatusRegister();
		if( !(status_register & Statusbit) )
		{
			ErrorCode = NO_ERR;	// tells us if there was an error erasing flash
			break;
		}

		Delay(DELAY);

		ErrorCode = POLL_TIMEOUT;	// Time out error

		HW::ResetWDT();
	};

	return ErrorCode;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void FlashSPI::Update()
{
	switch (flashState)
	{
		case FLASH_STATE_WAIT:

			curFlwb = writeFlBuf.Get();

			if (curFlwb.Valid())
			{
				flwb = (FLWB*)curFlwb->GetDataPtr();

				flashWriteAdr = flwb->adr; //FLASH_START_ADR + request->stAdr;
				flashWritePtr = flwb->data + flwb->dataOffset;
				flashWriteLen = flwb->dataLen;

				u16 block = flashWriteAdr/FLASH_SECTOR_SIZE;

				if (lastErasedBlock != block)
				{
					lastErasedBlock = block;

					flashState = FLASH_STATE_ERASE_START;
				}
				else
				{
					flashState = FLASH_STATE_WRITE_PAGE;
				};
			};

			break;

		case FLASH_STATE_ERASE_START:

			GlobalUnProtect();
			GlobalUnProtect();

			CmdWriteEnable();

			tm.Reset();

			flashState = FLASH_STATE_ERASE_WAIT;

			break;

		case FLASH_STATE_ERASE_WAIT:
		{ 
			byte st = ReadStatusRegister();

			if ((st & RDY_BSY) == 0 && (st & WEL) != 0)
			{
				lastError = NO_ERR;

				CmdEraseSector(lastErasedBlock);

				tm.Reset();

				flashState = FLASH_STATE_ERASE_WAIT_2;
			}
			else if (tm.Check(MS2CTM(10)))
			{
				__breakpoint();
				lastError = POLL_TIMEOUT;
				flashState = FLASH_STATE_WAIT;
			}; 

			break;
		};

		case FLASH_STATE_ERASE_WAIT_2:
		{
			byte st = ReadStatusRegister();

			if ((st & RDY_BSY) == 0)
			{
				if (st & EPE)
				{
					__breakpoint();
					lastError = ERROR_ERASE;
					flashState = FLASH_STATE_WAIT;
				}
				else
				{
					lastError = NO_ERR;

					flashState = (flashWritePtr != 0 && flashWriteLen != 0) ? FLASH_STATE_WRITE_PAGE : FLASH_STATE_ERASE_WAIT;
				};
			}
			else if (tm.Check(MS2CTM(1000)))
			{
				__breakpoint();
				lastError = POLL_TIMEOUT;
				flashState = FLASH_STATE_WAIT;
			};

			break;
		};

		//case FLASH_STATE_WRITE_START:
		//{
		//	//ReqDsp06 &req = request->req;

		//	flashWriteAdr = FLASH_START_ADR + request->stAdr;
		//	flashWritePtr = (byte*)request->GetDataPtr();
		//	flashWriteLen = request->len;

		//	u16 block = flashWriteAdr/FLASH_SECTOR_SIZE;

		//	if (lastErasedBlock != block)
		//	{
		//		lastErasedBlock = block;

		//		flashState = FLASH_STATE_ERASE_START;

		//		break;
		//	};
		//};

		case FLASH_STATE_WRITE_PAGE:

			CmdWriteEnable();

			tm.Reset();

			flashState = FLASH_STATE_WRITE_PAGE_2;

			break;

		case FLASH_STATE_WRITE_PAGE_2:
		{ 
			byte st = ReadStatusRegister();

			if ((st & RDY_BSY) == 0 && (st & WEL) != 0)
			{
				lastError = NO_ERR;

				ChipEnable();
				
				buf[0] = SPI_PP;
				buf[1] = flashWriteAdr >> 16;
				buf[2] = flashWriteAdr >> 8;
				buf[3] = flashWriteAdr;

				spi.WriteAsyncDMA(buf, 4, flashWritePtr, flashWriteLen);

				//WritePageAsync(flashWritePtr, flashWriteAdr, flashWriteLen);

				flashState = FLASH_STATE_WRITE_PAGE_3;
			}
			else if (tm.Check(MS2CTM(10)))
			{
				__breakpoint();
				lastError = POLL_TIMEOUT;
				flashState = FLASH_STATE_WAIT;
			}; 

			break;
		};

		case FLASH_STATE_WRITE_PAGE_3:

			if (spi.CheckWriteComplete())
			{
				ChipDisable();

				tm.Reset();

				flashState = FLASH_STATE_WRITE_PAGE_4;
			};

			break;

		case FLASH_STATE_WRITE_PAGE_4:
		{ 
			byte st = ReadStatusRegister();

			if ((st & RDY_BSY) == 0)
			{
				if (st & EPE)
				{
					__breakpoint();
					lastError = ERROR_PROGRAM;
					flashState = FLASH_STATE_WAIT;
				}
				else
				{
					lastError = NO_ERR;

					ChipEnable();
				
					buf[0] = SPI_FAST_READ;
					buf[1] = flashWriteAdr >> 16;
					buf[2] = flashWriteAdr >> 8;
					buf[3] = flashWriteAdr;
					buf[4] = 0;

					spi.WriteSyncDMA(buf, 5);
					spi.ReadAsyncDMA(bufpage, flashWriteLen);

					//ReadAsyncDMA(bufpage, flashWriteAdr, flashWriteLen);

					flashState = FLASH_STATE_VERIFY_PAGE;
				};
			}
			else if (tm.Check(MS2CTM(10)))
			{
				__breakpoint();
				lastError = POLL_TIMEOUT;
				flashState = FLASH_STATE_WAIT;
			}; 

			break;
		};

		case FLASH_STATE_VERIFY_PAGE:

			if (spi.CheckReadComplete())
			{
				ChipDisable();

				bool c = false;

				for (u32 i = 0; i < flashWriteLen; i++)
				{
					if (flashWritePtr[i] != bufpage[i]) { c = true; break; };
				};

				if (c)
				{
					__breakpoint();

					lastError = VERIFY_WRITE;
				}
				else
				{
					lastError = NO_ERR;
				};

				flashWritePtr = 0;
				flashWriteLen = 0;

				flashState = FLASH_STATE_WAIT;
			};

			break;


	};
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void FlashSPI::ADSP_CheckFlash()
{
	while (Busy()) Update();

	u32 *p = (u32*)&bh;

	u32 adr = 0;
	
	flashOK = flashChecked = flashCRCOK = false;

	//at25df021_Read(buf, 0, sizeof(buf));

	while (1)
	{
		Read(FLASH_START_ADR + adr, (byte*)&bh, sizeof(bh));

		u32 x = p[0] ^ p[1] ^ p[2] ^ p[3];
		x ^= x >> 16; 
		x = (x ^ (x >> 8)) & 0xFF; 

		if (((u32)(bh.dBlockCode) >> 24) == 0xAD && x == 0)
		{
			adr += sizeof(bh);

			if ((bh.dBlockCode & BFLAG_FILL) == 0)
			{
				adr += bh.dByteCount;	
			};

			if (bh.dBlockCode & BFLAG_FINAL)
			{
				flashOK = true;

				break;
			};
		}
		else
		{
			break;
		};
	};

	flashLen = adr;

	Read(FLASH_START_ADR + adr, &crc, sizeof(crc));

	if (flashLen > 0) flashCRC = this->GetCRC16(FLASH_START_ADR, flashLen), flashCRCOK = (flashCRC == crc);

	if (!flashCRCOK) flashLen = 0;

	flashChecked = true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // FLASHINT_IMP_H__24_02_2024__16_53
