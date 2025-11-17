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
//#ifdef FLASH_AT25DF021
//#elif defined(FLASH_IS25LP080D)
//#else
////#error !!! Must defined flash type !!!
//#define FLASH_AT25DF021
//#endif

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

#if !defined(FLASHSPI_REQUESTUPDATE) && !defined(FLASHSPI_WRITESYNC)
#define FLASHSPI_REQUESTUPDATE
#endif

#if defined(ADSP_CRC_PROTECTION) && !defined(ADSP_CHECKFLASH)
#define ADSP_CHECKFLASH
#endif


#ifndef PAGEDWORDS
#define PAGEDWORDS		(FLASH_PAGE_SIZE/4)
#endif

#ifndef FLASH_START
#define FLASH_START		FLASH_START_ADR
#endif

#ifndef ISP_PAGESIZE
#define ISP_PAGESIZE	FLASH_PAGE_SIZE
#endif

#ifndef NS2CCLK
#define NS2CCLK(x)	NS2CLK(x)
#endif

#ifdef __ADSPBF59x__
#define SPIMODE	(CPOL|CPHA)
#elif defined(__ADSPBF70x__) || defined(__ADSPBF60x__)
#define SPIMODE	(SPI_CPOL|SPI_CPHA)
#endif

#ifndef Pin_VerifyPageError_Set
#define Pin_VerifyPageError_Set()		{}
#endif

#ifndef Pin_VerifyPageError_Clr
#define Pin_VerifyPageError_Clr()		{}
#endif

//static char 		*pFlashDesc =		"Atmel AT25DF021";
//static char 		*pDeviceCompany	=	"Atmel Corporation";


//static u32			bufsect[(SECTOR_SIZE+3)/4];


/* flash commands */
#define CMD_WREN            (0x06)  //	Set Write Enable Latch
#define CMD_WRDI            (0x04)  //	Reset Write Enable Latch
#define CMD_RDJDID          (0x9F)  //	Read Identification
#define CMD_RDSR            (0x05)  //	Read Status Register
#define CMD_WRSR            (0x01)  //	Write Status Register
#define CMD_READ            (0x03)  //	Read data from memory
#define CMD_FAST_READ       (0x0B)  //	Read data from memory
#define CMD_PP              (0x02)  //	Program Data into memory
#define CMD_SE              (0x20)  //	Erase one sector in memory
#define CMD_BE              (0xC7)  //	Erase all memory

#define SR_RDY_BSY 			(0x1)	//	Check the write in progress bit of the SPI status register
#define SR_WEL				(0x2)	//	Check the write enable bit of the SPI status register

#define CMD_BER32K			(0x52)	//	Block Erase (32 Kbytes)
#define CMD_BER64K			(0xD8)	//	Block Erase (64 Kbytes)
#define CMD_EDPD			(0xB9)	//	ENTER DEEP POWER DOWN (DP, B9h)
#define CMD_RDRP			(0xAB)	//	RELEASE DEEP POWER DOWN (RDPD, ABh)

//#ifdef FLASH_AT25DF021

	#define CMD_UPS			(0x39)  //	Unprotect Sector 
	#define CMD_PRS			(0x36)  //	Protect Sector 
	#define CMD_RSPR		(0x3C)  //	Read Sector Protection Registers 
	#define CMD_POSR		(0x9B)  //	Program OTP Security Register 
	#define CMD_ROSR		(0x77)  //	Read OTP Security Register

	#define SR_SPRL			(0x80)	//	Sector Protection Registers Locked
	#define SR_SWP			(0xC)	//	Software Protection Status
	#define SR_WPP			(0x10)	//	Write Protect (WP) Pin Status
	#define SR_EPE			(0x20)	//	Erase/Program Error

//#endif

//#ifdef FLASH_IS25LP080D

	#define SR_SPWD			(0x80)	//	Status Register Write Disable
	#define SR_QE			(0x40)	//	indicates the Quad output function enable
	#define SR_BP3			(0x20)	//	Block Protection Bit3
	#define SR_BP2			(0x10)	//	Block Protection Bit2
	#define SR_BP1			(0x08)	//	Block Protection Bit1
	#define SR_BP0			(0x04)	//	Block Protection Bit0

	#define CMD_NOP			(0x00)	//	NO OPERATION (NOP, 00h)
	#define CMD_FRDTR		(0x0D)	//	FAST READ DTR MODE OPERATION IN SPI MODE (FRDTR, 0Dh)
	#define CMD_RDABR		(0x14)	//	AUTOBOOT REGISTER READ OPERATION (RDABR, 14h)
	#define CMD_WRABR		(0x15)	//	AUTOBOOT REGISTER WRITE OPERATION (WRABR, 15h)
	#define CMD_SER			(0x20)	//	SECTOR ERASE OPERATION (SER, D7h/20h)
	#define CMD_SECLOCK		(0x24)	//	SECTOR LOCK OPERATION (SECLOCK, 24h)
	#define CMD_SECUNLOCK	(0x26)	//	SECTOR UNLOCK OPERATION (SECUNLOCK, 26h)
	#define CMD_PPQ			(0x32)	//	QUAD INPUT PAGE PROGRAM OPERATION (PPQ, 32h/38h)
	#define CMD_QPIEN		(0x35)  //	ENTER QUAD PERIPHERAL INTERFACE (QPI) MODE OPERATION 
	#define CMD_FRDO		(0x3B)	//	FAST READ DUAL OUTPUT OPERATION (FRDO, 3Bh)
	#define CMD_WRFR		(0x42)	//	WRITE FUNCTION REGISTER OPERATION (WRFR, 42h)
	#define CMD_RDFR		(0x48)	//	READ FUNCTION REGISTER OPERATION (RDFR, 48h)
	#define CMD_RDUID		(0x4B)	//	READ UNIQUE ID NUMBER (RDUID, 4Bh)
	#define CMD_RDSFDP		(0x5A)	//	READ SFDP OPERATION (RDSFDP, 5Ah)
	#define CMD_RRDPOP		(0x61)	//	READ READ PARAMETERS OPERATION (RDRP, 61 h)
	#define CMD_IRP			(0x62)	//	INFORMATION ROW PROGRAM OPERATION (IRP, 62h)
	#define CMD_IRER		(0x64)	//	INFORMATION ROW ERASE OPERATION (IRER, 64h)
	#define CMD_SRPNV		(0x65)	//	SET READ PARAMETERS OPERATION (SRPNV: 65h, SRPV: C0h/63h)
	#define CMD_RSTEN		(0x66)	//	RESET-ENABLE (RSTEN, 66h) 
	#define CMD_IRRD		(0x68)	//	INFORMATION ROW READ OPERATION (IRRD, 68h)
	#define CMD_FRQO		(0x6B)	//	FAST READ QUAD OUTPUT OPERATION (FRQO, 6Bh)
	#define CMD_PERSUS		(0x75)	//	SUSPEND DURING SECTOR-ERASE OR BLOCK-ERASE (PERSUS 75h/B0h)
	#define CMD_PERRSM		(0x7A)	//	PROGRAM/ERASE RESUME (PERRSM 7Ah/30h)
	#define CMD_RDERP		(0x81)	//	READ EXTENDED READ PARAMETERS OPERATION (RDERP, 81 h)
	#define CMD_CLERP		(0x82)	//	CLEAR EXTENDED READ PARAMETERS OPERATION (CLERP, 82h)
	#define CMD_SERPNV		(0x85)	//	SET EXTENDED READ PARAMETERS OPERATION (SERPNV: 85h, SERPV: 83h)
	#define CMD_RDMDID		(0x90)	//	READ DEVICE MANUFACTURER AND DEVICE ID OPERATION (RDMDID, 90h)
	#define CMD_RST			(0x99)	//	RESET (RST, 99h)
	#define CMD_RDJDID		(0x9F)	//	READ PRODUCT IDENTIFICATION BY JEDEC ID OPERATION (RDJDID, 9Fh; RDJDIDQ, AFh)
	#define CMD_RDID		(0xAB)	//	READ PRODUCT IDENTIFICATION (RDID, ABh)
	#define CMD_RDJDIDQ		(0xAF)	//	READ PRODUCT IDENTIFICATION BY JEDEC ID OPERATION (RDJDID, 9Fh; RDJDIDQ, AFh)
	#define CMD_FRDIO		(0xBB)	//	FAST READ DUAL I/O OPERATION (FRDIO, BBh)
	#define CMD_PRSUS		(0xB0)	//	SUSPEND DURING PAGE PROGRAMMING (PERSUS 75h/B0h)
	#define CMD_SRPV		(0xC0)	//	SET READ PARAMETERS OPERATION (SRPNV: 65h, SRPV: C0h/63h)
	#define CMD_CER			(0xC7)	//	CHIP ERASE OPERATION (CER, C7h/60h)
	#define CMD_FRQIO		(0xEB)	//	FAST READ QUAD I/O OPERATION (FRQIO, EBh)
	#define CMD_FRDDTR		(0xBD)	//	FAST READ DUAL IO DTR MODE OPERATION (FRDDTR, BDh)
	#define CMD_FRQDTR		(0xED)	//	FAST READ QUAD IO DTR MODE OPERATION IN SPI MODE (FRQDTR, EDh)
	#define CMD_QPIDI		(0xF5)  //	Exit Quad Peripheral Interface (QPI) Mode

	#define ERP_WIP			(0x01)	//	Write In Progress Bit: Has exactly same function as the bit0 (WIP) of Status Register “0”: Ready, “1”: Busy
	#define ERP_PROT_E		(0x02)	//	Protection Error Bit: "0" indicates no error, "1" indicates protection error in an Erase or a Program operation
	#define ERP_P_ERR		(0x04)	//	Program Error Bit: "0" indicates no error, "1" indicates an Program operation failure or protection error
	#define ERP_E_ERR		(0x08)	//	Erase Error Bit: "0" indicates no error, "1" indicates a Erase operation failure or protection error
	#define ERP_ODS0		(0x20)
	#define ERP_ODS1		(0x40)
	#define ERP_ODS2		(0x80)
	#define ERP_ODS12		(ERP_ODS0)						//	Output Driver Strength: 12.5%
	#define ERP_ODS25		(ERP_ODS1)						//	Output Driver Strength: 25%
	#define ERP_ODS37		(ERP_ODS0|ERP_ODS1)				//	Output Driver Strength: 37.5%
	#define ERP_ODS75		(ERP_ODS0|ERP_ODS2)				//	Output Driver Strength: 75%
	#define ERP_ODS100		(ERP_ODS1|ERP_ODS2)				//	Output Driver Strength: 100%
	#define ERP_ODS50		(ERP_ODS0|ERP_ODS1|ERP_ODS2)	//	Output Driver Strength: 50%

//#endif


#define SPI_PAGE_SIZE		(256)
//#define SPI_SECTOR_SIZE		(0x1000)

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

#if !defined(_ADI_COMPILER) && defined(ADSP_CHECKFLASH)

#define BFLAG_FINAL         0x00008000   /* final block in stream */
#define BFLAG_FIRST         0x00004000   /* first block in stream */
#define BFLAG_INDIRECT      0x00002000   /* load data via intermediate buffer */
#define BFLAG_IGNORE        0x00001000   /* ignore block payload */
#define BFLAG_INIT          0x00000800   /* call initcode routine */
#define BFLAG_CALLBACK      0x00000400   /* call callback routine */
#define BFLAG_QUICKBOOT     0x00000200   /* boot block only when BFLAG_WAKEUP=0 */
#define BFLAG_FILL          0x00000100   /* fill memory with 32-bit argument value */
#define BFLAG_AUX           0x00000020   /* load auxiliary header -- reserved */
#define BFLAG_SAVE          0x00000010   /* save block on power down -- reserved */

#define BFLG_FINAL    		BFLAG_FINAL
#define BFLG_FIRST			BFLAG_FIRST
#define BFLG_INDIRECT 		BFLAG_INDIRECT
#define BFLG_IGNORE   		BFLAG_IGNORE
#define BFLG_INIT     		BFLAG_INIT
#define BFLG_CALLBACK 		BFLAG_CALLBACK
#define BFLG_QUICKBOOT		BFLAG_QUICKBOOT
#define BFLG_FILL     		BFLAG_FILL
#define BFLG_AUX      		BFLAG_AUX
#define BFLG_SAVE     		BFLAG_SAVE

struct ADI_BOOT_HEADER
{
    u32   dBlockCode;
    void* pTargetAddress;
    u32   dByteCount;
    u32   dArgument;
    
}; 

#endif

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
	
#ifdef FLASHSPI_REQUESTUPDATE

	enum FlashState 
	{ 
		FLASH_STATE_WAIT = 0, 
		FLASH_STATE_WRITE_START, 
		FLASH_STATE_ERASE_START, 
		FLASH_STATE_ERASE_WAIT,
		FLASH_STATE_ERASE_WAIT_2,
		FLASH_STATE_ERASE_CHECK,
		FLASH_STATE_WRITE_PAGE, 
		FLASH_STATE_WRITE_PAGE_2, 
		FLASH_STATE_WRITE_PAGE_3, 
		FLASH_STATE_WRITE_PAGE_4, 
		FLASH_STATE_WRITE_PAGE_CHECK, 
		FLASH_STATE_VERIFY_PAGE
	};

	FlashState flashState;

	CTM32 tm;
	byte*		flashWritePtr;
	u16			flashWriteLen;
	u32			flashWriteAdr;

	u32 flash_write_error;
	u32 flash_write_ok;

	FLWB *flwb;
	Ptr<MB> curFlwb;
	ListPtr<MB> writeFlBuf;

#endif

	u32	gNumSectors;

	S_SPIM	&spi;

	ERROR_CODE	lastError;

	u16	lastErasedBlock;

	#ifdef ADSP_CHECKFLASH

		HDRTYPE bh;
	
		#ifdef ADSP_CRC_PROTECTION
			u16 adsp_crc;
		#endif
	#endif

	enum FlashTypeID 
	{ 
		FLID_UNKNOWN = 0, 
		FLID_AT25DF021, 
		FLID_IS25LP080D
	};

	FlashTypeID flashID;

	byte cmd_ProgramEraseStatus;
	byte mask_ProgramEraseStatus;
	byte cmd_EnterQPI;
	byte cmd_ExitQPI;

	byte _manCode;
	u16 _devCode;

	char *_manStr;
	char *_devStr;

	bool _flashTypeModeDetected;
	bool _QPI_mode;

	byte _csnum;
	byte buf[10];

	byte bufpage[SPI_PAGE_SIZE];

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	#ifdef __ADSPBF59x__

		inline void ChipEnable() {	spi.ChipSelect(0, SPIMODE, BAUD_RATE_DIVISOR); }

		inline void SetModeStandart()	{ }
		inline void SetModeDual()		{ }
		inline void SetModeQuad()		{ }

	#elif defined(__ADSPBF70x__) || defined(__ADSPBF60x__)

		u32 _spimode;

		inline void ChipEnable()		{ spi.ChipSelect(0, _spimode,												BAUD_RATE_DIVISOR); }
		inline void ChipEnableDual()	{ spi.ChipSelect(0, (_spimode & ~(SPI_MIO_QUAD|SPI_MIO_DUAL))|SPI_MIO_DUAL,	BAUD_RATE_DIVISOR); }
		inline void ChipEnableQuad()	{ spi.ChipSelect(0, (_spimode & ~(SPI_MIO_QUAD|SPI_MIO_DUAL))|SPI_MIO_QUAD,	BAUD_RATE_DIVISOR); }

		inline void SetModeStandart()	{ _spimode = SPIMODE; }
		inline void SetModeDual()		{ _spimode = (_spimode & ~(SPI_MIO_QUAD|SPI_MIO_DUAL))|SPI_MIO_DUAL; }
		inline void SetModeQuad()		{ _spimode = (_spimode & ~(SPI_MIO_QUAD|SPI_MIO_DUAL))|SPI_MIO_QUAD; }

	#else

		inline void ChipEnable() {	spi.ChipSelect(_csnum); }

		inline void SetModeStandart()	{ }
		inline void SetModeDual()		{ }
		inline void SetModeQuad()		{ }

	#endif

	inline void ChipDisable() { spi.ChipDisable(); }

	void Delay(u32 us);

	ERROR_CODE GetLastError() {	return lastError; }

	void __SendSingleCommand(byte iCommand);
	void __Send3bytesCommand(byte iCommand, u32 adr);

	void CmdWriteEnable()				{ __SendSingleCommand(CMD_WREN);		}
	void CmdWriteDisable()				{ __SendSingleCommand(CMD_WRDI);		}
	void CmdEraseSector(u32 sec)		{ __Send3bytesCommand(CMD_SE, sec);		}
	void CmdProtectSector(u32 sec)		{ __Send3bytesCommand(CMD_PRS, sec);	}
	void CmdUnprotectSector(u32 sec)	{ __Send3bytesCommand(CMD_UPS, sec);	}
	void CmdWriteStatusReg(byte stat);
	void CmdEnterQPI()	{ __SendSingleCommand(cmd_EnterQPI);	_QPI_mode = cmd_EnterQPI != 0; }
	void CmdExitQPI()	{ __SendSingleCommand(cmd_ExitQPI);		_QPI_mode = false; }

	u32 GetNumSectors() { return gNumSectors; }
	u32 GetSectorSize() { return FLASH_SECTOR_SIZE; }
	
	void GlobalUnProtect() { CmdWriteEnable(); CmdWriteStatusReg(0); }

	u16 GetCRC16(u32 stAdr, u16 count);
	
	ERROR_CODE WritePage(const void *data, u32 stAdr, u16 count);
	ERROR_CODE VerifyPage(const byte *data, u32 stAdr, u16 count);
	ERROR_CODE Wait_For_WEL(void);
	ERROR_CODE Wait_For_Status( char Statusbit );

	byte ReadStatusRegister();
	byte ReadProgramEraseStatus();
	void ReadDeviceID();
	bool GetFlashType();
	void DetectFlashTypeMode();

#ifdef FLASHSPI_AFP

  public:

	ERROR_CODE EraseFlash();
	ERROR_CODE GetCodes(int *pnManCode, int *pnDevCode);
	ERROR_CODE GetStr(char **manStr, char **devStr);
	ERROR_CODE GetSectorNumber( unsigned long ulAddr, int *pnSector );
	ERROR_CODE ResetFlash() { return NO_ERR; }
	ERROR_CODE GetSectorStartEnd( unsigned long *ulStartOff, unsigned long *ulEndOff, int nSector );

#else

  protected:

	ERROR_CODE GetSectorStartEnd( unsigned long *ulStartOff, unsigned long *ulEndOff, int nSector );

#endif

	ERROR_CODE EraseBlock(int nBlock);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

public:

	#ifdef ADSP_CHECKFLASH

		u32 	flashLen;
		bool 	flashOK;
		bool 	flashChecked;

		#ifdef ADSP_CRC_PROTECTION

			bool 	flashCRCOK;
			u16 	flashCRC;
			
			u16		CRC16(u32 len, u32 *rlen);

		#endif
	
	
	#endif

	u32			GetSectorAdrLen(u32 sadr, u32 *radr);
	ERROR_CODE	CmdRead(u32 addr, void *data, u32 size);
	u32			Read(u32 addr, void *data, u32 size);

#ifdef FLASHSPI_REQUESTUPDATE
	void	InitFlashWrite() { /*state_write_flash = WRITE_INIT;*/ }
	bool	Busy() { return (flashState != FLASH_STATE_WAIT) || !writeFlBuf.Empty(); }
	u32 	Get_WriteError() { return flash_write_error; }
	u32 	Get_WriteOK() { return flash_write_ok; }
	void	Update();
	bool	RequestWrite(Ptr<MB> &b);
	void	WaitBusy() 	{ while (Busy()) Update(); }
#else
	void	WaitBusy() 	{ }
#endif

	void	Init();
	
#ifdef FLASHSPI_WRITESYNC
	ERROR_CODE WriteSync(const byte *data, u32 stAdr, u32 count, bool verify);
#endif

#if defined(__ADSPBF70x__) || defined(__ADSPBF60x__)
	FlashSPI(S_SPIM &sp, byte csnum = 0) : FlashMem(FLASH_PAGE_SIZE, FLASH_START_ADR), spi(sp), _csnum(csnum), _spimode(SPIMODE) {}
#else
	FlashSPI(S_SPIM &sp, byte csnum = 0) : FlashMem(FLASH_PAGE_SIZE, FLASH_START_ADR), spi(sp), _csnum(csnum) {}
#endif


#ifdef ADSP_CHECKFLASH

	void ADSP_CheckFlash();

#endif
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void FlashSPI::Init()
{
	spi.Connect(AT25_SPI_BAUD_RATE);

#ifdef FLASHSPI_REQUESTUPDATE
	flashState			= FLASH_STATE_WAIT;
	flwb				= 0;
	flash_write_error	= 0;
	flash_write_ok		= 0;
#endif

#ifdef ADSP_CHECKFLASH
	flashLen			= 0;
	flashOK				= false;
	flashChecked		= false;
#endif

#ifdef ADSP_CRC_PROTECTION
	flashCRC			= 0;
	flashCRCOK			= false;
#endif

	lastErasedBlock 	= ~0;
	lastError			= NO_ERR;

	_QPI_mode = false;

#if defined(__ADSPBF70x__) || defined(__ADSPBF60x__)

	SetModeStandart();

	ReadStatusRegister(); 

	DetectFlashTypeMode();

	ReadStatusRegister(); 

	#ifdef FLASH_QPI_MODE
		CmdEnterQPI();
		SetModeQuad();
	#endif

#else

	gNumSectors = NUM_SECTORS;

	DetectFlashTypeMode();

#endif


#ifdef ADSP_CHECKFLASH

	ADSP_CheckFlash();

#endif

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void FlashSPI::ReadDeviceID()
{
	ChipEnable();

	spi.WriteByteSync(CMD_RDJDID);
	spi.ReadByteStart(3);
	_manCode = spi.ReadByteAsync();
	_devCode = (spi.ReadByteAsync()<<8)|spi.ReadByteAsync();

	ChipDisable();		
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool FlashSPI::GetFlashType()
{
	bool res = false;

	ReadDeviceID();

	flashID = FLID_UNKNOWN;
	gNumSectors = 0;

	_manStr = "Unknow";
	_devStr = "Unknow";

	if (_manCode == 0x1F) // Atmel Corporation
	{
		_manStr = "Atmel Corporation";

		switch (_devCode)
		{
			case 0x4300:	_devStr = "Atmel AT25DF021 2-Mbit"; gNumSectors = 128;
				
				flashID = FLID_AT25DF021;
				cmd_ProgramEraseStatus = CMD_RDSR;
				mask_ProgramEraseStatus = SR_EPE;
				cmd_EnterQPI = 0;
				cmd_ExitQPI = 0;

				GlobalUnProtect();

				if (flashStartAdr >= 0x10000) CmdWriteEnable(), CmdProtectSector(0);

				res = true; 
				break; 
		};
	}
	else if (_manCode == 0x9D) // Integrated Silicon Solution
	{
		_manStr = "Integrated Silicon Solution";

		switch (_devCode)
		{
			case 0x6014:	_devStr = "ISSI IS25LP080D 8-Mbit";	gNumSectors = 512; goto _is25lp080d; 
			case 0x7014:	_devStr = "ISSI IS25WP080D 8-Mbit";	gNumSectors = 512; goto _is25lp080d;  
			case 0x7013:	_devStr = "ISSI IS25WP040D 4-Mbit";	gNumSectors = 256; goto _is25lp080d;  
			case 0x7012:	_devStr = "ISSI IS25WP020D 2-Mbit";	gNumSectors = 128; goto _is25lp080d;  
		
			_is25lp080d:

				flashID = FLID_IS25LP080D;
				cmd_ProgramEraseStatus = CMD_RDERP;
				mask_ProgramEraseStatus = ERP_PROT_E|ERP_P_ERR|ERP_E_ERR;
				cmd_EnterQPI = 0x35; 
				cmd_ExitQPI = 0xF5;

				byte status = ReadStatusRegister();

				byte status_new = (status & ~(SR_BP0|SR_BP1|SR_BP2|SR_BP3)) | SR_QE;

				if (flashStartAdr >= 0x10000) status_new |= SR_BP1|SR_BP2|SR_BP3;

				if (status != status_new) CmdWriteEnable(), CmdWriteStatusReg(status_new), Wait_For_Status(SR_RDY_BSY);

				res = true;
		};

		//SendSingleCommand(0x66);
		//SendSingleCommand(0x99);

	};

	return res;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void FlashSPI::DetectFlashTypeMode()
{
	SetModeStandart();

	bool res = GetFlashType();

	if (res) goto exit;		

	SetModeDual();

	res = GetFlashType();

	if (res) goto exit;		

	SetModeQuad();

	res = GetFlashType();

exit:

	_flashTypeModeDetected = res;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if defined(ADSP_CHECKFLASH) && defined(ADSP_CRC_PROTECTION) 

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
		crc = this->GetCRC16(0, len);
	};

	if (rlen != 0) *rlen = len;

	return crc;
}

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef FLASHSPI_REQUESTUPDATE

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

#endif 

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

void FlashSPI::__SendSingleCommand(byte iCommand )
{
	ChipEnable();

	spi.WriteByteSync(iCommand);

	ChipDisable();

	Delay(1);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void FlashSPI::__Send3bytesCommand(byte iCommand, u32 adr)
{
	ChipEnable();

	buf[0] = iCommand;
	buf[1] = adr >> 16;
	buf[2] = adr >> 8;
	buf[3] = adr;

	spi.WriteSyncDMA(buf, 4);

	ChipDisable();

	Delay(1);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//void FlashSPI::CmdEraseSector(u32 sec)
//{
//	ChipEnable();
//
//	buf[0] = CMD_SE;
//	buf[1] = sec >> 16;
//	buf[2] = sec >> 8;
//	buf[3] = sec;
//
//	spi.WriteSyncDMA(buf, 4);
//
//	ChipDisable();
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//void FlashSPI::CmdProtectSector(u32 sec)
//{
//	ChipEnable();
//
//	buf[0] = CMD_PRS;
//	buf[1] = sec >> 16;
//	buf[2] = sec >> 8;
//	buf[3] = sec;
//
//	spi.WriteSyncDMA(buf, 4);
//
//	ChipDisable();
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//void FlashSPI::CmdUnprotectSector(u32 sec)
//{
//	ChipEnable();
//
//	buf[0] = CMD_UPS;
//	buf[1] = sec >> 16;
//	buf[2] = sec >> 8;
//	buf[3] = sec;
//
//	spi.WriteSyncDMA(buf, 4);
//
//	ChipDisable();
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void FlashSPI::CmdWriteStatusReg(byte stat)
{
	ChipEnable();

	buf[0] = CMD_WRSR;
	buf[1] = stat;

	spi.WriteSyncDMA(buf, 2);

	ChipDisable();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE FlashSPI::CmdRead(u32 addr, void *data, u32 size)
{
	addr += flashStartAdr;

    buf[0] = CMD_FAST_READ;
    buf[1] = addr >> 16;
    buf[2] = addr >> 8;
    buf[3] = addr;
    buf[4] = 0;

	ChipEnable();

	spi.WriteSyncDMA(buf, (_QPI_mode)?7:5);

	spi.ReadSyncDMA(data, size);

	ChipDisable();

	return NO_ERR;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32 FlashSPI::Read(u32 addr, void *data, u32 size)
{
	u32 flashend = FLASH_SECTOR_SIZE*gNumSectors;

#ifdef ADSP_CHECKFLASH
	if (flashChecked) flashend = flashLen;
#endif

	u32 a = addr+flashStartAdr;

	if (a >= flashend)
	{
		return 0;
	};

	if ((a + size) >= flashend)
	{
		size = flashend - a;	
	};

	CmdRead(addr, data, size);

	return size;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma optimize_for_speed

u16 FlashSPI::GetCRC16(u32 stAdr, u16 count)
{
	DataCRC crc;

	crc.w = 0xFFFF;

	u16 t = 0;

	stAdr += flashStartAdr;

    buf[0] = CMD_FAST_READ;
    buf[1] = stAdr >> 16;
    buf[2] = stAdr >> 8;
    buf[3] = stAdr;
    buf[4] = 0;

	HW::ResetWDT();

	ChipEnable();

	spi.WriteSyncDMA(buf, (_QPI_mode)?7:5);

	spi.ReadByteStart(count);

	for ( ; count > 0; count--)
	{
		t = spi.ReadByteAsync();

		crc.w = tableCRC[crc.b[0] ^ t] ^ crc.b[1];
		
		if ((count&0xFF) == 0) HW::ResetWDT();
	};
	
	ChipDisable();

	return crc.w;
}

#pragma optimize_as_cmd_line

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE FlashSPI::WritePage(const void *data, u32 stAdr, u16 count )
{
	WaitBusy();

    lastError = NO_ERR;

	u16 block = stAdr/FLASH_SECTOR_SIZE;

	if ((stAdr & 0xFF) != 0 || count > 256 || count == 0) { lastError = INVALID_BLOCK; goto exit; };

	if (lastErasedBlock != block)
	{
		lastError = EraseBlock(block);

		if (lastError != NO_ERR) goto exit;

		lastErasedBlock = block;
	};

	CmdWriteEnable();

	lastError = Wait_For_WEL();

    if(lastError != NO_ERR) goto exit;

	ChipEnable();

	buf[0] = CMD_PP;		//spi.WriteReadByte(SPI_PP);
	buf[1] = stAdr >> 16;	//spi.WriteReadByte(stAdr >> 16);
	buf[2] = stAdr >> 8;	//spi.WriteReadByte(stAdr >> 8);
	buf[3] = stAdr;			//spi.WriteReadByte(stAdr);

	spi.WriteSyncDMA(buf, 4, data, count);

	ChipDisable();

	lastError = Wait_For_Status(SR_RDY_BSY);

	if(lastError != NO_ERR) goto exit;

	if (ReadProgramEraseStatus() & mask_ProgramEraseStatus) lastError = ERROR_PROGRAM;

exit:

	return lastError;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE FlashSPI::VerifyPage(const byte *data, u32 stAdr, u16 count )
{
    lastError = NO_ERR;

	if ((stAdr & 0xFF) != 0 || count > 256 || count == 0)
	{
		return lastError = INVALID_BLOCK;
	};

	ChipEnable();

    buf[0] = CMD_FAST_READ;
    buf[1] = stAdr >> 16;
    buf[2] = stAdr >> 8;
    buf[3] = stAdr;
    buf[4] = 0;

	spi.WriteSyncDMA(buf, 5);
	spi.ReadSyncDMA(bufpage, sizeof(bufpage));

	byte *d = bufpage;

	for ( ; count > 0; count--)
	{
		if (*(d++) != *(data++))
		{
			lastError = VERIFY_WRITE;
			break;
		};
	};

	ChipDisable();

	return lastError;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE FlashSPI::EraseBlock(int nBlock)
{
	 			lastError   = NO_ERR;		//tells us if there was an error erasing flash
 	unsigned long ulSectStart = 0x0;		//stores the sector start offset
 	unsigned long ulSectEnd   = 0x0;		//stores the sector end offset(however we do not use it here)

	// Get the sector start offset
	// we get the end offset too however we do not actually use it for Erase sector
	GetSectorStartEnd( &ulSectStart, &ulSectEnd, nBlock );

	//CmdWriteEnable();

	//CmdUnprotectSector(ulSectStart);

	//GlobalUnProtect();
	//GlobalUnProtect();

	CmdWriteEnable();

	lastError = Wait_For_WEL();

	if(lastError != NO_ERR) goto exit;

	CmdEraseSector(ulSectStart);

	lastError = Wait_For_Status(SR_RDY_BSY);

	if(lastError != NO_ERR) goto exit;

	if (ReadProgramEraseStatus() & mask_ProgramEraseStatus) lastError = ERROR_ERASE;

exit:

	return lastError;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef FLASHSPI_AFP

ERROR_CODE FlashSPI::EraseFlash()
{
	GlobalUnProtect();
//	GlobalUnProtect();

	CmdWriteEnable();

	__SendSingleCommand(CMD_BE);

	lastError = Wait_For_Status(SR_RDY_BSY);

	if(lastError != NO_ERR) goto exit;

	if (ReadProgramEraseStatus() & mask_ProgramEraseStatus) lastError = ERROR_ERASE;

exit:

	return lastError;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE FlashSPI::GetCodes(int *pnManCode, int *pnDevCode)
{
	*pnManCode = _manCode;
	*pnDevCode = _devCode;

	return NO_ERR;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE FlashSPI::GetStr(char **manStr, char **devStr)
{
	*manStr = _manStr;
	*devStr = _devStr;

	return NO_ERR;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE FlashSPI::GetSectorNumber( unsigned long ulAddr, int *pnSector )
{
	int nSector = 0;
	int i;
	int error_code = 1;
	unsigned long ulMask;					//offset mask
	unsigned long ulOffset;					//offset
	unsigned long ulStartOff;
	unsigned long ulEndOff;

	ulMask      	  = 0x7ffffff;
	ulOffset		  = ulAddr & ulMask;

	for(i = 0; i < gNumSectors; i++)
	{
		GetSectorStartEnd(&ulStartOff, &ulEndOff, i);
		if ( (ulOffset >= ulStartOff)
			&& (ulOffset <= ulEndOff) )
		{
			error_code = 0;
			nSector = i;
			break;
		}
	}

	// if it is a valid sector, set it
	if (error_code == 0)
		*pnSector = nSector;
	// else it is an invalid sector
	else
		return INVALID_SECTOR;

	// ok
	return NO_ERR;
}

#endif 

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
		return lastError = INVALID_SECTOR;
	};

	// ok
	return lastError = NO_ERR;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//#ifdef __ADSPBF70x__

byte FlashSPI::ReadStatusRegister(void)
{
	ChipEnable();

	spi.WriteByteSync(CMD_RDSR);

	byte usStatus = spi.ReadByteSync(1);

	ChipDisable();		

	return usStatus;
}

//#else
//
//byte FlashSPI::ReadStatusRegister(void)
//{
//	ChipEnable();
//
//	spi.WriteReadByte(CMD_RDSR);
//
//	byte usStatus = spi.WriteReadByte(0);
//
//	ChipDisable();		
//
//	return usStatus;
//}
//
//#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

byte FlashSPI::ReadProgramEraseStatus(void)
{
	ChipEnable();

	spi.WriteByteSync(cmd_ProgramEraseStatus);

	byte usStatus = spi.ReadByteSync(1);

	ChipDisable();		

	return usStatus;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE FlashSPI::Wait_For_WEL(void)
{
	volatile int n, i;
	char status_register = 0;
	
	lastError = NO_ERR;	// tells us if there was an error erasing flash

	for(i = 0; i < 35; i++)
	{
		status_register = ReadStatusRegister();

		if( (status_register & SR_WEL) )
		{
			lastError = NO_ERR;	// tells us if there was an error erasing flash
			break;
		};

		Delay(DELAY);

		lastError = POLL_TIMEOUT;	// Time out error

		HW::ResetWDT();
	};

	return lastError;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE FlashSPI::Wait_For_Status( char Statusbit )
{
	volatile int n, i;
	char status_register = 0xFF;
	
	lastError = NO_ERR;	// tells us if there was an error erasing flash

	for(i = 0; i < TIMEOUT; i++)
	{
		status_register = ReadStatusRegister();
		if( !(status_register & Statusbit) )
		{
			lastError = NO_ERR;	// tells us if there was an error erasing flash
			break;
		}

		Delay(DELAY);

		lastError = POLL_TIMEOUT;	// Time out error

		HW::ResetWDT();
	};

	return lastError;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef FLASHSPI_WRITESYNC

ERROR_CODE FlashSPI::WriteSync(const byte *data, u32 stAdr, u32 count, bool verify)
{
    lastError = NO_ERR;

//	u32 c;

	while (count > 0)
	{
		u16 c = (count >= 256) ? 256 : count;

		count -= c;

		lastError = WritePage((void*)data, stAdr, c);

		if (lastError != NO_ERR) break;

		if (verify)
		{
			lastError = VerifyPage(data, stAdr, c);
			if (lastError != NO_ERR) break;
		};

		data += c;
		stAdr += c;

		HW::ResetWDT();
    };

    return lastError;
}

#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef FLASHSPI_REQUESTUPDATE

void FlashSPI::Update()
{
	switch (flashState)
	{
		case FLASH_STATE_WAIT:

			curFlwb = writeFlBuf.Get();

			if (curFlwb.Valid())
			{
				flwb = (FLWB*)curFlwb->GetDataPtr();

				flashWriteAdr = flwb->adr + flashStartAdr; 
				flashWritePtr = flwb->data + flwb->dataOffset;
				flashWriteLen = flwb->dataLen;

				flashState = FLASH_STATE_WRITE_START;

#ifdef FLASHSPI_EXTWDT_TIMEOUT

			}
			else if (tm.Check(FLASHSPI_EXTWDT_TIMEOUT))
			{
				ReadStatusRegister();

#endif
			};

			break;

		case FLASH_STATE_WRITE_START:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		{
			u16 block = flashWriteAdr/FLASH_SECTOR_SIZE;

			if (lastErasedBlock != block)
			{
				lastErasedBlock = block;

				flashState = FLASH_STATE_ERASE_START;
			}
			else
			{
				flashState = FLASH_STATE_WRITE_PAGE;
	
				break;
			};
		};

		case FLASH_STATE_ERASE_START:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			//GlobalUnProtect();
			//GlobalUnProtect();

			//HW::PIOC->SET(PC10);

			CmdWriteEnable();

			tm.Reset();

			flashState = FLASH_STATE_ERASE_WAIT;

			break;

		case FLASH_STATE_ERASE_WAIT:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		{ 
			byte st = ReadStatusRegister();

			if ((st & SR_RDY_BSY) == 0 && (st & SR_WEL) != 0)
			{
				lastError = NO_ERR;

				CmdEraseSector(lastErasedBlock*FLASH_SECTOR_SIZE);

				tm.Reset();

				flashState = FLASH_STATE_ERASE_WAIT_2;
			}
			else if (tm.Check(MS2CTM(10)))
			{
				DEBUG_ASSERT(0);
				lastError = POLL_TIMEOUT;
				flashState = FLASH_STATE_WAIT;
			}; 

			break;
		};

		case FLASH_STATE_ERASE_WAIT_2:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		{
			byte st = ReadStatusRegister();

			if ((st & SR_RDY_BSY) == 0)
			{
					flashState = FLASH_STATE_ERASE_CHECK;
			}
			else if (tm.Check(MS2CTM(1000)))
			{
				DEBUG_ASSERT(0);
				lastError = POLL_TIMEOUT;
				flashState = FLASH_STATE_WAIT;
			};

			break;
		};

		case FLASH_STATE_ERASE_CHECK:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		{
			byte st = ReadProgramEraseStatus();

			if (st & mask_ProgramEraseStatus)
			{
				DEBUG_ASSERT(0);

				lastError = ERROR_ERASE;
				flashState = FLASH_STATE_WAIT;
			}
			else
			{
				lastError = NO_ERR;

				flashState = (flashWritePtr != 0 && flashWriteLen != 0) ? FLASH_STATE_WRITE_PAGE : FLASH_STATE_ERASE_WAIT;

				//HW::PIOC->CLR(PC10);
			};

			break;
		};

		case FLASH_STATE_WRITE_PAGE:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			//HW::PIOC->SET(PC9);

			CmdWriteEnable();

			tm.Reset();

			flashState = FLASH_STATE_WRITE_PAGE_2;

			break;

		case FLASH_STATE_WRITE_PAGE_2:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		{ 
			byte st = ReadStatusRegister();

			if ((st & SR_RDY_BSY) == 0 && (st & SR_WEL) != 0)
			{
				lastError = NO_ERR;

				u16 len = (flashWriteLen > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : flashWriteLen;

				ChipEnable();
				
				buf[0] = CMD_PP;
				buf[1] = flashWriteAdr >> 16;
				buf[2] = flashWriteAdr >> 8;
				buf[3] = flashWriteAdr;

				spi.WriteAsyncDMA(buf, 4, flashWritePtr, len);

				//WritePageAsync(flashWritePtr, flashWriteAdr, flashWriteLen);

				flashState = FLASH_STATE_WRITE_PAGE_3;
			}
			else if (tm.Check(MS2CTM(10)))
			{
				DEBUG_ASSERT(0);
				lastError = POLL_TIMEOUT;
				flashState = FLASH_STATE_WAIT;
			}; 

			break;
		};

		case FLASH_STATE_WRITE_PAGE_3:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (spi.CheckWriteComplete())
			{
				ChipDisable();

				tm.Reset();

				flashState = FLASH_STATE_WRITE_PAGE_4;
			};

			break;

		case FLASH_STATE_WRITE_PAGE_4:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		{ 
			byte st = ReadStatusRegister();

			if ((st & SR_RDY_BSY) == 0)
			{
				flashState = FLASH_STATE_WRITE_PAGE_CHECK;
			}
			else if (tm.Check(MS2CTM(10)))
			{
				DEBUG_ASSERT(0);
				lastError = POLL_TIMEOUT;
				flashState = FLASH_STATE_WAIT;
			}; 

			break;
		};

		case FLASH_STATE_WRITE_PAGE_CHECK:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		{ 
			byte st = ReadProgramEraseStatus();

			if (st & mask_ProgramEraseStatus)
			{
				DEBUG_ASSERT(0);
				lastError = ERROR_PROGRAM;
				flashState = FLASH_STATE_WAIT;
			}
			else
			{
				lastError = NO_ERR;

				u16 len = (flashWriteLen > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : flashWriteLen;

				ChipEnable();
				
				buf[0] = CMD_FAST_READ;
				buf[1] = flashWriteAdr >> 16;
				buf[2] = flashWriteAdr >> 8;
				buf[3] = flashWriteAdr;
				buf[4] = 0;

				spi.WriteSyncDMA(buf, 5);
				spi.ReadAsyncDMA(bufpage, len);

				//ReadAsyncDMA(bufpage, flashWriteAdr, flashWriteLen);

				flashState = FLASH_STATE_VERIFY_PAGE;
			};

			break;
		};

		case FLASH_STATE_VERIFY_PAGE:	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (spi.CheckReadComplete())
			{
				ChipDisable();

				bool c = false;

				u16 len = (flashWriteLen > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : flashWriteLen;

				for (u32 i = 0; i < flashWriteLen; i++)
				{
					if (flashWritePtr[i] != bufpage[i]) { c = true; break; };
				};

				if (c)
				{
					Pin_VerifyPageError_Set();

					DEBUG_ASSERT(0);

					lastError = VERIFY_WRITE;
				}
				else
				{
					lastError = NO_ERR;
				};

				flashWriteLen -= len;

				if (flashWriteLen != 0)
				{
					flashWritePtr += len;
					flashWriteAdr += len;

					flashState = FLASH_STATE_WRITE_START;
				}
				else
				{
					flashWritePtr = 0;
					flashWriteLen = 0;

					flashState = FLASH_STATE_WAIT;
				};

				Pin_VerifyPageError_Clr();

				//HW::PIOC->CLR(PC9);
			};

			break;
	};
}

#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef ADSP_CHECKFLASH

void FlashSPI::ADSP_CheckFlash()
{
	WaitBusy();

	u32 *p = (u32*)&bh;

	u32 adr = 0;
	
	flashOK = flashChecked = false;

	#ifdef ADSP_CRC_PROTECTION
		flashCRCOK = false;
	#endif

	//at25df021_Read(buf, 0, sizeof(buf));

	while (1)
	{
		Read(adr, (byte*)&bh, sizeof(bh));

		u32 x = p[0] ^ p[1] ^ p[2] ^ p[3];
		x ^= x >> 16; 
		x = (x ^ (x >> 8)) & 0xFF; 

		if (((u32)(bh.dBlockCode) >> 24) == 0xAD && x == 0)
		{
			adr += sizeof(bh);

			if ((bh.dBlockCode & BFLG_FILL) == 0)
			{
				adr += bh.dByteCount;	
			};

			if (bh.dBlockCode & BFLG_FINAL)
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

	#ifdef ADSP_CRC_PROTECTION

		Read(adr, &adsp_crc, sizeof(adsp_crc));

		if (flashLen > 0)
		{
			flashCRC = this->GetCRC16(0, flashLen);
			flashCRCOK = (flashCRC == adsp_crc);
		};
		if (!flashCRCOK) flashLen = 0;

	#endif

	flashChecked = true;
}

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // FLASHINT_IMP_H__24_02_2024__16_53
