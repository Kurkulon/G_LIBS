#ifndef CRC_CCITT_DMA_IMP_H__24_11_2023__15_50
#define CRC_CCITT_DMA_IMP_H__24_11_2023__15_50

#pragma once

#include "CRC\CRC_CCITT_DMA.h"
#include "DMA\DMA.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef CPU_XMC48

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static DMA_CH CRC_DMA(CRC_DMA_CH);

static bool busy_CRC_CCITT_DMA = false;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CRC_CCITT_PIO(const void *data, u32 len, u16 *crc, u16 init)
{
	CRC_FCE->CRC = init;	//	DataCRC CRC = { init };

	__packed const byte *s = (__packed const byte*)data;

	for ( ; len > 0; len--)
	{
		CRC_FCE->IR = *(s++);
	};

	//if (len > 0)
	//{
	//	CRC_FCE->IR = *(s++)&0xFF;
	//}

	__dsb(15);

	if (crc != 0) *crc = CRC_FCE->RES;

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16 CRC_CCITT_DMA(const void *data, u32 len, u16 init)
{
	CRC_FCE->CRC = init;	//	DataCRC CRC = { init };

	CRC_DMA.MemCopySrcInc(data, &CRC_FCE->IR, len);

	while (!CRC_DMA.CheckMemCopyComplete());

	__dsb(15);

	return (byte)CRC_FCE->RES;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CRC_CCITT_DMA_Async(const void* data, u32 len, u16 init)
{
	if (busy_CRC_CCITT_DMA) return false;

	busy_CRC_CCITT_DMA = true;

	CRC_FCE->CRC = init;	//	DataCRC CRC = { init };

	CRC_DMA.MemCopySrcInc(data, &CRC_FCE->IR, len);

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CRC_CCITT_DMA_CheckComplete(u16* crc)
{
	if (CRC_DMA.CheckMemCopyComplete())
	{
		__dsb(15);

		*crc = (byte)CRC_FCE->RES;

		busy_CRC_CCITT_DMA = false;

		return true;
	}
	else
	{
		return false;
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void Init_CRC_CCITT_DMA()
{
	SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_YELLOW "Init_CRC_CCITT_DMA ... ");

	HW::Peripheral_Enable(PID_FCE);

	HW::FCE->CLC = 0;
	CRC_FCE->CFG = 0;

	SEGGER_RTT_WriteString(0, "OK\n");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#elif defined(CPU_SAME53)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static DMA_CH CRC_DMA(CRC_DMA_CH);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef CRCDMA_TC

	#define crcdmaTC		HW::CRCDMA_TC

	#ifndef CRCDMA_PER
	#define CRCDMA_PER		(NS2CLK(1000)-1)
	#endif

	#if CRCDMA_PER < 1
	#undef CRCDMA_PER
	#define CRCDMA_PER	1
	#endif

	inline void CRCDMA_ClockEnable()  { HW::GCLK->PCHCTRL[CONCAT2(GCLK_,CRCDMA_TC)] = GCLK_GEN(CONCAT2(GEN_,CRCDMA_TC))|GCLK_CHEN; HW::MCLK->ClockEnable(CONCAT2(PID_,CRCDMA_TC)); }

	#define CRCDMA_TRIGSRC_OVF	CONCAT3(DMCH_TRIGSRC_,CRCDMA_TC,_OVF)

#else

	//#error  Must defined MANT_TC or MANT_TCC

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef CRCDMA_TC

static void Init_CRC_CCITT_DMA()
{
	SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_YELLOW "Init_CRC_CCITT_DMA ... ");

	CRCDMA_ClockEnable();

	crcdmaTC->CTRLA = 0;
	crcdmaTC->CTRLA = TC_SWRST;
	while(crcdmaTC->SYNCBUSY & TC_SWRST);

	crcdmaTC->CTRLA		= TC_MODE_COUNT8;
	crcdmaTC->WAVE		= TC_WAVEGEN_NPWM;
	crcdmaTC->DRVCTRL	= TC_INVEN0|TC_INVEN1;
	crcdmaTC->PER8		= CRCDMA_PER;
	crcdmaTC->CC8[0]	= 0; 
	crcdmaTC->CC8[1]	= 0; 

	crcdmaTC->EVCTRL = TC_OVFEO;

	crcdmaTC->CTRLA = TC_MODE_COUNT8|TC_ENABLE;

	HW::DMAC->CRCSTATUS = ~0;
	HW::DMAC->CRCCTRL = 0;

	SEGGER_RTT_WriteString(0, "OK\n");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CRC_CCITT_DMA_Async(const void* data, u32 len, u16 init)
{
	if (HW::DMAC->CRCCTRL != 0) return false;

	HW::DMAC->CRCSTATUS = ~0;
	HW::DMAC->CRCCTRL = 0;
	HW::DMAC->CRCCHKSUM = ReverseWord(init);
	HW::DMAC->CRCCTRL	= DMAC_CRCBEATSIZE_BYTE | DMAC_CRCPOLY_CRC16 | DMAC_CRCMODE_DEFAULT | DMAC_CRCSRC_IO;

	CRC_DMA.WritePeripheral(data, &HW::DMAC->CRCDATAIN, len, DMCH_TRIGACT_BURST|CRCDMA_TRIGSRC_OVF, DMDSC_BEATSIZE_BYTE);

	crcdmaTC->CTRLBSET = TC_CMD_RETRIGGER;

	//CRC_DMA.CRC_CCITT(data, len, init);

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CRC_CCITT_DMA_CheckComplete(u16* crc)
{
	if (CRC_DMA.CheckComplete())
	{
		crcdmaTC->CTRLBSET = TC_CMD_STOP;

		*crc = CRC_DMA.Get_CRC_CCITT_Result();

		HW::DMAC->CRCSTATUS = ~0;
		HW::DMAC->CRCCTRL = 0;

		return true;
	}
	else
	{
		return false;
	};
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CRC_CCITT_PIO(const void *data, u32 len, u16 *crc, u16 init)
{
	if (HW::DMAC->CRCCTRL != 0) return false;

	HW::DMAC->CRCSTATUS = ~0;
	HW::DMAC->CRCCTRL = 0;
	HW::DMAC->CRCCHKSUM = ReverseWord(init);
	HW::DMAC->CRCCTRL	= DMAC_CRCBEATSIZE_BYTE | DMAC_CRCPOLY_CRC16 | DMAC_CRCMODE_DEFAULT | DMAC_CRCSRC_IO;

	const byte *s = (const byte*)data;

	for ( ; len > 0; len--) HW::DMAC->CRCDATAIN = *(s++);

	__dsb(15);

	*crc = CRC_DMA.Get_CRC_CCITT_Result();

	HW::DMAC->CRCSTATUS = ~0;
	HW::DMAC->CRCCTRL = 0;

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#else // #ifdef CRCDMA_TC

static void Init_CRC_CCITT_DMA()
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CRC_CCITT_DMA_Async(const void* data, u32 len, u16 init)
{
	if (HW::DMAC->CRCCTRL != 0) return false;

	HW::DMAC->CRCSTATUS = ~0;
	HW::DMAC->CRCCTRL = 0;
	HW::DMAC->CRCCHKSUM = ReverseWord(init);
	HW::DMAC->CRCCTRL	= DMAC_CRCBEATSIZE_BYTE | DMAC_CRCPOLY_CRC16 | DMAC_CRCMODE_DEFAULT | DMAC_CRCSRC_IO;

	CRC_DMA.WritePeripheral(data, &HW::DMAC->CRCDATAIN, len, DMCH_TRIGACT_TRANSACTION, DMDSC_BEATSIZE_BYTE);
	CRC_DMA.SoftwareTrigger();

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CRC_CCITT_DMA_CheckComplete(u16* crc)
{
	if (CRC_DMA.CheckComplete())
	{
		__dsb(15);

		*crc = CRC_DMA.Get_CRC_CCITT_Result();

		HW::DMAC->CRCSTATUS = ~0;
		HW::DMAC->CRCCTRL = 0;

		return true;
	}
	else
	{
		return false;
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CRC_CCITT_PIO(const void *data, u32 len, u16 *crc, u16 init)
{
	if (HW::DMAC->CRCCTRL != 0) return false;

	HW::DMAC->CRCSTATUS = ~0;
	HW::DMAC->CRCCTRL = 0;
	HW::DMAC->CRCCHKSUM = ReverseWord(init);
	HW::DMAC->CRCCTRL	= DMAC_CRCBEATSIZE_BYTE | DMAC_CRCPOLY_CRC16 | DMAC_CRCMODE_DEFAULT | DMAC_CRCSRC_IO;

	const byte *s = (const byte*)data;

	for ( ; len > 0; len--) HW::DMAC->CRCDATAIN = *(s++);

	__dsb(15);

	*crc = CRC_DMA.Get_CRC_CCITT_Result();

	HW::DMAC->CRCSTATUS = ~0;
	HW::DMAC->CRCCTRL = 0;

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // #else // #ifdef CRCDMA_TC

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16 CRC_CCITT_DMA(const void *data, u32 len, u16 init)
{
	while (!CRC_CCITT_DMA_Async(data, len, init));

	u16 crc;

	while (!CRC_CCITT_DMA_CheckComplete(&crc));

	return crc;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#elif defined(CPU_SAM4SA)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

__align(512) T_HW::S_CRCCU::TRDSCR crccuDsc = {0};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void Init_CRC_CCITT_DMA()
{
	HW::PMC->ClockEnable(HW::PID::CRCCU_I);

	HW::CRCCU->DMA_DIS	= CRCCU_DMADIS;
	HW::CRCCU->DSCR		= &crccuDsc;
	HW::CRCCU->MR		= CRCCU_ENABLE|CRCCU_CCITT16;
	HW::CRCCU->CR		= CRCCU_RESET;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CRC_CCITT_DMA_Async(const void* data, u32 len, u16 init)
{
	if (HW::CRCCU->DMA_SR & CRCCU_DMASR) return false;

	HW::CRCCU->DSCR		= &crccuDsc;
	HW::CRCCU->MR		= CRCCU_ENABLE|CRCCU_CCITT16;
	HW::CRCCU->CR		= CRCCU_RESET;
	
	crccuDsc.ADDR = data;
	crccuDsc.CTRL = (u16)len;

	HW::CRCCU->DMA_EN	= CRCCU_DMAEN;

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CRC_CCITT_DMA_CheckComplete(u16* crc)
{
	if ((HW::CRCCU->DMA_SR & CRCCU_DMASR) == 0)
	{
		if (crc != 0) *crc = HW::CRCCU->SR;

		return true;
	}
	else
	{
		return false;
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CRC_CCITT_PIO(const void *data, u32 len, u16 *crc, u16 init)
{
	if (!CRC_CCITT_DMA_Async(data, len, init)) return false;

	while (!CRC_CCITT_DMA_CheckComplete(crc));

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#elif defined(WIN32)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static u16 crc_ccit_result = 0;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CRC_CCITT_PIO(const void *data, u32 len, u16 *crc, u16 init)
{
	if (crc != 0) *crc = 0;

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16 CRC_CCITT_DMA(const void *data, u32 len, u16 init)
{
	return 0;//GetCRC16_CCIT(data, len, init);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CRC_CCITT_DMA_Async(const void* data, u32 len, u16 init)
{
	crc_ccit_result = 0;//GetCRC16_CCIT(data, len, init);

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CRC_CCITT_DMA_CheckComplete(u16* crc)
{
	if (crc != 0)
	{
		*crc = crc_ccit_result;

		return true;
	}
	else
	{
		return false;
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#elif defined(CPU_BF607)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static u16 crc_ccit_result = 0;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CRC_CCITT_PIO(const void *data, u32 len, u16 *crc, u16 init)
{
	if (crc != 0) *crc = 0;

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16 CRC_CCITT_DMA(const void *data, u32 len, u16 init)
{
	return 0;//GetCRC16_CCIT(data, len, init);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CRC_CCITT_DMA_Async(const void* data, u32 len, u16 init)
{
	crc_ccit_result = 0;//GetCRC16_CCIT(data, len, init);

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool CRC_CCITT_DMA_CheckComplete(u16* crc)
{
	if (crc != 0)
	{
		*crc = crc_ccit_result;

		return true;
	}
	else
	{
		return false;
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#endif // MEM_IMP_H__12_10_2022__17_05
