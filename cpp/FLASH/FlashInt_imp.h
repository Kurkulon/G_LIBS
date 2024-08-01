#ifndef FLASHINT_IMP_H__24_02_2024__16_53
#define FLASHINT_IMP_H__24_02_2024__16_53

#pragma once

#include "core.h"
#include "FlashMem.h"
#include "CRC\crc16.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef CPU_SAME53	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	#define ISP_PAGESIZE 512
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

	#define ISP_PAGESIZE 256
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

#elif defined(CPU_LPC824)	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	#define PAGESIZE 64
	#define ISP_PAGESIZE PAGESIZE
	#define FRDY 1
	#define FCMDE 2
	#define FLOCKE 4
	#define PAGEDWORDS (PAGESIZE>>2)
	#define PAGES_IN_SECTOR 16
	#define SECTORSIZE (PAGESIZE*PAGES_IN_SECTOR)
	#define SECTORDWORDS (SECTORSIZE>>2)
	#define SECTORMASK (~(SECTORSIZE-1))

	u32 PLANESIZE = 0x1000;

	#ifndef BOOT_START_SECTOR
		#ifdef CPU_LPC824
			#define BOOT_START_SECTOR 8
		#elif defined(CPU_LPC812)
			#define BOOT_START_SECTOR 4
		#endif
	#endif

	#define START_PAGE (BOOT_START_SECTOR*PAGES_IN_SECTOR)

	#define BOOTSIZE (SECTORSIZE*BOOT_START_SECTOR)
	#define FLASH0 0x000000
	#define FLASH_START (FLASH0+BOOTSIZE)

	//#define FLASH1 (FLASH0+PLANESIZE)

	#define FLASH_END (FLASH0+PLANESIZE)
	#define FLASH_SIZE (FLASH_END-FLASH_START)

	#define IAP_LOCATION 0X1FFF1FF1
	static u32 command_param[5];
	static u32 status_result[4];
	typedef void (*IAP)(unsigned int [],unsigned int[]);
	#define iap_entry ((void(*)(u32[],u32[]))IAP_LOCATION)
	//#define iap_entry ((IAP)IAP_LOCATION);

	enum IAP_STATUS { CMD_SUCCESS = 0,  INVALID_COMMAND,  SRC_ADDR_ERROR,  DST_ADDR_ERROR,  SRC_ADDR_NOT_MAPPED,  DST_ADDR_NOT_MAPPED,  COUNT_ERROR,  INVALID_SECTOR,  SECTOR_NOT_BLANK, 
	 SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION, COMPARE_ERROR, BUSY, ERR_ISP_IRC_NO_POWER , ERR_ISP_FLASH_NO_POWER,  ERR_ISP_FLASH_NO_CLOCK  };

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	static bool IAP_PrepareSector(u32 sector)
	{
		sector += BOOT_START_SECTOR;

		command_param[0] = 50;
		command_param[1] = sector;
		command_param[2] = sector;
		command_param[3] = 0;

		__disable_irq(); iap_entry(command_param, status_result); __enable_irq();

		return status_result[0] == 0;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	static bool IAP_WritePage(u32 pagenum, __packed u32 *pbuf)
	{
		command_param[0] = 51;
		command_param[1] = FLASH_START + pagenum*PAGESIZE;
		command_param[2] = (u32)pbuf;
		command_param[3] = PAGESIZE;
		command_param[4] = MCK/1000;

		__disable_irq(); iap_entry(command_param, status_result); __enable_irq();

		return status_result[0] == 0;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	static bool IAP_EraseSector(u32 sector)
	{
		sector += BOOT_START_SECTOR;

		command_param[0] = 52;
		command_param[1] = sector;
		command_param[2] = sector;
		command_param[3] = MCK/1000;

		__disable_irq(); iap_entry(command_param, status_result); __enable_irq();

		return status_result[0] == 0;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class FlashInt : public FlashMem
{
protected:
	
	enum STATEWRFL { WRITE_WAIT = 0, WRITE_START, WRITE_ERASE_SECTOR, WRITE_PAGE, WRITE_PAGE_0, WRITE_OK, WRITE_ERROR, WRITE_FINISH, WRITE_INIT };
	STATEWRFL state_write_flash;

	u32 secStartAdr;
	u32 secEndAdr;
	u32 wadr;
	u32 wlen;
	u32 flash_write_error;
	u32 flash_write_ok;
	__packed u32 *wdata;

	FLWB *flwb;
	Ptr<MB> curFlwb;
	ListPtr<MB> writeFlBuf;

	bool REQ_WritePage(u32 pa, __packed u32 *pbuf, u32 len);
	bool REQ_EraseSector(u32 sa);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	bool IsFlashReady()
	{
	#ifdef CPU_SAME53
		return HW::NVMCTRL->STATUS & NVMCTRL_READY;
	#elif defined(CPU_XMC48)
		return (HW::FLASH0->FSR & FLASH_FSR_PBUSY_Msk) == 0;
	#endif
	}
	
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	bool IsFlashReqOK()
	{
	#ifdef CPU_SAME53
		return HW::NVMCTRL->INTFLAG == NVMCTRL_INTFLAG_DONE;
	#elif defined(CPU_XMC48)
		return (HW::FLASH0->FSR & (FLASH_FSR_VER_Msk|FLASH_FSR_SQER_Msk)) == 0;
	#endif
	}

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
	void	InitFlashWrite(){ state_write_flash = WRITE_INIT; }
	bool	Busy() { return (state_write_flash != WRITE_WAIT) || !writeFlBuf.Empty(); }
	bool	RequestWrite(Ptr<MB> &b);
	void	Init() { }
	u16		CRC16(u32 len, u32 *rlen);

	FlashInt() : FlashMem(ISP_PAGESIZE, FLASH_START), state_write_flash(WRITE_WAIT), secStartAdr(~0), secEndAdr(~0), flwb(0), wadr(0), wlen(0), 
					flash_write_error(0), flash_write_ok(0), wdata(0) {}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16 FlashInt::CRC16(u32 len, u32 *rlen)
{
	u16 crc;

	if (flashChecked)
	{
		crc = flashCRC;
		len = flashLen;
	}
	else
	{
		crc = GetCRC16((void*)FLASH_START, len);
	};

	if (rlen != 0) *rlen = len;

	return crc;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool FlashInt::RequestWrite(Ptr<MB> &b)
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

u32 FlashInt::GetSectorAdrLen(u32 sadr, u32 *radr)
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

#elif defined(CPU_LPC824)

	u32 len = SECTORSIZE;
	sadr -= BOOTSIZE;
	sadr &= SECTORMASK;

#endif

	if (len != 0 && radr != 0)
	{
		*radr = sadr;
	};

	return len;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32 FlashInt::Read(u32 addr, void *data, u32 size) 
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

	COPY((void*)addr, data, size);
	//CopyDataDMA((void*)addr, data, size);

	//while (!CheckDataComplete());

	return size;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


void FlashInt::Update()
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

bool FlashInt::REQ_WritePage(u32 pa, __packed u32 *pbuf, u32 len)
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

#elif defined(CPU_LPC824)

	if (!IAP_PrepareSector(pa/SECTORSIZE))
	{
		return false;
	};

	if (!IAP_WritePage(pa/PAGESIZE, pbuf))
	{
		return false;
	};

#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool FlashInt::REQ_EraseSector(u32 sa)
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

#elif defined(CPU_LPC824)

	if (!IAP_PrepareSector(sa/SECTORSIZE))
	{
		return false;
	};

	if (!IAP_EraseSector(sa/SECTORSIZE))
	{
		return false;
	};

#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // FLASHINT_IMP_H__24_02_2024__16_53
