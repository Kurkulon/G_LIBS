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

#define SPIMODE	(CPOL|CPHA)

static char 		*pFlashDesc =		"Atmel AT25DF021";
static char 		*pDeviceCompany	=	"Atmel Corporation";

static int			gNumSectors = NUM_SECTORS;

//static u32			bufsect[(SECTOR_SIZE+3)/4];
static u16			lastErasedBlock = ~0;

static byte*		flashWritePtr = 0;
static u16			flashWriteLen = 0;
static u32			flashWriteAdr = 0;

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
protected:
	
	enum FlashState 
	{ 
		FLASH_STATE_WAIT = 0, 
		FLASH_STATE_ERASE_START, 
		FLASH_STATE_ERASE_WAIT,
		FLASH_STATE_ERASE_WAIT_2,
		FLASH_STATE_ERASE_CHECK,
		FLASH_STATE_WRITE_START, 
		FLASH_STATE_WRITE_PAGE, 
		FLASH_STATE_WRITE_PAGE_2, 
		FLASH_STATE_WRITE_PAGE_3, 
		FLASH_STATE_WRITE_PAGE_4, 
		FLASH_STATE_VERIFY_PAGE
	};

	FlashState flashState;

	u32 secStartAdr;
	u32 secEndAdr;
	u32 wadr;
	u32 wlen;
	u32 flash_write_error;
	u32 flash_write_ok;
	__packed u32 *wdata;

	S_SPIM	&spi;

	FLWB *flwb;
	Ptr<MB> curFlwb;
	ListPtr<MB> writeFlBuf;

	ERROR_CODE	lastError;

	byte buf[SPI_PAGE_SIZE];

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

	u32 Get_WriteError() { return flash_write_error; }
	u32 Get_WriteOK() { return flash_write_ok; }

	virtual u32		GetSectorAdrLen(u32 sadr, u32 *radr);
	virtual void	Update();
	virtual u32		Read(u32 addr, byte *data, u32 size);
	virtual void	InitFlashWrite() { /*state_write_flash = WRITE_INIT;*/ }
	virtual bool	Busy() { return (flashState != FLASH_STATE_WAIT) || !writeFlBuf.Empty(); }
	virtual bool	RequestWrite(Ptr<MB> &b);

	FlashSPI(S_SPIM &sp) : FlashMem(FLASH_PAGE_SIZE, FLASH_START_ADR), flashState(FLASH_STATE_WAIT), lastError(NO_ERR), secStartAdr(~0), secEndAdr(~0), flwb(0), wadr(0), wlen(0), 
					flash_write_error(0), flash_write_ok(0), wdata(0), spi(sp) {}
};

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

u32 FlashSPI::Read(u32 addr, byte *data, u32 size)
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
	switch(state_write_flash)
	{
		case WRITE_WAIT:

			curFlwb = writeFlBuf.Get();

			if (curFlwb.Valid())
			{
				flwb = (FLWB*)curFlwb->GetDataPtr();
				wadr = flwb->adr;
				wlen = flwb->dataLen;
				wdata = (__packed u32*)(flwb->data + flwb->dataOffset);

				state_write_flash = WRITE_START;
			};

			break;

		case WRITE_START:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	
			
			if (wadr >= secStartAdr &&  wadr < secEndAdr)
			{
				state_write_flash = WRITE_PAGE;
			}
			else
			{
				u32 len = GetSectorAdrLen(wadr, &secStartAdr);

				if (len != 0)
				{
					secEndAdr = secStartAdr + len - 1;

					if (!REQ_EraseSector(secStartAdr)) len = 0;
				};

				state_write_flash = (len != 0) ? WRITE_ERASE_SECTOR : WRITE_ERROR;
			};

			break;
	
		case WRITE_ERASE_SECTOR:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			if (IsFlashReady())
			{
				state_write_flash = (IsFlashReqOK()) ? WRITE_PAGE : WRITE_ERROR;
			};

			break;

		case WRITE_PAGE:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			state_write_flash = (REQ_WritePage(wadr, wdata, wlen)) ? WRITE_PAGE_0 : WRITE_ERROR;

			break;

		case WRITE_PAGE_0:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			if (IsFlashReady())
			{
				if (IsFlashReqOK())
				{
					wadr += ISP_PAGESIZE;
					wdata += PAGEDWORDS;

					if (wlen >= ISP_PAGESIZE)
					{
						wlen -= ISP_PAGESIZE;
					}
					else
					{
						wlen = 0;	
					};

					state_write_flash = (wlen > 0) ? WRITE_START : WRITE_OK;
				}
				else
				{
					state_write_flash = WRITE_ERROR;
				};
			};

			break;

		case WRITE_OK:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			flash_write_ok++;

			state_write_flash = WRITE_FINISH;

			break;

		case WRITE_ERROR:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			flash_write_error++;

			state_write_flash = WRITE_FINISH;

			break;
		
		case WRITE_FINISH:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			curFlwb.Free();

			flwb = 0;

			state_write_flash = WRITE_WAIT;

			break;

		case WRITE_INIT:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

			curFlwb.Free();

			flash_write_error = 0;
			flash_write_ok = 0;

			secStartAdr = ~0;
			secEndAdr = ~0;

			flwb = 0;

			state_write_flash = WRITE_WAIT;

			break;

	}; // switch(state_write_flash)
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // FLASHINT_IMP_H__24_02_2024__16_53
