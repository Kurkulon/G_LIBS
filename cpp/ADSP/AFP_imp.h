#ifndef AFP_IMP_H__16_07_2024__14_53
#define AFP_IMP_H__16_07_2024__14_53

#pragma once
#pragma rtcheck(off)

#include "core.h"
#include <stdio.h>
#include <stdlib.h>					// malloc includes
//#include "types.h"

#ifdef __ADSPBF59x__

#ifndef BAUD_RATE_DIVISOR 
#define BAUD_RATE_DIVISOR 	2
#endif

#define COMMON_SPI_SETTINGS (SPE|MSTR|CPOL|CPHA)  /* settings to the SPI_CTL */
#define TIMOD01 (0x01)                  /* sets the SPI to work with core instructions */

#elif defined(__ADSPBF70x__)

#ifndef BAUD_RATE_DIVISOR 
#define BAUD_RATE_DIVISOR 	2
#endif

#define COMMON_SPI_SETTINGS (SPI_EN|SPI_MSTR|SPI_CPOL|SPI_CPHA|SPI_FMODE)  /* settings to the SPI_CTL */

#define SPI_CS	PB15

#endif

#ifdef _MISRA_RULES
#pragma diag(suppress:misra_rule_5_7:"ADI header will re-use identifiers")
#pragma diag(suppress:misra_rule_6_3:"ADI header allows use of basic types")
#pragma diag(suppress:misra_rule_18_4:"ADI header requires use of unions")
#pragma diag(suppress:misra_rule_19_15:"ADI header defines and/or undefines macros within a block")
#endif /* _MISRA_RULES */

/* Get-Codes data type */
typedef struct Get_Codes_Struct
{
	unsigned long	*pManCode;
	unsigned long	*pDevCode;
	unsigned long 	ulFlashStartAddr;

}GET_CODES_STRUCT;

/* Reset data type */
typedef struct Reset_Struct
{
 	unsigned long 	ulFlashStartAddr;

}RESET_STRUCT;

/* Erase-All data type */
typedef struct Erase_All_Struct
{
 	unsigned long 	ulFlashStartAddr;

}ERASE_ALL_STRUCT;

/* Erase-Sector data type */
typedef struct Erase_Sector_Struct
{
 	int 			nSectorNum;
 	unsigned long 	ulFlashStartAddr;

}ERASE_SECTOR_STRUCT;


/* Get-Sector-Number data type */
typedef struct Get_SectNum_Struct
{
	unsigned long	ulOffset;
	unsigned long	*pSectorNum;

}GET_SECTNUM_STRUCT;

/* Get-Sector-Start-End data type */
typedef struct Get_SectStartEnd_Struct
{
	int 	nSectorNum;
	unsigned long	*pStartOffset;
	unsigned long	*pEndOffset;

}GET_SECTSTARTEND_STRUCT;

/* Get-Description data type */
typedef struct Get_Desc_Struct
{
	char	*pTitle;
	char	*pDesc;
	char 	*pFlashCompany;

}GET_DESC_STRUCT;

/* Get-Num Sectors data type */
typedef struct Get_NumSectors_Struct
{
	int		*pnNumSectors;

}GET_NUM_SECTORS_STRUCT;

/* Get-Flash-Width data type */
typedef struct Get_Flash_Width_Struct
{
	int		*pnFlashWidth;	

}GET_FLASH_WIDTH_STRUCT;

/* Supports-CFI data type */
typedef struct Supports_CFI_Struct
{
	bool	*pbSupportsCFI;

}SUPPORTS_CFI_STRUCT;

/* Get-CFI-Data data type */
typedef struct Get_CFI_Data_Struct
{
	void *pCFIData;
	unsigned long 	ulFlashStartAddr;
	
}GET_CFI_DATA_STRUCT;


/* union describing all the data types */
typedef union
{
	ERASE_ALL_STRUCT		SEraseAll;
	ERASE_SECTOR_STRUCT		SEraseSect;
	GET_CODES_STRUCT		SGetCodes;
	GET_DESC_STRUCT			SGetDesc;
	GET_SECTNUM_STRUCT		SGetSectNum;
	GET_SECTSTARTEND_STRUCT SSectStartEnd;
	RESET_STRUCT			SReset;
	GET_NUM_SECTORS_STRUCT	SGetNumSectors;
	GET_FLASH_WIDTH_STRUCT	SGetFlashWidth;
	SUPPORTS_CFI_STRUCT		SSupportsCFI;
	GET_CFI_DATA_STRUCT		SGetCFIData;
}COMMAND_STRUCT;

typedef enum
{
	_FLASH_WIDTH_8 = 1,
	_FLASH_WIDTH_16 = 2,
	_FLASH_WIDTH_32 = 4,
	_FLASH_WIDTH_64 = 8
	
} enFlashWidth;


/* enum for adi_pdd_control */
typedef enum
{
	CNTRL_GET_CODES,		/* 0 - gets manufacturer and device IDs */
	CNTRL_RESET,			/* 1 - resets device */	
	CNTRL_ERASE_ALL,		/* 2 - erase entire device */
	CNTRL_ERASE_SECT,		/* 3 - erase a sector */
	CNTRL_GET_SECTNUM,		/* 4 - get sector number */
	CNTRL_GET_SECSTARTEND,	/* 5 - get sector start and end addresses */
	CNTRL_GET_DESC,			/* 6 - get device description */
	CNTRL_GETNUM_SECTORS,	/* 7 - get number of sectors in device */
	CNTRL_GET_FLASH_WIDTH,	/* 8 - get the flash width */
	CNTRL_SUPPORTS_CFI,		/* 9 - does the device support CFI? */
	CNTRL_GET_CFI_DATA		/* 10 - gets CFI data */
} enCntrlCmds;

#if defined(ADI_FLASH_DRIVER_RELEASE_BUILD)
#warning "Flash Driver may not work in the Release build"
#endif

#ifdef _MISRA_RULES
#pragma diag(suppress:misra_rule_6_3:"ADI header allows use of basic types")
#pragma diag(suppress:misra_rule_19_15:"ADI header defines and/or undefines macros within a block")
#endif /* _MISRA_RULES */

/* enum of possible errors */
typedef enum
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
	NUM_ERROR_CODES
}ERROR_CODE;


#ifndef NUM_SECTORS
#define NUM_SECTORS 	8			/* number of sectors in the flash device */
#endif

#ifndef SECTOR_SIZE
#define SECTOR_SIZE		4096
#endif

//static char 	*pFlashDesc =		"Atmel AT25DF021";
//static char 	*pDeviceCompany	=	"Atmel Corporation";

static int		gNumSectors = NUM_SECTORS;

#undef TIMEOUT
#undef DELAY

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
#define WIP					(0x1)	//Check the write in progress bit of the SPI status register
#define WEL					(0x2)	//Check the write enable bit of the SPI status register
#define SPI_UPS				(0x39)  //Unprotect Sector 
#define SPI_PRS				(0x36)  //Protect Sector 


#define SPI_PAGE_SIZE		(256)
//#define SPI_SECTORS		    (512)
//#define SPI_SECTOR_SIZE		(4224)
//#define SPI_SECTOR_DIFF		(3968)
//#define PAGE_BITS			(10)
//#define PAGE_SIZE_DIFF		(496)

#define DELAY				15
#define TIMEOUT        35000*64

//char			SPI_Page_Buffer[SPI_PAGE_SIZE];
//int 			SPI_Page_Index = 0;
//char            SPI_Status;

ERROR_CODE GetSectorStartEnd( unsigned long *ulStartOff, unsigned long *ulEndOff, int nSector );
ERROR_CODE GetSectorNumber( unsigned long ulAddr, int *pnSector );

/* function prototypes */
static ERROR_CODE SetupForFlash();
static ERROR_CODE Wait_For_nStatus(void);
ERROR_CODE Wait_For_Status( char Statusbit );
static ERROR_CODE Wait_For_WEL(void);
byte ReadStatusRegister(void);
extern void SetupSPI();
extern void SPI_OFF(void);
void SendSingleCommand( const int iCommand );
//unsigned long DataFlashAddress (unsigned long address);

static ERROR_CODE PollToggleBit(void);
static byte ReadFlash();
static byte WriteFlash(byte usValue);
static void WriteFlashByte(byte usValue);
static unsigned long GetFlashStartAddress( unsigned long ulAddr);
static void GlobalUnProtect();

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

//Flash Programmer commands
typedef enum
{
	FLASH_NO_COMMAND,		// 0
	FLASH_GET_CODES,		// 1
	FLASH_RESET,			// 2
	FLASH_WRITE,			// 3
	FLASH_FILL,				// 4
	FLASH_ERASE_ALL,		// 5
	FLASH_ERASE_SECT,		// 6
	FLASH_READ,				// 7
	FLASH_GET_SECTNUM,		// 8
	FLASH_GET_SECSTARTEND,	// 9
}enProgCmds;

//----- g l o b a l s -----//

char 			*AFP_Title ;							// EzKit info
char 			*AFP_Description;						// Device Description
char			*AFP_DeviceCompany;						// Device Company
char 			*AFP_DrvVersion		= "1.01.0";			// Driver Version
char			*AFP_BuildDate		= __DATE__;			// Driver Build Date
enProgCmds 		AFP_Command 		= FLASH_NO_COMMAND;	// command sent down from the GUI
int 			AFP_ManCode 		= -1;				// 0x20 = Numonyx
int 			AFP_DevCode 		= -1;				// 0x15 = M25P16
unsigned long 	AFP_Offset 			= 0x0;				// offset into flash
int 			*AFP_Buffer;							// buffer used to read and write flash
long 			AFP_Size 			= BUFFER_SIZE;		// buffer size
long 			AFP_Count 			= -1;				// count of locations to be read or written
long 			AFP_Stride 			= -1;				// stride used when reading or writing
int				AFP_SectorSize		= -1;
int 			AFP_NumSectors 		= -1;				// number of sectors in the flash device
int 			AFP_Sector 			= -1;				// sector number
int 			AFP_Error 			= NO_ERR;			// contains last error encountered
bool 			AFP_Verify 			= FALSE;			// verify writes or not
unsigned long 	AFP_StartOff 		= 0x0;				// sector start offset
unsigned long 	AFP_EndOff 			= 0x0;				// sector end offset
int				AFP_FlashWidth		= 0x8;				// width of the flash device
int 			*AFP_SectorInfo;

bool bExit = FALSE; 								//exit flag

#ifdef CPU_BF592
static char *pEzKitTitle = "ADSP-BF592A KARSAR";
#elif defined(CPU_BF706)
static char *pEzKitTitle = "ADSP-BF706 KARSAR";
#else
static char *pEzKitTitle = "ADSP-BF706 KARSAR";
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32 GetNumSectors()
{
	return gNumSectors;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32 GetSectorSize()
{
	return SECTOR_SIZE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//#pragma optimize_for_speed

//////////////////////////////////////////////////////////////
// void Wait_For_SPIF(void)
//
// Polls the SPIF (SPI single word transfer complete) bit
// of SPISTAT until the transfer is complete.
// Inputs - none
// returns- none
//
//////////////////////////////////////////////////////////////
inline void Wait_For_SPIF(void)
{
	delay(DELAY);

#ifdef __ADSPBF59x__

	while((*pSPI0_STAT & SPIF) == 0) PIO_TST->Set = TST, PIO_TST->Clear = TST;

#elif defined(__ADSPBF70x__)

	while((HW::SPI2->STAT & STAT_SPIF) == 0) PIO_TST->DATA_SET = TST, PIO_TST->DATA_CLR = TST;// HW::PORTB->NOT(PB5);

	//asm("R0 = W[%0];" : : "p" (&(HW::SPI2->RFIFO)));
	//asm("R0 = W[%0];" : : "p" (&(HW::SPI2->RFIFO)));

#endif

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline void Wait_For_RXS_SPIF(void)
{
	//volatile int n;

	//for(n=0; n<DELAY; n++)
	//{
	//	asm("nop;");
	//}

#ifdef __ADSPBF59x__

	while((*pSPI0_STAT & (SPIF|RXS)) != (SPIF|RXS)) PIO_TST->Set = TST, PIO_TST->Clear = TST;

#elif defined(__ADSPBF70x__)

	//HW::PORTB->SET(PB5);
	while((HW::SPI2->STAT & (STAT_SPIF|STAT_RFE)) != STAT_SPIF) PIO_TST->DATA_SET = TST, PIO_TST->DATA_CLR = TST;// HW::PORTB->NOT(PB5);
	//HW::PORTB->CLR(PB5);

#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE at25df021_Open(void)
{
	return (NO_ERR);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE at25df021_Close(void)
{
	return (NO_ERR);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE at25df021_Read(byte *data, u32 stAdr, u32 count )
{
    ERROR_CODE Result = NO_ERR;

	SetupSPI();

        /* send the bulk erase command to the flash */
    WriteFlash(SPI_FAST_READ);
    WriteFlash((stAdr) >> 16);
    WriteFlash((stAdr) >> 8);
    WriteFlash(stAdr);
    WriteFlash(0);

	for ( ; count > 0; count--)
	{
		*data++ = ReadFlash();
	};

    SPI_OFF();

	return(Result);
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE WritePage(byte *data, u32 stAdr, u16 count )
{
    ERROR_CODE Result = NO_ERR;

	if ((stAdr & 0xFF) != 0 || count > 256 || count == 0)
	{
		return INVALID_BLOCK;
	};

    SendSingleCommand(SPI_WREN);

    Result = Wait_For_WEL();

    if( POLL_TIMEOUT == Result )
	{
		return Result;
	}
    else
    {
        SetupSPI();

        /* send the bulk erase command to the flash */
        WriteFlashByte(SPI_PP );
        WriteFlashByte((stAdr) >> 16);
        WriteFlashByte((stAdr) >> 8);
        WriteFlashByte(stAdr);

		for ( ; count > 0; count--)
		{
	        WriteFlashByte(*data++);
		};

        SPI_OFF();
    };

	return Wait_For_Status(WIP);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE VerifyPage(byte *data, u32 stAdr, u16 count )
{
    ERROR_CODE Result = NO_ERR;

	if ((stAdr & 0xFF) != 0 || count > 256 || count == 0)
	{
		return INVALID_BLOCK;
	};

	SetupSPI();

        /* send the bulk erase command to the flash */
    WriteFlashByte(SPI_FAST_READ);
    WriteFlashByte((stAdr) >> 16);
    WriteFlashByte((stAdr) >> 8);
    WriteFlashByte(stAdr);
    WriteFlash(0);

	for ( ; count > 0; count--)
	{
		if (ReadFlash() != *data)
		{
			Result = VERIFY_WRITE;
			break;
		};

		data++;
	};

    SPI_OFF();

	return Result;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE at25df021_Write(byte *data, u32 stAdr, u32 count, bool verify)
{
    ERROR_CODE Result = NO_ERR;

	u32 c;

	while (count > 0)
	{
		u16 c = (count >= 256) ? 256 : count;

		count -= c;

		Result = WritePage(data, stAdr, c);
		if (Result != NO_ERR) break;

		if (verify)
		{
			Result = VerifyPage(data, stAdr, c);
			if (Result != NO_ERR) break;
		};

		data += c;
		stAdr += c;

    };

    return(Result);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE at25df021_Control(  unsigned int uiCmd,
                            COMMAND_STRUCT *pCmdStruct)
{
	ERROR_CODE ErrorCode = NO_ERR;

	// switch on the command
	switch ( uiCmd )
	{
		// erase all
		case CNTRL_ERASE_ALL:
//			ErrorCode = EraseFlash();
			break;

		// erase sector
		case CNTRL_ERASE_SECT:
//			ErrorCode = EraseBlock( pCmdStruct->SEraseSect.nSectorNum, pCmdStruct->SEraseSect.ulFlashStartAddr );
			break;

		// get manufacturer and device codes
		case CNTRL_GET_CODES:
//			ErrorCode = GetCodes((int *)pCmdStruct->SGetCodes.pManCode, (int *)pCmdStruct->SGetCodes.pDevCode, (unsigned long)pCmdStruct->SGetCodes.ulFlashStartAddr);
			break;

		case CNTRL_GET_DESC:
			//Filling the contents with data
			pCmdStruct->SGetDesc.pDesc			= AFP_Description;
			pCmdStruct->SGetDesc.pFlashCompany  = AFP_DeviceCompany;
			break;

		// get sector number based on address
		case CNTRL_GET_SECTNUM:
			ErrorCode = GetSectorNumber( pCmdStruct->SGetSectNum.ulOffset, (int *)pCmdStruct->SGetSectNum.pSectorNum );
			break;

		// get sector number start and end offset
		case CNTRL_GET_SECSTARTEND:
			ErrorCode = GetSectorStartEnd( pCmdStruct->SSectStartEnd.pStartOffset, pCmdStruct->SSectStartEnd.pEndOffset, pCmdStruct->SSectStartEnd.nSectorNum );
			break;

		// get the number of sectors
		case CNTRL_GETNUM_SECTORS:
			pCmdStruct->SGetNumSectors.pnNumSectors[0] = gNumSectors;
			break;

		// reset
		case CNTRL_RESET:
//			ErrorCode = ResetFlash(pCmdStruct->SReset.ulFlashStartAddr);
			break;

		// no command or unknown command do nothing
		default:
			// set our error
			ErrorCode = UNKNOWN_COMMAND;
			break;
	}

	// return
	return(ErrorCode);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-----  H e l p e r   F u n c t i o n s	----//

//----------- R e s e t F l a s h  ( ) ----------//
//
//  PURPOSE
//  	Sends a "reset" command to the flash.
//
//	INPUTS
//		unsigned long ulStartAddr - flash start address
//
// 	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise

ERROR_CODE ResetFlash()
{
	SetupSPI();

	//send the bulk erase command to the flash
	WriteFlash(SPI_WRDI);

	SPI_OFF();

	return PollToggleBit();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void GlobalUnProtect()
{
	ERROR_CODE 	  ErrorCode   = NO_ERR;	

	SetupSPI();

	WriteFlash(SPI_WREN );

	SPI_OFF();

	SetupSPI();

	WriteFlashByte(SPI_WRSR);
	WriteFlashByte(0);

	SPI_OFF();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//----------- E r a s e F l a s h  ( ) ----------//
//
//  PURPOSE
//  	Sends an "erase all" command to the flash.
//
//	INPUTS
//		unsigned long ulStartAddr - flash start address
//
// 	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise

ERROR_CODE EraseFlash()
{
	ERROR_CODE ErrorCode = NO_ERR;	// tells us if there was an error erasing flash
//	int nBlock = 0;					// index for each block to erase

	GlobalUnProtect();
	GlobalUnProtect();

	//A write enable instruction must previously have been executed
	SendSingleCommand(SPI_WREN);

	//The status register will be polled to check the write enable latch "WREN"
	ErrorCode = Wait_For_WEL();

	if( POLL_TIMEOUT == ErrorCode )
	{
		return ErrorCode;
	}
	else
	{
	    //The bulk erase instruction will erase the whole flash
		SendSingleCommand(SPI_BE);

		// Erasing the whole flash will take time, so the following bit must be polled.
		//The status register will be polled to check the write in progress bit "WIP"
		ErrorCode = Wait_For_Status(WIP);

		printf("Error Code: %d", ErrorCode);


	}

	// erase should be complete
	return ErrorCode;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE UnProtectBlock(u32 adr)
{
	ERROR_CODE 	  ErrorCode   = NO_ERR;		//tells us if there was an error erasing flash

	SetupSPI();

	// send the write enable instruction
	WriteFlash(SPI_WREN );

	SPI_OFF();

	SetupSPI();

	WriteFlash(SPI_WRSR);
	WriteFlash(0);

	SPI_OFF();

	// Poll the status register to check the Write in Progress bit
	// Sector erase takes time
	return ErrorCode;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//----------- E r a s e B l o c k ( ) ----------//
//
//  PURPOSE
//  	Sends an "erase block" command to the flash.
//
//	INPUTS
//		int nBlock - block to erase
//		unsigned long ulStartAddr - flash start address
//
// 	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise

ERROR_CODE EraseBlock(int nBlock)
{

	ERROR_CODE 	  ErrorCode   = NO_ERR;		//tells us if there was an error erasing flash
 	unsigned long ulSectStart = 0x0;		//stores the sector start offset
 	unsigned long ulSectEnd   = 0x0;		//stores the sector end offset(however we do not use it here)

	// Get the sector start offset
	// we get the end offset too however we do not actually use it for Erase sector
	GetSectorStartEnd( &ulSectStart, &ulSectEnd, nBlock );

	GlobalUnProtect();
	GlobalUnProtect();

	SetupSPI();

	// send the write enable instruction
	WriteFlash(SPI_WREN );

	SPI_OFF();

	SetupSPI();

	//send the erase block command to the flash
	WriteFlashByte(SPI_SE );

	WriteFlashByte(GB(&ulSectStart, 2));
	WriteFlashByte(GB(&ulSectStart, 1));
	WriteFlashByte(GB(&ulSectStart, 0));

	SPI_OFF();

	// Poll the status register to check the Write in Progress bit
	// Sector erase takes time
	ErrorCode = Wait_For_Status(WIP);



 	// block erase should be complete
	return ErrorCode;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//----------- P o l l T o g g l e B i t ( ) ----------//
//
//  PURPOSE
//  	Polls the toggle bit in the flash to see when the operation
//		is complete.
//
//	INPUTS
//	unsigned long ulAddr - address in flash
//
// 	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise


ERROR_CODE PollToggleBit(void)
{
	ERROR_CODE ErrorCode = NO_ERR;	// flag to indicate error
	char status_register = 0;
	int i;

	for(i = 0; i < 500; i++)
	{
		status_register = ReadStatusRegister();
		if( (status_register & WEL) )
		{
			ErrorCode = NO_ERR;

		}
		ErrorCode = POLL_TIMEOUT;	// Time out error
	};

	// we can return
	return ErrorCode;
}

#pragma optimize_as_cmd_line

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//----------- G e t C o d e s ( ) ----------//
//
//  PURPOSE
//  	Sends an "auto select" command to the flash which will allow
//		us to get the manufacturer and device codes.
//
//  INPUTS
//  	int *pnManCode - pointer to manufacture code
//		int *pnDevCode - pointer to device code
//		unsigned long ulStartAddr - flash start address
//
//	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE GetCodes(int *pnManCode, int *pnDevCode)
{
	//Open the SPI, Deasserting CS
	SetupSPI();

	//Write the OpCode and Write address, 4 bytes.
	WriteFlash( SPI_RDID );

	// now we can read the codes
	*pnManCode = ReadFlash();

	*pnDevCode = ReadFlash() << 8;
	*pnDevCode |= ReadFlash();

	SPI_OFF();
	// ok
	return NO_ERR;
}

//----------- G e t S e c t o r N u m b e r ( ) ----------//
//
//  PURPOSE
//  	Gets a sector number based on the offset.
//
//  INPUTS
//  	unsigned long ulAddr - absolute address
//		int 	 *pnSector     - pointer to sector number
//
//	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//#pragma optimize_for_speed

ERROR_CODE GetSectorNumber( unsigned long ulAddr, int *pnSector )
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

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//----------- G e t S e c t o r S t a r t E n d ( ) ----------//
//
//  PURPOSE
//  	Gets a sector start and end address based on the sector number.
//
//  INPUTS
//  	unsigned long *ulStartOff - pointer to the start offset
//		unsigned long *ulEndOff - pointer to the end offset
//		int nSector - sector number
//
//	RETURN VALUE
//		ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ERROR_CODE GetSectorStartEnd( unsigned long *ulStartOff, unsigned long *ulEndOff, int nSector )
{
	u32 ulSectorSize = SECTOR_SIZE;

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

//----------- G e t F l a s h S t a r t A d d r e s s ( ) ----------//
//
//  PURPOSE
//  	Gets flash start address from an absolute address.
//
//  INPUTS
//  	unsigned long ulAddr - absolute address
//
//	RETURN VALUE
//		unsigned long - Flash start address

unsigned long GetFlashStartAddress( unsigned long ulAddr)
{

	unsigned long ulFlashStartAddr;			//flash start address

	ulFlashStartAddr  =  0;

	return(ulFlashStartAddr);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//----------- R e a d F l a s h ( ) ----------//
//
//  PURPOSE
//  	Reads a value from an address in flash.
//
//  INPUTS
// 		unsigned long ulAddr - the address to read from
// 		int pnValue - pointer to store value read from flash
//
//	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise

byte ReadFlash()
{
#ifdef __ADSPBF59x__

	//asm("R0 = W[%0];" : : "p" (pSPI0_RDBR));
	//Wait_For_SPIF();

	//*pSPI0_TDBR = 0;
	//Wait_For_RXS_SPIF();

	//return *pSPI0_RDBR;	

	HW::SPI0->TDBR = 0;

	while((HW::SPI0->Stat & RXS) == 0) PIO_TST->Set = TST, PIO_TST->Clear = TST;
	//while((HW::SPI0->Stat & (SPIF|RXS)) != (SPIF|RXS)) PIO_TST->Set = TST, PIO_TST->Clear = TST;

	return HW::SPI0->RDBR;

#elif defined(__ADSPBF70x__)


	//asm("R0 = W[%0];" : : "p" (&(HW::SPI2->RFIFO)));
	//Wait_For_SPIF();

	HW::SPI2->TFIFO = 0;

	while((HW::SPI2->STAT & STAT_RFE) != 0) PIO_TST->DATA_SET = TST, PIO_TST->DATA_CLR = TST;// HW::PORTB->NOT(PB5);

	return HW::SPI2->RFIFO;

#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//----------- W r i t e F l a s h ( ) ----------//
//
//  PURPOSE
//  	Write a value to an address in flash.
//
//  INPUTS
//	 	unsigned long  ulAddr - address to write to
//		unsigned short nValue - value to write
//
//	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise

byte WriteFlash(byte usValue)
{
#ifdef __ADSPBF59x__

	HW::SPI0->TDBR = usValue;

	delay(DELAY);
	
	//while((HW::SPI0->Stat & RXS) == 0) PIO_TST->Set = TST, PIO_TST->Clear = TST;
	while((HW::SPI0->Stat & (SPIF|RXS)) != (SPIF|RXS)) PIO_TST->Set = TST, PIO_TST->Clear = TST;

	return HW::SPI0->RDBR;

#elif defined(__ADSPBF70x__)

	HW::SPI2->TFIFO = usValue;
	
	delay(DELAY);

	while((HW::SPI2->STAT & (STAT_SPIF|STAT_RFE)) != STAT_SPIF) PIO_TST->DATA_SET = TST, PIO_TST->DATA_CLR = TST;// HW::PORTB->NOT(PB5);

	return HW::SPI2->RFIFO;

#endif	
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void WriteFlashByte(byte usValue)
{
#ifdef __ADSPBF59x__

	while((HW::SPI0->Stat & TXS) != 0) PIO_TST->Set = TST, PIO_TST->Clear = TST;
	
	HW::SPI0->TDBR = usValue;
	
	//while((HW::SPI0->Stat & RXS) == 0) PIO_TST->Set = TST, PIO_TST->Clear = TST;
	//while((HW::SPI0->Stat & (SPIF|RXS)) != (SPIF|RXS)) PIO_TST->Set = TST, PIO_TST->Clear = TST;

	//return HW::SPI0->RDBR;

#elif defined(__ADSPBF70x__)

	while((HW::SPI2->STAT & STAT_TFF) != 0) PIO_TST->DATA_SET = TST, PIO_TST->DATA_CLR = TST;// HW::PORTB->NOT(PB5);

	HW::SPI2->TFIFO = usValue;
	
	//Wait_For_SPIF();

#endif	
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//////////////////////////////////////////////////////////////
// int ReadStatusRegister(void)
//
// Returns the 8-bit value of the status register.
// Inputs - none
// returns- second location of status_register[2],
//         first location is garbage.
// Core sends the command
//
//////////////////////////////////////////////////////////////

byte ReadStatusRegister(void)
{
	SetupSPI(); // Turn on the SPI

	//send instruction to read status register
	WriteFlash(SPI_RDSR);

	// receive the status register
	byte usStatus = ReadFlash();

	SPI_OFF();		// Turn off the SPI

	return usStatus;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//////////////////////////////////////////////////////////////
// Wait_For_WEL(void)
//
// Polls the WEL (Write Enable Latch) bit of the Flash's status
// register.
// Inputs - none
// returns- none
//
//////////////////////////////////////////////////////////////

ERROR_CODE Wait_For_WEL(void)
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
		}

		for(n=0; n<DELAY; n++)
			asm("nop;");
		ErrorCode = POLL_TIMEOUT;	// Time out error
	}


	return ErrorCode;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//////////////////////////////////////////////////////////////
// Wait_For_Status(void)
//
// Polls the Status Register of the Flash's status
// register until the Flash is finished with its access. Accesses
// that are affected by a latency are Page_Program, Sector_Erase,
// and Block_Erase.
// Inputs - Statusbit
// returns- none
//
//////////////////////////////////////////////////////////////

ERROR_CODE Wait_For_Status( char Statusbit )
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

		for(n=0; n<DELAY; n++)
			asm("nop;");
		ErrorCode = POLL_TIMEOUT;	// Time out error
	}


	return ErrorCode;

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//////////////////////////////////////////////////////////////
// void SendSingleCommand( const int iCommand )
//
// Sends a single command to the SPI flash
// inputs - the 8-bit command to send
// returns- none
//
//////////////////////////////////////////////////////////////
void SendSingleCommand( const int iCommand )
{
	volatile int n;

	//turns on the SPI in single write mode
	SetupSPI();

	//sends the actual command to the SPI TX register

	WriteFlash(iCommand);

//#ifdef __ADSPBF59x__
//	*pSPI0_TDBR = iCommand;
//#elif defined(__ADSPBF70x__)
//	HW::SPI2->TFIFO = iCommand;
//#endif
//	//The SPI status register will be polled to check the SPIF bit
//	Wait_For_SPIF();

	//The SPI will be turned off
	SPI_OFF();

	//Pause before continuing
	for(n=0; n<DELAY; n++)
	{
		asm("nop;");
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//////////////////////////////////////////////////////////////
// Sets up the SPI for mode specified in spi_setting
// Inputs - spi_setting
// returns- none
//////////////////////////////////////////////////////////////
void SetupSPI()
{
    volatile int i;

#ifdef __ADSPBF59x__

	/* PF8 - SPI0_SSEL2 */

	*pPORTF_FER   |= (PF13 | PF14 | PF15);
	*pPORTF_FER   &= ~(PF8);
	*pPORTF_MUX   &= ~(PF13 | PF14 | PF15);
   	*pPORTFIO_SET = PF8;
  	*pPORTFIO_DIR |= PF8;
   	*pPORTFIO_SET = PF8;

 //  	for(i=0; i<DELAY; i++)
	//{
		asm("nop;");
		asm("nop;");
		asm("nop;");
		asm("nop;");
		asm("nop;");
//	}

	*pSPI0_BAUD = BAUD_RATE_DIVISOR;
	*pSPI0_CTL = COMMON_SPI_SETTINGS|TIMOD01;	
	*pPORTFIO_CLEAR = PF8;

#elif defined(__ADSPBF70x__)

	HW::PORTB->FER_SET = PB10|PB11|PB12;
	HW::PORTB->MUX &= ~(PB10|PB11|PB12);
	HW::PORTB->DATA_SET = PB15;
	HW::PORTB->DIR_SET = PB15;
	HW::PORTB->DATA_SET = PB15;

	delay(10);

	HW::SPI2->CLK = BAUD_RATE_DIVISOR;
	HW::SPI2->CTL = COMMON_SPI_SETTINGS;
	HW::SPI2->TXCTL = TXCTL_TEN|TXCTL_TTI;
	HW::SPI2->RXCTL = RXCTL_REN/*|RXCTL_RTI*/;
	HW::SPI2->DLY = 0x01;
	HW::SPI2->SLVSEL = 0xFC00;

	HW::PORTB->CLR(PB15);

#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//////////////////////////////////////////////////////////////
// Turns off the SPI
// Inputs - none
// returns- none
//
//////////////////////////////////////////////////////////////

void SPI_OFF(void)
{
	volatile int i;

	Wait_For_SPIF();

#ifdef __ADSPBF59x__

	*pPORTFIO_SET = PF8;
	*pSPI0_CTL = CPHA|CPOL;	// disable SPI
	*pSPI0_BAUD = 0;

#elif defined(__ADSPBF70x__)

	HW::PORTB->SET(PB15);
	HW::SPI2->CTL &= ~1;
	HW::SPI2->CLK = 0;
	HW::SPI2->TXCTL = 0;
	HW::SPI2->RXCTL = 0;
	HW::SPI2->SLVSEL = 0xFE00;

#endif	

	delay(10);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//////////////////////////////////////////////////////////////
// Wait_For_nStatus(void)
//
// Polls the WEL (Write Enable Latch) bit of the Flash's status
// register.
// Inputs - none
// returns- none
//
//////////////////////////////////////////////////////////////
ERROR_CODE Wait_For_nStatus(void)
{
	volatile int i;
	char status_register = 0;
	ERROR_CODE ErrorCode = NO_ERR;	// tells us if there was an error erasing flash

	for(i = 0; i < 500; i++)
	{
		status_register = ReadStatusRegister();
		if( (status_register & WEL) )
		{
			ErrorCode = NO_ERR;
			return ErrorCode;
		}
		ErrorCode = POLL_TIMEOUT;	// Time out error
	}

	return ErrorCode;
}

#pragma optimize_as_cmd_line

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/////////////////////////////////////////////////////////////////
// unsigned long DataFlashAddress()
//
// Translates a binary address into the appropriate dataflash address.
//
// Inputs: unsigned long address - The binary address supplied
/////////////////////////////////////////////////////////////////
//unsigned long DataFlashAddress (unsigned long address)
//{
//	// Determine the page that it is on
//	unsigned long pageAddress = address / SPI_PAGE_SIZE;
//
//	// Determine the byte offset within the page
//	unsigned long bitAddress = address % SPI_PAGE_SIZE;
//
//	// Shift the page address the correct number of bits
//	pageAddress = pageAddress << PAGE_BITS;
//
//	// Return the combined Page and Byte offset address
//	return (pageAddress | bitAddress);
//}
//

#ifndef FLASH_START_ADDR
#define FLASH_START_ADDR	0x00000000
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE			0x4000
#endif

byte buffer[BUFFER_SIZE];





//----- c o n s t a n t   d e f i n i t i o n s -----//

// structure for flash sector information

typedef struct _SECTORLOCATION
{
 	unsigned long ulStartOff;
	unsigned long ulEndOff;
}SECTORLOCATION;


//----- f u n c t i o n   p r o t o t y p e s -----//
ERROR_CODE AllocateAFPBuffer(void);
ERROR_CODE GetSectorMap(SECTORLOCATION *pSectInfo);
ERROR_CODE GetFlashInfo(void);
ERROR_CODE ProcessCommand(void);
ERROR_CODE FillData( unsigned long ulStart, long lCount, long lStride, int *pnData );
ERROR_CODE ReadData( unsigned long ulStart, long lCount, long lStride, int *pnData );
ERROR_CODE WriteData( unsigned long ulStart, long lCount, long lStride, int *pnData );
void FreeAFPBuffer(void);
void InitPLL_SDRAM(void);
extern ERROR_CODE SetupForFlash(void);


//------------- m a i n ( ) ----------------//

int main(void)
{
	//Init_PLL();

	PIO_MAINLOOP->DirSet(MAINLOOP);
	PIO_TST->DirSet(TST);

	SECTORLOCATION *pSectorInfo;
    ERROR_CODE Result;							// result

    /* open flash driver */
	AFP_Error = at25df021_Open();

	if (AFP_Error != NO_ERR)
		return FALSE;

	// get flash manufacturer & device codes, title & desc
	if( AFP_Error == NO_ERR )
	{
		AFP_Error = GetFlashInfo();
	};

	byte stat = ReadStatusRegister();

	// get the number of sectors for this device
	if( AFP_Error == NO_ERR )
	{
		AFP_NumSectors = GetNumSectors();
		AFP_SectorSize = GetSectorSize();
	}

	if( AFP_Error == NO_ERR )
	{
		// malloc enough space to hold our start and end offsets
		pSectorInfo = (SECTORLOCATION *)malloc(AFP_NumSectors * sizeof(SECTORLOCATION));
	}

	// allocate AFP_Buffer
	if( AFP_Error == NO_ERR )
	{
		AFP_Error = AllocateAFPBuffer();
	}

	// get sector map
	if( AFP_Error == NO_ERR )
	{
		AFP_Error = GetSectorMap(pSectorInfo);
	}

	// point AFP_SectorInfo to our sector info structure
	if( AFP_Error == NO_ERR )
	{
		AFP_SectorInfo = (int*)pSectorInfo;
	}

	// command processing loop
	while ( !bExit )
	{
		// the plug-in will set a breakpoint at "AFP_BreakReady" so it knows
		// when we are ready for a new command because the DSP will halt
		//
		// the jump is used so that the label will be part of the debug
		// information in the driver image otherwise it may be left out
 		// since the label is not referenced anywhere
		asm("AFP_BreakReady:");
       		asm("nop;");
			if ( FALSE )
				asm("jump AFP_BreakReady;");

		// Make a call to the ProcessCommand
		   AFP_Error = ProcessCommand();

		   PIO_MAINLOOP->NOT(MAINLOOP);
	};

	// Clear the AFP_Buffer
	FreeAFPBuffer();

	//if( pSectorInfo )
	//{
	//	free(pSectorInfo);
	//	pSectorInfo = NULL;
	//}

	// Close the Device
	AFP_Error = at25df021_Close();

	if (AFP_Error != NO_ERR)
		return FALSE;

	return TRUE;
}


//----------- P r o c e s s   C o m m a n d  ( ) ----------//
//
//  PURPOSE
//  	Process each command sent by the GUI based on the value in
//  	the AFP_Command.
//
// 	RETURN VALUE
//  	ERROR_CODE - value if any error occurs during Opcode scan
//  	NO_ERR     - otherwise
//
// 	CHANGES
//  	9-28-2005 Created

ERROR_CODE ProcessCommand()
{

	ERROR_CODE ErrorCode = 	NO_ERR; 		// return error code

	COMMAND_STRUCT CmdStruct;

//	AFP_Command = FLASH_ERASE_ALL;

	// switch on the command and fill command structure.
	switch ( AFP_Command )
	{

		// erase all
		case FLASH_ERASE_ALL:
			ErrorCode = EraseFlash();
			break;

		// erase sector
		case FLASH_ERASE_SECT:
			ErrorCode = EraseBlock(AFP_Sector);
			break;

		// fill
		case FLASH_FILL:
			ErrorCode = FillData( AFP_Offset, AFP_Count, AFP_Stride, AFP_Buffer );
			break;

		// get manufacturer and device codes
		case FLASH_GET_CODES:
			ErrorCode = GetCodes(&AFP_ManCode, &AFP_DevCode);
			break;

		// get sector number based on address
		case FLASH_GET_SECTNUM:
			ErrorCode = GetSectorNumber(AFP_Offset, &AFP_Sector);
			break;

		// get sector number start and end offset
		case FLASH_GET_SECSTARTEND:
			ErrorCode = GetSectorStartEnd(&AFP_StartOff, &AFP_EndOff,  AFP_Sector);
			break;

		// read
		case FLASH_READ:
			ErrorCode = ReadData( AFP_Offset, AFP_Count, AFP_Stride, AFP_Buffer );
			break;

		// reset
		case FLASH_RESET:
			ErrorCode = ResetFlash();
			break;

		// write
		case FLASH_WRITE:
			ErrorCode = WriteData( AFP_Offset, AFP_Count, AFP_Stride, AFP_Buffer );
			break;

		// no command or unknown command do nothing
		case FLASH_NO_COMMAND:
		default:
			// set our error
			ErrorCode = UNKNOWN_COMMAND;
			break;
	}

	// clear the command
	AFP_Command = FLASH_NO_COMMAND;

	return(ErrorCode);
}


//----------- A l l o c a t e A F P B u f f e r  ( ) ----------//
//
//  PURPOSE
//  	Allocate memory for the AFP_Buffer
//
// 	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise
//
// 	CHANGES
//  	9-28-2005 Created

ERROR_CODE AllocateAFPBuffer()
{

	ERROR_CODE ErrorCode = NO_ERR;	//return error code

	// by making AFP_Buffer as big as possible the plug-in can send and
	// receive more data at a time making the data transfer quicker
	//
	// by allocating it on the heap the compiler does not create an
	// initialized array therefore making the driver image smaller
	// and faster to load
	//
	// The linker description file (LDF) could be modified so that
	// the heap is larger, therefore allowing the BUFFER_SIZE to increase.

	// the data type of the data being sent from the flash programmer GUI
	// is in bytes but we store the data as integers to make data
	// manipulation easier when actually programming the data.  This is why
	// BUFFER_SIZE bytes are being allocated rather than BUFFER_SIZE * sizeof(int).
	
//	AFP_Buffer = (int *)malloc(BUFFER_SIZE);

	AFP_Buffer = (int *)buffer;

	// AFP_Buffer will be NULL if we could not allocate storage for the
	// buffer
	if ( AFP_Buffer == NULL )
	{
		// tell GUI that our buffer was not initialized
		ErrorCode = BUFFER_IS_NULL;
	}

	return(ErrorCode);
}


//----------- F r e e A F P B u f f e r  ( ) ----------//
//
//  PURPOSE
//  	Free the AFP_Buffer
//
// 	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise
//
// 	CHANGES
//  	9-28-2005 Created

void FreeAFPBuffer()
{
	// free the buffer if we were able to allocate one
//	if ( AFP_Buffer )
//		free( AFP_Buffer );

}

//----------- G e t S e c t o r M a p  ( ) ----------//
//
//  PURPOSE
//  	Get the start and end offset for each sector in the flash.
//
// 	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise
//
// 	CHANGES
//  	9-28-2005 Created

ERROR_CODE GetSectorMap(SECTORLOCATION *pSectInfo)
{

	ERROR_CODE ErrorCode = NO_ERR;			//return error code
	GET_SECTSTARTEND_STRUCT	SSectStartEnd;	//structure for GetSectStartEnd
	int i;									//index

	//initiate sector information structures
	for( i=0;i<AFP_NumSectors; i++)
	{
		SSectStartEnd.nSectorNum = i;
		SSectStartEnd.pStartOffset = &pSectInfo[i].ulStartOff;
		SSectStartEnd.pEndOffset = &pSectInfo[i].ulEndOff;

		ErrorCode = at25df021_Control( CNTRL_GET_SECSTARTEND, (COMMAND_STRUCT *)&SSectStartEnd  );
	}

	return(ErrorCode);
}


//----------- G e t F l a s h I n f o  ( ) ----------//
//
//  PURPOSE
//  	Get the manufacturer code and device code
//
// 	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise
//
// 	CHANGES
//  	9-28-2005 Created

ERROR_CODE GetFlashInfo()
{
	AFP_DeviceCompany	= "Unknow";
	AFP_Description		= "Unknow";

	ERROR_CODE ErrorCode = NO_ERR;		//return error code

	//setup code so that flash programmer can just read memory instead of call GetCodes().

	ErrorCode = GetCodes(&AFP_ManCode, &AFP_DevCode);

	if(!ErrorCode)
	{
		AFP_Title = pEzKitTitle;
		
		char *str = AFP_Description;

		if (AFP_ManCode == 0x1F)
		{
			AFP_DeviceCompany	= "Atmel Corporation";

			switch (AFP_DevCode)
			{
				case 0x4300:	str = "Atmel AT25DF021 2-Mbit"; gNumSectors = 128; break;
			};
		}
		else if (AFP_ManCode == 0x9D)
		{
			AFP_DeviceCompany	= "Integrated Silicon Solution";

			switch (AFP_DevCode)
			{
				case 0x6014:	str = "ISSI IS25LP080D 8-Mbit"; gNumSectors = 512; break;
				case 0x7014:	str = "ISSI IS25WP080D 8-Mbit"; gNumSectors = 512; break;
				case 0x7013:	str = "ISSI IS25WP040D 4-Mbit"; gNumSectors = 256; break;
				case 0x7012:	str = "ISSI IS25WP020D 2-Mbit"; gNumSectors = 128; break;
			};

		};
		
		AFP_Description	= str;
	};

	return(ErrorCode);
}


//----------- F i l l D a t a  ( ) ----------//
//
//  PURPOSE
//  	Fill flash device with a value.
//
// 	INPUTS
//	 	unsigned long ulStart - address in flash to start the writes at
// 		long lCount - number of elements to write, in this case bytes
// 		long lStride - number of locations to skip between writes
// 		int *pnData - pointer to data buffer
//
// 	RETURN VALUE
// 		ERROR_CODE - value if any error occurs during fill
// 		NO_ERR     - otherwise
//
// 	CHANGES
//  	9-28-2005 Created

ERROR_CODE FillData( unsigned long ulStart, long lCount, long lStride, int* pnData )
{
	long i = 0;							// loop counter
	ERROR_CODE ErrorCode = NO_ERR;		// tells whether we had an error while filling
	bool bVerifyError = FALSE;			// lets us know if there was a verify error
	unsigned long ulNewOffset = 0;		// used if we have an odd address
	bool bByteSwapMid = FALSE;			// if writing to odd address the middle needs byte swapping
	unsigned long ulStartAddr;   		// current address to fill
	unsigned long ulSector = 0;			// sector number to verify address
	int nCompare = 0;					// value that we use to verify flash

	ulStartAddr = FLASH_START_ADDR + ulStart;
	COMMAND_STRUCT	CmdStruct;	//structure for GetSectStartEnd

	// if we have an odd offset we need to write a byte
	// to the first location and the last
	//if(ulStartAddr%4 != 0)
	//{
	//	// read the offset - 1 and OR in our value
	//	ulNewOffset = ulStartAddr - 1;
	//	ErrorCode = at25df021_Read( (unsigned short*)&nCompare, ulNewOffset, 0x1 );

	//	nCompare &= 0x00FF;
	//	nCompare |= ( (pnData[0] << 8) & 0xFF00 );

	//	// unlock the flash, do the write, and wait for completion
	//	ErrorCode = at25df021_Write( (unsigned short*)&nCompare, ulNewOffset, 0x1 );

	//	// move to the last offset
	//	ulNewOffset = ( (ulStartAddr - 1) + (lCount * ( lStride  ) ) );

	//	// read the value and OR in our value
	//	ErrorCode = at25df021_Read( (unsigned short*)&nCompare, ulNewOffset, 0x1 );

	//	nCompare &= 0xFF00;
	//	nCompare |= ( ( pnData[0] >> 8) & 0x00FF );

	//	// unlock the flash, do the write, and wait for completion
	//	ErrorCode = at25df021_Write( (unsigned short*)&nCompare, ulNewOffset, 0x1 );

	//	// increment the offset and count
	//	ulStartAddr = ( (ulStartAddr - 1) + ( lStride  ) );
	//	lCount--;

	//	bByteSwapMid = TRUE;
	//}

	//if( bByteSwapMid == TRUE )
	//{
	//	int nTemp = (char)pnData[0];
	//	pnData[0] &= 0xFF00;
	//	pnData[0] >>= 8;
	//	pnData[0] |= (nTemp << 8);
	//}

	//// verify writes if the user wants to
	//if( AFP_Verify == TRUE )
	//{
	//	// fill the value
	//	for (i = 0; ( ( i < lCount ) && ( ErrorCode == NO_ERR ) ); i++, ulStartAddr += ( lStride ) )
	//	{

	//		// check to see that the address is within a valid sector
	//		CmdStruct.SGetSectNum.ulOffset = ulStartAddr;
	//		CmdStruct.SGetSectNum.pSectorNum = &ulSector;
	//		ErrorCode = at25df021_Control( CNTRL_GET_SECTNUM, &CmdStruct  );

	//		if( NO_ERR == ErrorCode )
	//		{
	//			// unlock the flash, do the write, and wait for completion
	//			ErrorCode = at25df021_Write( (unsigned short*)&pnData[0], ulStartAddr, 0x1 );
	//			ErrorCode = at25df021_Read( (unsigned short*)&nCompare, ulStartAddr, 0x1 );

	//			if( nCompare != ( pnData[0] & 0x0000FFFF ) )
	//			{
	//				bVerifyError = TRUE;
	//				break;
	//			}
	//		}
	//		else
	//		{
	//			return ErrorCode;
	//		}

	//	}

	//	// return appropriate error code if there was a verification error
	//	if( bVerifyError == TRUE )
	//		return VERIFY_WRITE;
	//}
	//// user did not want to verify writes
	//else
	//{
	//	// fill the value
	//	for (i = 0; ( ( i < lCount ) && ( ErrorCode == NO_ERR ) ); i++, ulStartAddr += ( lStride ))
	//	{

	//		// check to see that the address is within a valid sector
	//		CmdStruct.SGetSectNum.ulOffset = ulStartAddr;
	//		CmdStruct.SGetSectNum.pSectorNum = &ulSector;
	//		ErrorCode = at25df021_Control( CNTRL_GET_SECTNUM, &CmdStruct  );

	//		if( NO_ERR == ErrorCode )
	//		{
	//			// unlock the flash, do the write, and wait for completion
	//			ErrorCode = at25df021_Write( (unsigned short*)&pnData[0], ulStartAddr, 0x1 );
	//		}
	//		else
	//		{
	//			return ErrorCode;
	//		}
	//	}
	//}

	// return the appropriate error code
	return ErrorCode;
}

//----------- W r i t e D a t a  ( ) ----------//
//
//  PURPOSE
//  	Write a buffer to flash device.
//
// 	INPUTS
// 		unsigned long ulStart - address in flash to start the writes at
//		long lCount - number of elements to write, in this case bytes
//		long lStride - number of locations to skip between writes
//		int *pnData - pointer to data buffer
//
//  RETURN VALUE
//  	ERROR_CODE - value if any error occurs during writing
//  	NO_ERR     - otherwise
//
//  CHANGES
//  	9-28-2005 Created

ERROR_CODE WriteData( unsigned long ulStart, long lCount, long lStride, int *pnData )
{
	// if the user wants to verify then do it

	return at25df021_Write((byte*)pnData, ulStart, lCount, AFP_Verify);
}

//----------- R e a d D a t a  ( ) ----------//
//
//  PURPOSE
//  	Read a buffer from flash device.
//
// 	INPUTS
//		unsigned long ulStart - address in flash to start the reads at
//		long lCount - number of elements to read, in this case bytes
//		long lStride - number of locations to skip between reads
//		int *pnData - pointer to data buffer to fill
//
//	RETURN VALUE
//  	ERROR_CODE - value if any error occurs during reading
//  	NO_ERR     - otherwise
//
//  CHANGES
//  	9-28-2005 Created

ERROR_CODE ReadData( unsigned long ulStart, long lCount, long lStride, int *pnData )
{
	return at25df021_Read((byte*)pnData, ulStart, lCount);
}


#endif // #ifndef AFP_IMP_H__16_07_2024__14_53
