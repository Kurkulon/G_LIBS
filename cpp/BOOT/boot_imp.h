#ifndef BOOT_IMP_H__14_11_2022__12_13
#define BOOT_IMP_H__14_11_2022__12_13

#include "core.h"

#include "ComPort\ComPort.h"
#include "CRC\CRC16.h"
#include "FLASH\at25df021.h"
#include "BOOT\boot_req.h"

static bool run = true;

static ComPort com;

static u16 manReqWord = BOOT_MAN_REQ_WORD;
static u16 manReqMask = BOOT_MAN_REQ_MASK;

//static u16 numDevice = 1;
//static u16 verDevice = 0x101;

static u32 manCounter = 0;
static u32 err06 = 0;


//static bool runMainApp = false;

static u16 flashCRC = 0;
static u32 flashLen = 0;
static u16 lastErasedBlock = ~0;
static bool flashOK = false;
static bool flashChecked = false;
static bool flashCRCOK = false;
//static bool cmdRunMainApp = false;

#ifdef BOOT_TIMEOUT
static CTM64 tm64;
static u64 timeOut = MS2CTM(500);
#endif

static void CheckFlash();

static u32 curWriteReqAdr = 0;

//static byte buf[SECTOR_SIZE];

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//#ifdef BOOT_NETADR
//
//extern u16 GetNetAdr();
//
//struct  ReqDsp05 { u16 adr; u16 rw; u16 crc; };												// запрос контрольной суммы и длины программы во флэш-памяти
//struct  ReqDsp06 { u16 adr; u16 rw; u16 stAdr; u16 len; byte data[256]; u16 crc; };			// запись страницы во флэш
//struct  ReqDsp07 { u16 adr; u16 rw; word crc; };											// перезагрузить блэкфин
//
//struct  RspDsp05 { u16 adr; u16 rw; u16 flashLen; u32 startAdr; u16 flashCRC; u16 crc; };	// запрос контрольной суммы и длины программы во флэш-памяти
//struct  RspDsp06 { u16 adr; u16 rw; u16 res; u16 crc; };									// запись страницы во флэш
//
//#else
//
//struct  ReqDsp05 { u16 rw; u16 crc; };												// запрос контрольной суммы и длины программы во флэш-памяти
//struct  ReqDsp06 { u16 rw; u16 stAdr; u16 len; byte data[256]; u16 crc; };			// запись страницы во флэш
//struct  ReqDsp07 { u16 rw; word crc; };												// перезагрузить блэкфин
//
//struct  RspDsp05 { u16 rw; u16 flashLen; u32 startAdr; u16 flashCRC; u16 crc; };	// запрос контрольной суммы и длины программы во флэш-памяти
//struct  RspDsp06 { u16 rw; u16 res; u16 crc; };										// запись страницы во флэш
//
//#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void RunMainApp()
{
	if (!flashChecked) CheckFlash();

	if (flashOK && flashCRCOK) HW::DisableWDT(), bfrom_SpiBoot(FLASH_START_ADR, BFLAG_PERIPHERAL | BFLAG_NOAUTO | BFLAG_FASTREAD | BFLAG_TYPE3 | 7, 0, 0);
	
	tm64.Reset(); timeOut = MS2CTM(1000);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestFunc_00(ReqAT25 *r, ComPort::WriteBuffer *wb)
{
	const BootReqV1 &req = *((BootReqV1*)(r->GetDataPtr()));
	static BootRspV1 rsp;

	if (req.F0.adr == 0 || r->len < sizeof(req.F0)) return  false;

	rsp.F0.adr		= req.F0.adr;
	rsp.F0.rw		= req.F0.rw;
	rsp.F0.ver		= rsp.VERSION;
	rsp.F0.maxFunc	= rsp.FUNC_MAX;
	rsp.F0.guid		= BOOT_SGUID;
	rsp.F0.startAdr = FLASH_START_ADR;
	rsp.F0.pageLen	= FLASH_PAGE_SIZE;

	rsp.F0.crc = GetCRC16(&rsp.F0, sizeof(rsp.F0)-sizeof(rsp.F0.crc));

	wb->data = &rsp.F0;
	wb->len = sizeof(rsp.F0);

	FreeReqAT25(r);

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestFunc_01(ReqAT25 *r, ComPort::WriteBuffer *wb)
{
	const BootReqV1 &req = *((BootReqV1*)(r->GetDataPtr()));
	static BootRspV1 rsp;

	if (req.F1.adr == 0 || r->len < sizeof(req.F1)) return  false;

	rsp.F1.adr		= req.F1.adr;
	rsp.F1.rw		= req.F1.rw;
	rsp.F1.flashLen	= flashLen;
	rsp.F1.flashCRC = flashCRC;

	rsp.F1.crc = GetCRC16(&rsp.F1, sizeof(rsp.F1)-sizeof(rsp.F1.crc));

	wb->data = &rsp.F1;
	wb->len = sizeof(rsp.F1);

	FreeReqAT25(r);

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestFunc_02(ReqAT25 *r, ComPort::WriteBuffer *wb)
{
	const BootReqV1 &req = *((BootReqV1*)(r->GetDataPtr()));
	static BootRspV1 rsp;

	u16 xl = req.F2.plen + sizeof(req.F2) - sizeof(req.F2.pdata);

	if (r->len < xl) return  false;

	u16 adr = req.F2.adr;

	if (req.F2.padr >= curWriteReqAdr)
	{
		curWriteReqAdr = req.F2.padr + req.F2.plen;
	
		r->dataOffset = (byte*)req.F2.pdata - r->data;
		r->stAdr = req.F2.padr;
		r->len = req.F2.plen;

		FlashWriteReq(r);
	}
	else
	{
		FreeReqAT25(r);
	};

	if (adr == 0) return false;

	rsp.F2.adr	= req.F2.adr;
	rsp.F2.rw	= req.F2.rw;
	rsp.F2.res	= GetLastError();

	rsp.F2.crc = GetCRC16(&rsp.F2, sizeof(rsp.F2)-sizeof(rsp.F2.crc));

	wb->data = &rsp.F2;
	wb->len = sizeof(rsp.F2);

	flashChecked = flashOK = flashCRCOK = false;

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestFunc_03(ReqAT25 *r, ComPort::WriteBuffer *wb)
{
	const BootReqV1 &req = *((BootReqV1*)(r->GetDataPtr()));
	static BootRspV1 rsp;

	if (r->len < sizeof(req.F3)) return  false;

	run = false;

	if (req.F3.adr == 0) return  false;

	rsp.F3.adr		= req.F3.adr;
	rsp.F3.rw		= req.F3.rw;

	rsp.F3.crc = GetCRC16(&rsp.F3, sizeof(rsp.F3)-sizeof(rsp.F3.crc));

	wb->data = &rsp.F3;
	wb->len = sizeof(rsp.F3);

	FreeReqAT25(r);

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestFunc(ReqAT25 *r, ComPort::WriteBuffer *wb)
{
	bool result = false;

	BootReqV1 &req = *((BootReqV1*)(r->GetDataPtr()));

	u16 t = req.F0.rw;
	u16 adr = GetNetAdr();

	bool cm = (t & manReqMask) == manReqWord;
	bool ca = req.F0.adr == adr || req.F0.adr == 0;

	#if defined(BOOT_TIMEOUT) && defined(BOOT_MAIN_TIMEOUT)
		if (cm)	tm64.Reset(), timeOut = BOOT_MAIN_TIMEOUT;
	#endif
	
	if (!cm || adr > BOOT_MAX_NETADR) run = false, RunMainApp();

	if (!ca || !cm || r->len < 2)
	{
		FreeReqAT25(r);
		return false;
	};

	manCounter += 1;

	t &= 0xFF;

	switch (t)
	{
		case 0: 	result = RequestFunc_00(r, wb); break;
		case 1: 	result = RequestFunc_01(r, wb); break;
		case 2: 	result = RequestFunc_02(r, wb); break;
		case 3: 	result = RequestFunc_03(r, wb); break;

		default:	FreeReqAT25(r); run = false; RunMainApp(); break;
	};

	//if (result)	tm64.Reset(), timeOut = MS2CTM(10000);

	return result;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void UpdateBlackFin()
{
	static byte i = 0;
	static ComPort::WriteBuffer wb;
	static ComPort::ReadBuffer rb;
	
	static ReqAT25 *req = 0;

	ResetWDT();

	switch(i)
	{
		case 0:

			req = AllocReqAT25();

			if (req != 0)
			{
				req->len = 0;
				req->dataOffset = 0;
				rb.data = req->GetDataPtr();
				rb.maxLen = req->MaxLen();
				com.Read(&rb, BOOT_COM_PRETIMEOUT, BOOT_COM_POSTTIMEOUT);
				i++;
			};

			break;

		case 1:

			if (!com.Update())
			{
				if (rb.recieved && rb.len > 0 && GetCRC16(rb.data, rb.len) == 0)
				{
					req->len = rb.len;

					if (RequestFunc(req, &wb))
					{
						com.Write(&wb);
						i++;
					}
					else
					{
						req = 0;

						i = 0;
					};
				}
				else
				{
					#ifdef BOOT_COM_ERROR_TIMEOUT
						if (rb.recieved) timeOut = BOOT_COM_ERROR_TIMEOUT;
					#endif

					FreeReqAT25(req);

					req = 0;

					i = 0;
				};
			};

			break;

		case 2:

			if (!com.Update())
			{
				i = 0;
			};

			break;
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef BOOT_HANDSHAKE

const u64 masterGUID	= BOOT_MGUID;
const u64 slaveGUID		= BOOT_SGUID;

#pragma pack(1)
struct ReqHS { u64 guid; u16 crc; };
struct RspHS { u64 guid; u16 crc; };
#pragma pack()

bool HandShake()
{
	static ReqHS req;
	static RspHS rsp;

	static ComPort::WriteBuffer wb = { false, sizeof(req), &req };

	static ComPort::ReadBuffer rb = { false, 0, sizeof(rsp), &rsp };

	req.guid = slaveGUID;
	req.crc = GetCRC16(&req, sizeof(req)-2);

	bool c = false;

	for (byte i = 0; i < 2; i++)
	{
		com.Read(&rb, BOOT_HANDSHAKE_PRETIMEOUT, BOOT_HANDSHAKE_POSTTIMEOUT);

		MAIN_LOOP_PIN_SET();

		while(com.Update())
		{
			HW::ResetWDT();
		};

		MAIN_LOOP_PIN_CLR();

		c = (rb.recieved && rb.len == sizeof(RspHS) && GetCRC16(rb.data, rb.len) == 0 && rsp.guid == masterGUID);

		if (c)
		{
			com.Write(&wb);

			while(com.Update()) { HW::ResetWDT() ; };

			break;
		};
	};

	return c;
}

#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void CheckFlash()
{
	static ADI_BOOT_HEADER bh;
	static u16 crc = 0;

	while (FlashBusy()) FlashUpdate();

	u32 *p = (u32*)&bh;

	u32 adr = 0;
	
	flashOK = flashChecked = flashCRCOK = false;

	//at25df021_Read(buf, 0, sizeof(buf));

	while (1)
	{
		at25df021_Read(&bh, FLASH_START_ADR + adr, sizeof(bh));

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

	at25df021_Read(&crc, FLASH_START_ADR + adr, sizeof(crc));

	if (flashLen > 0) flashCRC = at25df021_GetCRC16(FLASH_START_ADR, flashLen), flashCRCOK = (flashCRC == crc);

	if (!flashCRCOK) flashLen = 0;

	flashChecked = true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

i16 index_max = 0;

int main( void )
{
	static byte s = 0;
	static byte i = 0;

	static u32 pt = 0;

	//static RTM32 tm;

	InitHardware();

	FlashInit();

	com.Connect(BOOT_COM_SPEED, BOOT_COM_PARITY);

	CheckFlash();

	#ifdef BOOT_HANDSHAKE

		run = HandShake();

	#endif

	#ifdef BOOT_TIMEOUT
		tm64.Reset(); timeOut = BOOT_TIMEOUT;
	#endif

	while (run)
	{
		UpdateBlackFin();
		FlashUpdate();

		#ifdef BOOT_HW_UPDATE
			BOOT_HW_UPDATE();
		#endif


		#ifdef BOOT_TIMEOUT
			if (tm64.Timeout(timeOut)) break;
		#endif

		MAIN_LOOP_PIN_TGL();
	};

	RunMainApp();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // BOOT_IMP_H__14_11_2022__12_13
