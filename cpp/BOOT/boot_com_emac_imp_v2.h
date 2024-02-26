#ifndef BOOT_COM_EMAC_IMP_V2_H__23_02_2024__18_48
#define BOOT_COM_EMAC_IMP_V2_H__23_02_2024__18_48

//#include <string.h>
#include "time.h"
#include "list.h"
#include "CRC\crc16.h"
#include "SEGGER_RTT\SEGGER_RTT.h"
#include "BOOT\boot_isp.h"
#include "BOOT\boot_req.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef BOOT_HANDSHAKE_TIMEOUT		
#define BOOT_HANDSHAKE_TIMEOUT		(200)
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef BOOT_EMAC

#include <EMAC\tftp.h>

#ifndef TFTP_DATA_CHUNK_SIZE
#define TFTP_DATA_CHUNK_SIZE 512
#endif

#ifndef ISP_DATASIZE
#define ISP_DATASIZE TFTP_DATA_CHUNK_SIZE
#endif

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef BOOT_COM

	#include "ComPort\ComPort.h"

	#ifndef BOOT_COM_MODE
	#define BOOT_COM_MODE	ComPort::ASYNC
	#endif

	#ifndef BOOT_COM_STOPBITS
	#define BOOT_COM_STOPBITS	1
	#endif

	#ifdef BOOT_HANDSHAKE
		const u64 masterGUID = BOOT_MGUID;
		const u64 slaveGUID = BOOT_SGUID;
	#endif

	#ifndef ISP_DATASIZE
	#define ISP_DATASIZE ISP_PAGESIZE
	#endif

	#ifndef BOOT_COM_PRETIMEOUT
	#define BOOT_COM_PRETIMEOUT		MS2COM(200)
	#endif

	#ifndef BOOT_COM_POSTTIMEOUT
	#define BOOT_COM_POSTTIMEOUT	MS2COM(2)
	#endif

	#ifndef BOOT_COM_WRITEDELAY			
	#define BOOT_COM_WRITEDELAY			(US2CTM(10))
	#endif


	static u16 manReqWord = BOOT_MAN_REQ_WORD;
	static u16 manReqMask = BOOT_MAN_REQ_MASK;

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//#ifndef ISP_DATASIZE
//#define ISP_DATASIZE 512
//#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef BOOTLOADER_VERSION
#define BOOTLOADER_VERSION			0x0101
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef CPU_SAME53
#elif defined(CPU_XMC48)
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef SMALL_BUF_LEN
#define SMALL_BUF_LEN	((ISP_DATASIZE+64*2) & (~64))
#endif

#ifndef MEDIUM_BUF_LEN
#define MEDIUM_BUF_LEN	((ISP_DATASIZE+64*4) & (~64))
#endif

#ifndef HUGE_BUF_LEN
#define HUGE_BUF_LEN	((ISP_DATASIZE+64*8) & (~64))    
#endif

#ifndef NUM_SMALL_BUF
#define	NUM_SMALL_BUF	8       
#endif

#ifndef NUM_MEDIUM_BUF
#define	NUM_MEDIUM_BUF	8
#endif

#ifndef NUM_HUGE_BUF
#define	NUM_HUGE_BUF	8
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static TM32 tm64;
static u32 timeOut;

#ifdef BOOT_COM
static bool runCom = true;
#else
static bool runCom = false;
#endif

#ifdef BOOT_EMAC
static bool runEmac = true;
#else
static bool runEmac = false;
#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

typedef BootReqHS ReqHS;
typedef BootRspHS RspHS;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct ReqMes
{
	u32 len;

	BootReqV1 mes;

	u32 exdata[PAGEDWORDS];
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct RspMes
{
	u32 len;

	BootRspV1 mes;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef BOOT_V2

#ifdef CPU_SAME53	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	#define PAGEDWORDS (ISP_PAGESIZE>>2)
	#define PAGES_IN_SECTOR 16
	#define SECTORSIZE (ISP_PAGESIZE*PAGES_IN_SECTOR)
	#define SECTORDWORDS (SECTORSIZE>>2)
	#define SECTORMASK (~(SECTORSIZE-1))
	#define PLANESIZE 0x80000

	#ifndef BOOT_START_SECTOR
	#define BOOT_START_SECTOR 2
	#endif

	//#define START_SECTOR 4
	//#define START_PAGE (START_SECTOR*PAGES_IN_SECTOR)

	#define BOOTSIZE (SECTORSIZE*BOOT_START_SECTOR)
	#define FLASH0_ADR 0x00000000
	#define FLASH_START (FLASH0_ADR+BOOTSIZE)

	//#define FLASH1 (FLASH0+PLANESIZE)

	#define FLASH_END (FLASH0_ADR+PLANESIZE)

	#define MEMCOPY_DMACH 0


	//const u32 sectorAdr[] = {0x010000, 0x020000, 0x040000, 0x080000, 0x0C0000, 0x100000, 0x140000, 0x180000, 0x1C0000, 0x200000 };

#elif defined(CPU_XMC48)	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	#define FRDY 1
	#define FCMDE 2
	#define FLOCKE 4
	#define PAGEDWORDS (ISP_PAGESIZE>>2)
	//#define PAGES_IN_SECTOR 64
	//#define SECTORSIZE (ISP_PAGESIZE*PAGES_IN_SECTOR)
	//#define SECTORDWORDS (SECTORSIZE>>2)
	#define PLANESIZE 0x200000
	//#define START_SECTOR 4
	//#define START_PAGE (START_SECTOR*PAGES_IN_SECTOR)

	#define BOOTSIZE 0x10000	//(SECTORSIZE*START_SECTOR)
	#define FLASH0_ADR 0x0C000000
	#define FLASH_START (FLASH0_ADR+BOOTSIZE)

	//#define FLASH1 (FLASH0+PLANESIZE)

	#define FLASH_END (FLASH0_ADR+PLANESIZE)

	const u32 sectorAdr[] = {0x010000, 0x020000, 0x040000, 0x080000, 0x0C0000, 0x100000, 0x140000, 0x180000, 0x1C0000, 0x200000 };

	volatile u32* const _CMD_553C = (u32*)(FLASH0_ADR + 0x553C);
	volatile u32* const _CMD_5554 = (u32*)(FLASH0_ADR + 0x5554);
	volatile u32* const _CMD_55F0 = (u32*)(FLASH0_ADR + 0x55F0);
	volatile u32* const _CMD_55F4 = (u32*)(FLASH0_ADR + 0x55F4);
	volatile u32* const _CMD_AAA8 = (u32*)(FLASH0_ADR + 0xAAA8);

	static void CMD_ResetToRead()		{ *_CMD_5554 = 0xF0; }
	static void CMD_EnterPageMode()		{ *_CMD_5554 = 0x50; }
	static void CMD_ClearStatus()		{ *_CMD_5554 = 0xF5; }
	static void CMD_ResumeProtection()	{ *_CMD_5554 = 0x5E; }

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	static void CMD_LoadPage(__packed const u32* data, u16 len)
	{ 
		u16 l = len&1;

		len >>= 1;

		while (len > 0)
		{
			*_CMD_55F0 = *(data++);
			*_CMD_55F4 = *(data++);
			len -= 1;
		};

		if (l)
		{
			*_CMD_55F0 = *(data++);
			*_CMD_55F4 = 0;
		};
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	static void CMD_WritePage(u32 pa)
	{
		*_CMD_5554 = 0xAA; *_CMD_AAA8 = 0x55; *_CMD_5554 = 0xA0; *((u32*)(FLASH_START+pa)) = 0xAA;
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	static void CMD_ErasePhysicalSector(u32 sa)
	{
		*_CMD_5554 = 0xAA; *_CMD_AAA8 = 0x55; *_CMD_5554 = 0x80; *_CMD_5554 = 0xAA; *_CMD_AAA8 = 0x55; *((u32*)(FLASH_START+sa)) = 0x40;
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#endif	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++




enum STATEWRFL { WRITE_WAIT = 0, WRITE_START, WRITE_ERASE_SECTOR, WRITE_PAGE, WRITE_PAGE_0, WRITE_OK, WRITE_ERROR, WRITE_FINISH, WRITE_INIT };

STATEWRFL state_write_flash = WRITE_WAIT;
u32 flash_write_error = 0;
u32 flash_write_ok = 0;

//static MEMB memBuffer[64];

//static List<MEMB> freeMemBuf;
static ListPtr<MB> writeFlBuf;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32	ISP_GetFlashWriteError()
{
	return flash_write_error;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32	ISP_GetFlashWriteOK()
{
	return flash_write_ok;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16	ISP_GetBootloaderVersion()
{
	return BOOTLOADER_VERSION;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool ISP_RequestFlashWrite(Ptr<MB> &b)
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

#pragma push
#pragma O3
#pragma Otime

static void CopyDataDMA(volatile void *src, volatile void *dst, u16 len)
{
	using namespace HW;

#ifdef CPU_SAME53

	DmaTable[MEMCOPY_DMACH].SRCADDR = (byte*)src+len;
	DmaTable[MEMCOPY_DMACH].DSTADDR = (byte*)dst+len;
	DmaTable[MEMCOPY_DMACH].DESCADDR = 0;
	DmaTable[MEMCOPY_DMACH].BTCNT = len;
	DmaTable[MEMCOPY_DMACH].BTCTRL = DMDSC_VALID|DMDSC_BEATSIZE_BYTE|DMDSC_DSTINC|DMDSC_SRCINC;

	DMAC->CH[MEMCOPY_DMACH].INTENCLR = ~0;
	DMAC->CH[MEMCOPY_DMACH].INTFLAG = ~0;
	DMAC->CH[MEMCOPY_DMACH].CTRLA = DMCH_ENABLE|DMCH_TRIGACT_TRANSACTION;
	DMAC->SWTRIGCTRL = 1<<MEMCOPY_DMACH;

#elif defined(CPU_XMC48)

	register u32 t __asm("r0");

	HW::GPDMA1->DMACFGREG = 1;

	HW::GPDMA1_CH3->CTLL = DST_INC|SRC_INC|TT_FC(0)|DEST_MSIZE(0)|SRC_MSIZE(0);
	HW::GPDMA1_CH3->CTLH = BLOCK_TS(len);

	HW::GPDMA1_CH3->SAR = (u32)src;
	HW::GPDMA1_CH3->DAR = (u32)dst;
	HW::GPDMA1_CH3->CFGL = 0;
	HW::GPDMA1_CH3->CFGH = PROTCTL(1);

	HW::GPDMA1->CHENREG = 0x101<<3;

#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool CheckDataComplete()
{
#ifdef CPU_SAME53
	return (HW::DMAC->CH[MEMCOPY_DMACH].CTRLA & DMCH_ENABLE) == 0 || (HW::DMAC->CH[MEMCOPY_DMACH].INTFLAG & DMCH_TCMPL);
#elif defined(CPU_XMC48)
	return (HW::GPDMA1->CHENREG & (1<<3)) == 0;
#endif
}

#pragma pop

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static u32 GetSectorAdrLen(u32 sadr, u32 *radr)
{
	sadr += BOOTSIZE;

	if (sadr >= PLANESIZE)
	{
		return 0;
	};


#ifdef CPU_SAME53

	u32 len = SECTORSIZE;
	sadr -= BOOTSIZE;
	sadr &= SECTORMASK;

#elif defined(CPU_XMC48)

	u32 len = 0;

	for (u32 i = 0; i < (ArraySize(sectorAdr)-1); i++)
	{
		if (sadr >= sectorAdr[i] && sadr < sectorAdr[i+1])
		{
			sadr = sectorAdr[i] - BOOTSIZE;
			len = sectorAdr[i+1] - sectorAdr[i];

			break;
		};
	};

#endif

	if (len != 0 && radr != 0)
	{
		*radr = sadr;
	};

	return len;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef CPU_XMC48

#endif

//static ComPort com;

//static ComPort* actCom = 0;


//struct FL
//{
//	u32 id;
//	u32 size;
//	u32 pageSize;
//	u32 nbPlane;
//	u32 planeSize;
//};

//static FL flDscr;

//static bool run = false;

static bool IsFlashReady()
{
#ifdef CPU_SAME53
	return HW::NVMCTRL->STATUS & NVMCTRL_READY;
#elif defined(CPU_XMC48)
	return (HW::FLASH0->FSR & FLASH_FSR_PBUSY_Msk) == 0;
#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool IsFlashReqOK()
{
#ifdef CPU_SAME53
	return HW::NVMCTRL->INTFLAG == NVMCTRL_INTFLAG_DONE;
#elif defined(CPU_XMC48)
	return (HW::FLASH0->FSR & (FLASH_FSR_VER_Msk|FLASH_FSR_SQER_Msk)) == 0;
#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool REQ_WritePage(u32 pa, __packed u32 *pbuf, u32 len)
{
	len = (len+3) >> 2;

	u32 count = (len < PAGEDWORDS) ? len : PAGEDWORDS;

#ifdef CPU_SAME53

	while (!IsFlashReady());

	HW::NVMCTRL->CTRLB = NVMCTRL_CMD_PBC;

	while (!IsFlashReady());

	HW::NVMCTRL->INTFLAG = ~0;

	u32 *p = (u32*)(FLASH_START + pa);


	while((count--) > 0)
	{
		*(p++) = *(pbuf++);
	};

	HW::NVMCTRL->CTRLB = NVMCTRL_CMD_WP;

	return true;

#elif defined(CPU_XMC48)

	CMD_ResetToRead();

	CMD_ClearStatus();

	CMD_EnterPageMode();

	while ((HW::FLASH0->FSR & (FLASH_FSR_PFPAGE_Msk|FLASH_FSR_SQER_Msk)) == 0);// { HW::WDT->Update(); };

	CMD_LoadPage(pbuf, count);

	CMD_WritePage(pa);

	while ((HW::FLASH0->FSR & (FLASH_FSR_PROG_Msk|FLASH_FSR_SQER_Msk)) == 0);// { HW::WDT->Update(); };

	return (HW::FLASH0->FSR & (FLASH_FSR_PROG_Msk|FLASH_FSR_SQER_Msk)) == FLASH_FSR_PROG_Msk;

#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool REQ_EraseSector(u32 sa)
{
#ifdef CPU_SAME53

	HW::NVMCTRL->INTFLAG = ~0;

	HW::NVMCTRL->ADDR = FLASH_START+sa;
	HW::NVMCTRL->CTRLB = NVMCTRL_CMD_EB;

	return true;

#elif defined(CPU_XMC48)
	CMD_ResetToRead();
	
	CMD_ClearStatus();

	CMD_ErasePhysicalSector(sa);

	while ((HW::FLASH0->FSR & (FLASH_FSR_ERASE_Msk|FLASH_FSR_SQER_Msk)) == 0);// { HW::WDT->Update(); };

	return (HW::FLASH0->FSR & (FLASH_FSR_ERASE_Msk|FLASH_FSR_SQER_Msk)) == FLASH_FSR_ERASE_Msk;
#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32 ISP_Flash_Read(u32 addr, byte *data, u32 size) 
{
#ifdef CPU_SAME53
#elif defined(CPU_XMC48)
	while (HW::FLASH0->FSR & FLASH_FSR_PFPAGE_Msk);
#endif

	addr += FLASH_START;

	if (addr >= FLASH_END)
	{
		return 0;
	};

	if ((addr + size) >= FLASH_END)
	{
		size = FLASH_END - addr;	
	};

	CopyDataDMA((void*)addr, data, size);

	while (!CheckDataComplete());

	return size;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void ISP_InitFlashWrite()
{
	state_write_flash = WRITE_INIT;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool FlashBusy()
{
	return (state_write_flash != WRITE_WAIT) || !writeFlBuf.Empty();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void UpdateWriteFlash()
{
	static u32 secStartAdr = ~0;
	static u32 secEndAdr = ~0;
	static Ptr<MB> curFlwb;
	static FLWB *flwb = 0;
	static u32 wadr = 0;
	static u32 wlen = 0;
	static __packed u32 *wdata = 0;

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
#else // #ifdef BOOT_V2

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32 ISP_Flash_Read(u32 addr, byte *data, u32 size) 
{
	return bootFlash.Read(addr, data, size);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void ISP_InitFlashWrite()
{
	bootFlash.InitFlashWrite();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32	ISP_GetFlashWriteError()
{
	return bootFlash.Get_WriteError();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32	ISP_GetFlashWriteOK()
{
	return bootFlash.Get_WriteOK();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16	ISP_GetBootloaderVersion()
{
	return BOOTLOADER_VERSION;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool ISP_RequestFlashWrite(Ptr<MB> &b)
{
	return bootFlash.RequestWrite(b);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // #else // #ifdef BOOT_V2

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef BOOT_HANDSHAKE

static bool HandShake()
{
	#ifdef BOOT_COM

		static ReqHS req;
		static RspHS rsp;

		static ComPort::WriteBuffer wb = { false, sizeof(req), &req };

		static ComPort::ReadBuffer rb = { false, 0, sizeof(rsp), &rsp };

		req.guid = slaveGUID;
		req.crc = GetCRC16(&req, sizeof(req)-2);

	#endif

	static byte i = 0;

	static TM32 tm;

	bool c = false;

	tm.Reset();

	runCom = false;
	runEmac = false;

	SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_CYAN "Start handshake ...\n");

	while (!tm.Check(BOOT_HANDSHAKE_TIMEOUT) && !c)
	{
		Pin_MainLoop_Tgl();

		HW::WDT->Update();

		#ifdef BOOT_COM

			switch (i)
			{
				case 0:

					com.Read(&rb, BOOT_HANDSHAKE_PRETIMEOUT, BOOT_HANDSHAKE_POSTTIMEOUT);

					i++;

					break;

				case 1:

					if (!com.Update())
					{
						if (rb.recieved && rb.len == sizeof(RspHS) && GetCRC16(rb.data, rb.len) == 0 && rsp.guid == masterGUID)
						{
							com.Write(&wb);

							i++;
						}
						else
						{
							i = 0;
						};
					};

					break;

				case 2:

					if (!com.Update())
					{
						runCom = c = true;

						timeOut.Reset();
					};

					break;
			};

		#endif

		#ifdef BOOT_EMAC

			runEmac = c;

			UpdateEMAC();

			if (EmacIsEnergyDetected() && EmacIsCableNormal())
			{
				runEmac = c = true;
				tm64.Reset();

				SEGGER_RTT_printf(0, RTT_CTRL_TEXT_BRIGHT_GREEN "Emac connected - %u ms\n", GetMilliseconds());
			};

		#endif
	};

	return c;
}

#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef ADSP_BLACKFIN

static void _MainAppStart(u32 adr)
{
	if (!bootFlash.flashChecked) bootFlash.ADSP_CheckFlash();

	if (bootFlash.flashOK && bootFlash.flashCRCOK) HW::WDT->Disable(), bfrom_SpiBoot(FLASH_START_ADR, BFLAG_PERIPHERAL | BFLAG_NOAUTO | BFLAG_FASTREAD | BFLAG_TYPE3 | 7, 0, 0);
	
	tm64.Reset(); timeOut = MS2CTM(1000);
}

#else

extern "C" void _MainAppStart(u32 adr);

#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef BOOT_COM

typedef FlashMem::FLWB FLWB;

static bool Request_00_GetInfo(ReqMes &req, RspMes &rsp)
{
	rsp.len = 0;

	BootReqV1::SF0 &rq = req.mes.F0;
	BootRspV1::SF0 &rp = rsp.mes.F0;

	if (rq.adr == 0 || req.len < sizeof(rq)) return true;

	rp.adr		= rq.adr;
	rp.rw		= rq.rw;
	rp.ver		= req.mes.VERSION;
	rp.maxFunc	= req.mes.FUNC_MAX;
	rp.guid		= BOOT_SGUID;
	rp.startAdr = FLASH_START;
	rp.pageLen	= ISP_PAGESIZE;
	rp.crc		= GetCRC16(&rp, sizeof(rp)-sizeof(rp.crc));

	rsp.len = sizeof(rp);

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool Request_01_GetCRC(ReqMes &req, RspMes &rsp)
{
	rsp.len = 0;

	BootReqV1::SF1 &rq = req.mes.F1;
	BootRspV1::SF1 &rp = rsp.mes.F1;

	if (rq.adr == 0) return true;

#ifdef CPU_XMC48
	bool c = (HW::FLASH0->FSR & FLASH_FSR_PFPAGE_Msk) == 0;
#else
	bool c = true;
#endif

	if (req.len == sizeof(rq) && c)
	{
		if (rq.len != 0)
		{
			rp.flashCRC = bootFlash.CRC16(rq.len, &rp.flashLen); //GetCRC16((void*)(FLASH_START), rq.len);
			//rp.flashLen = rq.len;
		};
	}
	else
	{
		rp.flashCRC = 0;
		rp.flashLen = 0;
	};

	rp.adr		= rq.adr;
	rp.rw		= rq.rw;
	rp.crc		= GetCRC16(&rp, sizeof(rp)-sizeof(rp.crc));

	rsp.len = sizeof(rp);

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool Request_02_WritePage(Ptr<MB> &mb, RspMes &rsp)
{
	FLWB &flwb = *((FLWB*)mb->GetDataPtr());
	ReqMes &req = *((ReqMes*)flwb.data);

	BootReqV1::SF2 &rq = req.mes.F2;
	BootRspV1::SF2 &rp = rsp.mes.F2;

	rsp.len = 0;

	u16 xl = rq.plen + sizeof(rq) - sizeof(rq.pdata);

	bool c = false;

	if (req.len >= xl /*&& flash_write_error == 0*/)
	{
		flwb.adr		= rq.padr;
		flwb.dataLen	= rq.plen;
		flwb.dataOffset = (byte*)rq.pdata - flwb.data;
		
		c = ISP_RequestFlashWrite(mb);
	};

	if (rq.adr == 0) return true;

	rp.adr		= rq.adr;
	rp.rw		= rq.rw;
	rp.res		= c;
	rp.crc		= GetCRC16(&rp, sizeof(rp)-sizeof(rp.crc));
	rsp.len		= sizeof(rp);

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool Request_03_ExitBootLoader(ReqMes &req, RspMes &rsp)
{
	BootReqV1::SF3 &rq = req.mes.F3;
	BootRspV1::SF3 &rp = rsp.mes.F3;

	rsp.len = 0;

	if (rq.adr == 0) return false;

	rp.adr		= rq.adr;
	rp.rw		= rq.rw;
	rp.crc		= GetCRC16(&rp, sizeof(rp)-sizeof(rp.crc));
	rsp.len		= sizeof(rp);

	return false;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool Request_04_SetTimeOut(ReqMes &req, RspMes &rsp)
{
	BootReqV1::SF4 &rq = req.mes.F4;
	BootRspV1::SF4 &rp = rsp.mes.F4;

	timeOut = rq.timeOutMS;
	tm64.Reset();

	rsp.len = 0;

	if (rq.adr == 0) return true;

	rp.adr		= rq.adr;
	rp.rw		= rq.rw;
	rp.crc		= GetCRC16(&rp, sizeof(rp)-sizeof(rp.crc));
	rsp.len		= sizeof(rp);

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestHandler(Ptr<MB> &mb, RspMes &rsp)
{
	FLWB &flwb = *((FLWB*)(mb->GetDataPtr()));
	ReqMes &req = *((ReqMes*)flwb.data);
	BootReqV1::SF0 &rq = req.mes.F0;

	if (req.len < (sizeof(rq.adr)+sizeof(rq.rw))) return true;

	u16 t = rq.rw;
	u16 adr = GetNetAdr();

	bool cm = (t & manReqMask) == manReqWord;
	bool ca = rq.adr == adr || rq.adr == 0;

	if (cm && adr <= BOOT_MAX_NETADR)
	{
		#if defined(BOOT_TIMEOUT) && defined(BOOT_MAIN_TIMEOUT)
			tm64.Reset();
			if (timeOut < BOOT_MAIN_TIMEOUT) timeOut = BOOT_MAIN_TIMEOUT;
		#endif
	}
	else
	{
		return true;
	};
	
	bool c = false;

	t &= 0xFF;

	switch (t)
	{
		case 0: c = Request_00_GetInfo(req, rsp);			break;
		case 1: c = Request_01_GetCRC(req, rsp);			break;
		case 2: c = Request_02_WritePage(mb, rsp);			break;
		case 3: c = Request_03_ExitBootLoader(req, rsp);	break;
		case 4: c = Request_04_SetTimeOut(req, rsp);		break;
	};

	return c;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void UpdateCom()
{
	static ComPort::WriteBuffer wb;
	static ComPort::ReadBuffer rb;

//	static ReqMes req;
	static RspMes rsp;

	static byte i = 0;

	static bool c = true;

	static CTM32 tm;

	static Ptr<MB> mb;
	static ReqMes *req = 0;

	switch (i)
	{
		case 0:

			mb = AllocMemBuffer(sizeof(ReqMes));

			if (mb.Valid())
			{
				FLWB &flwb = *((FLWB*)mb->GetDataPtr());
				req = (ReqMes*)flwb.data;
				i++;
			};

			break;

		case 1:

			rb.data = &req->mes;
			rb.maxLen = sizeof(*req) - sizeof(req->len);
			
			com.Read(&rb, BOOT_COM_PRETIMEOUT, BOOT_COM_POSTTIMEOUT);

			i++;

			break;

		case 2:

			if (!com.Update())
			{
				if (rb.recieved && rb.len > 0 && GetCRC16(rb.data, rb.len) == 0)
				{
					req->len = rb.len;

					c = RequestHandler(mb, rsp);
					
					tm.Reset();

					i++;
				}
				else
				{
					#ifdef BOOT_COM_ERROR_TIMEOUT
						if (rb.recieved) timeOut = BOOT_COM_ERROR_TIMEOUT;
					#endif

					i = 4;
				};
			};

			break;

		case 3:

			if (tm.Check(BOOT_COM_WRITEDELAY))
			{
				wb.data = &rsp.mes;
				wb.len = rsp.len;

				com.Write(&wb);

				i++;
			};

			break;

		case 4:

			if (!com.Update())
			{
				mb.Free();
				req = 0;
				i = 0;

				runCom = c;
			};

			break;
	};
}

#endif // BOOT_COM
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void WDT_Init()
{
	#ifdef CPU_SAME53	

		HW::MCLK->APBAMASK |= APBA_WDT;

		HW::WDT->CONFIG = WDT_WINDOW_CYC512|WDT_PER_CYC1024;
	
		#ifndef _DEBUG
		HW::WDT->CTRLA = WDT_ENABLE;
		#else
		HW::WDT->CTRLA = 0;
		#endif

		while(HW::WDT->SYNCBUSY);

	#elif defined(CPU_XMC48)

		HW::WDT_Enable();

		HW::WDT->WLB = OFI_FREQUENCY/2;
		HW::WDT->WUB = (3 * OFI_FREQUENCY)/2;
		HW::SCU_CLK->WDTCLKCR = 0|SCU_CLK_WDTCLKCR_WDTSEL_OFI;

		#ifndef _DEBUG
		HW::WDT->CTR = WDT_CTR_ENB_Msk|WDT_CTR_DSP_Msk;
		#else
		HW::WDT->CTR = WDT_CTR_ENB_Msk;
		#endif

	#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int main()
{
	#ifdef BOOT_START_BREAKPOINT
		__breakpoint(0);
	#endif

	SEGGER_RTT_Init();
	SEGGER_RTT_WriteString(0, RTT_CTRL_CLEAR);
	SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_YELLOW "Bootloader Start ...\n");

	#ifdef BOOT_EMAC
		ResetPHY();
	#endif

	Init_time(MCK);
	WDT_Init();

	bootFlash.Init();

	#ifdef BOOT_EMAC
		InitEMAC();
	#endif

	#ifdef BOOT_COM
		com.Connect(BOOT_COM_MODE, BOOT_COM_SPEED, BOOT_COM_PARITY, BOOT_COM_STOPBITS);
	#endif

	#ifdef BOOT_HANDSHAKE
		HandShake();
	#endif

	CTM32	tm;

	tm64.Reset();
	
	#ifdef BOOT_TIMEOUT
		 timeOut = BOOT_TIMEOUT;
	#endif

	while(runCom || runEmac)
	{
		bootFlash.Update();

		#ifdef BOOT_COM
			if (runCom) UpdateCom();
		#else
				runCom = false;
		#endif

		#ifdef BOOT_EMAC

			if (runEmac)
			{
				UpdateEMAC();
				runEmac = TFTP_Idle();

				if (!TFTP_Connected() && tm64.Check(10000))
				{
					runEmac = false;
				};
			};

		#else
				runEmac = false;
		#endif

		#ifdef BOOT_TIMEOUT
			if (tm64.Timeout(timeOut)) break;
		#endif

		HW::WDT->Update();

		if(tm.Check(MS2CTM(50))) Pin_MainLoop_Tgl();
	};

	while (bootFlash.Busy()) bootFlash.Update();

	#ifdef BOOT_EXIT_BREAKPOINT
		__breakpoint(0);
	#endif

	__disable_irq();

	#ifdef CORTEX_M4
		CM4::SysTick->CTRL = 0;
	#endif

	#ifdef BOOT_EMAC
		ResetPHY();
	#endif

	#ifdef CPU_SAME53

		HW::RTC->CTRLA = RTC_SWRST;
		HW::GMAC->NCR = 0;
		HW::GMAC->NCFGR = 0x80000;
		HW::DMAC->CTRL = DMAC_SWRST;
		HW::GCLK->CTRLA = GCLK_SWRST;
		HW::WDT->Disable();
		HW::MCLK->APBAMASK &= ~APBA_WDT;

	#elif defined(CPU_XMC48)

		HW::Peripheral_Disable(PID_DMA0);
		HW::Peripheral_Disable(PID_DMA1);

		HW::Peripheral_Disable(PID_USIC0);
		HW::Peripheral_Disable(PID_USIC1);

		HW::WDT_Disable();

	#endif

	SEGGER_RTT_printf(0, RTT_CTRL_TEXT_BRIGHT_GREEN "Main App Start ... %u ms\n", GetMilliseconds());

	_MainAppStart(FLASH_START);

	return FLASH_START;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include <MEM\mem_imp.h>

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef BOOT_EMAC

#include <EMAC\emac_imp_v2.h>
#include <EMAC\tftp_imp.h>

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // BOOT_COM_EMAC_IMP_V2_H__23_02_2024__18_48
