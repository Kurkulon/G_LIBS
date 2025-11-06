#ifndef AFP_IMP_H__07_08_2024__16_05
#define AFP_IMP_H__07_08_2024__16_05

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

#define COMMON_SPI_SETTINGS (SPI_EN|SPI_MSTR|SPI_CPOL|SPI_CPHA)  /* settings to the SPI_CTL */

#define SPI_CS	PB15

#elif defined(__ADSPBF60x__)

#ifndef BAUD_RATE_DIVISOR 
#define BAUD_RATE_DIVISOR 	2
#endif

#define COMMON_SPI_SETTINGS (SPI_EN|SPI_MSTR|SPI_CPOL|SPI_CPHA)  /* settings to the SPI_CTL */

//#define SPI_CS	PB15

#endif

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

///* enum of possible errors */
//typedef enum
//{
//	NO_ERR,					/* No Error */
//	POLL_TIMEOUT,			/* Polling toggle bit failed */
//	VERIFY_WRITE,			/* Verifying write to flash failed */
//	INVALID_SECTOR,			/* Invalid Sector */
//	INVALID_BLOCK,			/* Invalid Block */
//	UNKNOWN_COMMAND,		/* Unknown Command */
//	PROCESS_COMMAND_ERR,	/* Processing command */
//	NOT_READ_ERROR,			/* Could not read memory from target */
//	DRV_NOTAT_BREAK,		/* The drive was not at AFP_BreakReady */
//	BUFFER_IS_NULL,			/* Could not allocate storage for the buffer */
//	NO_ACCESS_SECTOR,		/* Cannot access the sector( could be locked or something is stored there that should not be touched ) */
//	NUM_ERROR_CODES
//}ERROR_CODE;


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
#define SPI_RDRP			(0x61)  //READ READ PARAMETERS OPERATION  
#define SPI_RDERP			(0x81)  //READ EXTENDED READ PARAMETERS OPERATION  
#define SPI_RDFR			(0x48)  //READ FUNCTION REGISTER OPERATION 


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

//ERROR_CODE GetSectorStartEnd( unsigned long *ulStartOff, unsigned long *ulEndOff, int nSector );
//ERROR_CODE GetSectorNumber( unsigned long ulAddr, int *pnSector );

/* function prototypes */
//static ERROR_CODE SetupForFlash();
//static ERROR_CODE Wait_For_nStatus(void);
//ERROR_CODE Wait_For_Status( char Statusbit );
//static ERROR_CODE Wait_For_WEL(void);
//byte ReadStatusRegister(void);
//extern void SetupSPI();
//extern void SPI_OFF(void);
//void SendSingleCommand( const int iCommand );
//unsigned long DataFlashAddress (unsigned long address);

//static ERROR_CODE PollToggleBit(void);
//static byte ReadFlash();
//static void WriteFlash(byte usValue);
//static void WriteFlashByte(byte usValue);
//static unsigned long GetFlashStartAddress( unsigned long ulAddr);
//static ERROR_CODE GlobalUnProtect();

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

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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
			ErrorCode = spiFlash.GetSectorNumber( pCmdStruct->SGetSectNum.ulOffset, (int *)pCmdStruct->SGetSectNum.pSectorNum );
			break;

		// get sector number start and end offset
		case CNTRL_GET_SECSTARTEND:
			ErrorCode = spiFlash.GetSectorStartEnd( pCmdStruct->SSectStartEnd.pStartOffset, pCmdStruct->SSectStartEnd.pEndOffset, pCmdStruct->SSectStartEnd.nSectorNum );
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
//ERROR_CODE ReadData( unsigned long ulStart, long lCount, long lStride, int *pnData );
ERROR_CODE WriteData( unsigned long ulStart, long lCount, long lStride, int *pnData );
void FreeAFPBuffer(void);
void InitPLL_SDRAM(void);
extern ERROR_CODE SetupForFlash(void);


//------------- m a i n ( ) ----------------//

int main(void)
{
	#ifdef INIT_PLL
		INIT_PLL();
	#endif

	PIO_MAINLOOP->DirSet(MAINLOOP);
	PIO_TST->DirSet(TST);

	SECTORLOCATION *pSectorInfo;
    ERROR_CODE Result;							// result

	spiFlash.Init();

	// get flash manufacturer & device codes, title & desc
	if( AFP_Error == NO_ERR )
	{
		AFP_Error = GetFlashInfo();
	};

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

	//spiFlash.EraseBlock(0);

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
			ErrorCode = spiFlash.EraseFlash(); //EraseFlash();
			break;

		// erase sector
		case FLASH_ERASE_SECT:
			ErrorCode = spiFlash.EraseBlock(AFP_Sector);
			break;

		// fill
		case FLASH_FILL:
			ErrorCode = FillData( AFP_Offset, AFP_Count, AFP_Stride, AFP_Buffer );
			break;

		// get manufacturer and device codes
		case FLASH_GET_CODES:
			ErrorCode = spiFlash.GetCodes(&AFP_ManCode, &AFP_DevCode);
			break;

		// get sector number based on address
		case FLASH_GET_SECTNUM:
			ErrorCode = spiFlash.GetSectorNumber(AFP_Offset, &AFP_Sector);
			break;

		// get sector number start and end offset
		case FLASH_GET_SECSTARTEND:
			ErrorCode = spiFlash.GetSectorStartEnd(&AFP_StartOff, &AFP_EndOff,  AFP_Sector);
			break;

		// read
		case FLASH_READ:
			ErrorCode = spiFlash.Read(AFP_Offset, AFP_Buffer, AFP_Count);
			break;

		// reset
		case FLASH_RESET:
			ErrorCode = spiFlash.ResetFlash();
			break;

		// write
		case FLASH_WRITE:
			ErrorCode = spiFlash.WriteSync((const byte*)AFP_Buffer, AFP_Offset, AFP_Count, false);
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
//	GET_SECTSTARTEND_STRUCT	SSectStartEnd;	//structure for GetSectStartEnd
	int i;									//index

	//initiate sector information structures
	for( i=0;i<AFP_NumSectors; i++)
	{
		//SSectStartEnd.nSectorNum = i;
		//SSectStartEnd.pStartOffset = &pSectInfo[i].ulStartOff;
		//SSectStartEnd.pEndOffset = &pSectInfo[i].ulEndOff;

		ErrorCode = spiFlash.GetSectorStartEnd( &pSectInfo[i].ulStartOff, &pSectInfo[i].ulEndOff, i);
		//ErrorCode = at25df021_Control( CNTRL_GET_SECSTARTEND, (COMMAND_STRUCT *)&SSectStartEnd  );
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
	ERROR_CODE ErrorCode = NO_ERR;		//return error code

	AFP_Title = pEzKitTitle;

	ErrorCode = spiFlash.GetCodes(&AFP_ManCode, &AFP_DevCode);
	ErrorCode = spiFlash.GetStr(&AFP_DeviceCompany, &AFP_Description);

	return ErrorCode;
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

//ERROR_CODE WriteData( unsigned long ulStart, long lCount, long lStride, int *pnData )
//{
//	// if the user wants to verify then do it
//
//	return at25df021_Write((byte*)pnData, ulStart, lCount, AFP_Verify);
//}

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

//ERROR_CODE ReadData( unsigned long ulStart, long lCount, long lStride, int *pnData )
//{
//	return at25df021_Read((byte*)pnData, ulStart, lCount);
//}


#endif // #ifndef AFP_IMP_H__07_08_2024__16_05
