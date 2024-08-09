#if !defined(CPU_LPC812)

#include "DMA.h"

#ifdef CPU_SAME53

	__align(16) T_HW::DMADESC DmaTable[32];
	__align(16) T_HW::DMADESC DmaWRB[32];
	
#elif defined(CPU_XMC48)

#elif defined(CPU_LPC824)

	__align(512) T_HW::DMADESC _DmaTable[18] = {0};

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef __ADSPBF59x__ 

void DMA_CH::Trans(volatile void *src, u16 len, u16 ctrl)
{
	_dmach->CONFIG = 0;
	_dmach->START_ADDR = (void*)src;
	_dmach->X_COUNT = len;
	_dmach->X_MODIFY = 1UL<<((ctrl>>2)&3);

	_dmach->IRQ_STATUS = DMA_DONE;
	_dmach->CONFIG = FLOW_STOP|DI_EN|(ctrl&(WDSIZE_16|WDSIZE_32|WNR))|SYNC|DMAEN;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void DMA_CH::Trans(volatile void *stadr1, u16 len1, u16 mdfy1, u16 ctrl1, volatile void *stadr2, u16 len2, u16 mdfy2, u16 ctrl2)
{
	_dmach->CONFIG = 0;

	ctrl1 = (ctrl1&(WDSIZE_16|WDSIZE_32|WNR))|SYNC|DMAEN;
	ctrl2 = (ctrl2&(WDSIZE_16|WDSIZE_32|WNR))|SYNC|DMAEN;

	_dsc1.SA = (void*)stadr1;
	_dsc1.XCNT = len1;
	_dsc1.XMOD = mdfy1;

	if (stadr2 != 0 && len2 != 0)
	{
		_dsc1.NDP = &_dsc2;
		_dsc1.DMACFG = FLOW_LARGE|NDSIZE_9|ctrl1;

		_dsc2.SA = (void*)stadr2;
		_dsc2.XCNT = len2;
		_dsc2.XMOD = mdfy2;
		_dsc2.DMACFG = FLOW_STOP|DI_EN|ctrl2;
	}
	else
	{
		_dsc1.DMACFG = FLOW_STOP|DI_EN|ctrl1;
	};

	_dmach->IRQ_STATUS = DMA_DONE;

	_dmach->NEXT_DESC_PTR = &_dsc1;
	_dmach->CONFIG = FLOW_LARGE|NDSIZE_9|DMAEN;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void DMA_CH::Trans2D(volatile void *stadr1, u16 xcount1, u16 xmdfy1, u16 ycount1, u16 ymdfy1, volatile void *stadr2, u16 xcount2, u16 xmdfy2, u16 ycount2, u16 ymdfy2, u16 ctrl)
{
	_dmach->CONFIG = 0;

	u16 ctrl1 = (ctrl&(WDSIZE_16|WDSIZE_32|WNR))|SYNC|DMAEN;
	u16 ctrl2 = ctrl1;

	if (ycount1 != 0 ) ctrl1 |= DMA2D;
	if (ycount2 != 0 ) ctrl2 |= DMA2D;

	_dsc1.SA = (void*)stadr1;
	_dsc1.XCNT = xcount1;
	_dsc1.XMOD = xmdfy1;
	_dsc1.YCNT = ycount1;
	_dsc1.YMOD = ymdfy1;

	if (stadr2 != 0 && xcount2 != 0)
	{
		_dsc1.NDP = &_dsc2;
		_dsc1.DMACFG = FLOW_LARGE|NDSIZE_9|ctrl1;

		_dsc2.SA = (void*)stadr2;
		_dsc2.XCNT = xcount2;
		_dsc2.XMOD = xmdfy2;
		_dsc2.YCNT = ycount2;
		_dsc2.YMOD = ymdfy2;

		_dsc2.DMACFG = FLOW_STOP|DI_EN|ctrl2;
	}
	else
	{
		_dsc1.DMACFG = FLOW_STOP|DI_EN|ctrl1;
	};

	_dmach->IRQ_STATUS = DMA_DONE;

	_dmach->NEXT_DESC_PTR = &_dsc1;
	_dmach->CONFIG = FLOW_LARGE|NDSIZE_9|DMAEN;
}

#elif defined(__ADSPBF70x__) //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void DMA_CH::Trans(volatile void *src, u16 len, u16 ctrl)
{
	_dmach->CFG = 0;
	_dmach->ADDRSTART = (void*)src;
	_dmach->XCNT = len;
	_dmach->XMOD = 1UL<<((ctrl>>4)&3);

	_dmach->STAT = ~0;
	_dmach->CFG = DMA_FLOW_STOP|(ctrl&(DMA_PSIZE(~0)|DMA_WNR))|DMA_SYNC|DMA_EN;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void DMA_CH::Trans(volatile void *stadr1, u16 len1, u16 mdfy1, u16 ctrl1, volatile void *stadr2, u16 len2, u16 mdfy2, u16 ctrl2)
{
	_dmach->CFG = 0;

	ctrl1 = (ctrl1&(DMA_PSIZE(~0)|DMA_WNR))|DMA_SYNC|DMA_EN;
	ctrl2 = (ctrl2&(DMA_PSIZE(~0)|DMA_WNR))|DMA_SYNC|DMA_EN;

	_dsc1.ADDRSTART	= (void*)stadr1;
	_dsc1.XCNT		= len1;
	_dsc1.XMOD		= mdfy1;

	if (stadr2 != 0 && len2 != 0)
	{
		_dsc1.DSCPTR_NXT = &_dsc2;
		_dsc1.DMACFG = DMA_FLOW_DSCLIST|DMA_NDSIZE5|ctrl1;

		_dsc2.ADDRSTART = (void*)stadr2;
		_dsc2.XCNT = len2;
		_dsc2.XMOD = mdfy2;
		_dsc2.DMACFG = DMA_FLOW_STOP|ctrl2;
	}
	else
	{
		_dsc1.DMACFG = DMA_FLOW_STOP|ctrl1;
	};

	_dmach->STAT = ~0;

	_dmach->DSCPTR_NXT = &_dsc1;
	_dmach->CFG = DMA_FLOW_DSCLIST|DMA_NDSIZE5|DMA_EN;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void DMA_CH::Trans2D(volatile void *stadr1, u16 xcount1, u16 xmdfy1, u16 ycount1, u16 ymdfy1, volatile void *stadr2, u16 xcount2, u16 xmdfy2, u16 ycount2, u16 ymdfy2, u16 ctrl)
{
	_dmach->CFG = 0;

	ctrl = (ctrl&(DMA_PSIZE(~0)|DMA_WNR))|DMA_SYNC|DMA_EN;
	u16 ctrl2 = ctrl;

	if (ycount1 != 0 ) ctrl  |= DMA_TWOD;
	if (ycount2 != 0 ) ctrl2 |= DMA_TWOD;

	_dsc1.ADDRSTART	= (void*)stadr1;
	_dsc1.XCNT		= xcount1;
	_dsc1.XMOD		= xmdfy1;
	_dsc1.YCNT		= ycount1;
	_dsc1.YMOD		= ymdfy1;

	if (stadr2 != 0 && xcount2 != 0)
	{
		_dsc1.DSCPTR_NXT = &_dsc2;
		_dsc1.DMACFG	 = DMA_FLOW_DSCLIST|DMA_NDSIZE7|ctrl;

		_dsc2.ADDRSTART = (void*)stadr2;
		_dsc2.XCNT		= xcount2;
		_dsc2.XMOD		= xmdfy2;
		_dsc2.YCNT		= ycount2;
		_dsc2.YMOD		= ymdfy2;

		_dsc2.DMACFG = DMA_FLOW_STOP|ctrl2;
	}
	else
	{
		_dsc1.DMACFG = DMA_FLOW_STOP|ctrl;
	};

	_dmach->STAT = ~0;

	_dmach->DSCPTR_NXT = &_dsc1;
	_dmach->CFG = DMA_FLOW_DSCLIST|DMA_NDSIZE7|DMA_EN;
}

#endif // #ifdef ADSP_BLACKFIN 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void DMA_CH::_MemCopy(const volatile void *src, volatile void *dst, u16 len, u32 ctrl)
{
	//using namespace HW;

#ifdef CPU_SAME53
	
	_dmadsc->SRCADDR = src;
	_dmadsc->DSTADDR = dst;
	_dmadsc->DESCADDR = 0;
	_dmadsc->BTCNT = _dmawrb->BTCNT =len;
	_dmadsc->BTCTRL = ctrl;

	_dmach->INTENCLR = ~0;
	_dmach->INTFLAG = ~0;
	_dmach->CTRLA = DMCH_ENABLE|DMCH_TRIGACT_TRANSACTION;

	SoftwareTrigger();

#elif defined(CPU_XMC48)

	_GPDMA->DMACFGREG = 1;

	_InitLLI(src, dst, len, ctrl); //DST_INC|SRC_INC|TT_FC_M2M_GPDMA|DEST_MSIZE_1|SRC_MSIZE_1|LLP_DST_EN|LLP_SRC_EN
	
	_dmach->CFGL = 0;
	_dmach->CFGH = PROTCTL(1);

	Enable();

#endif

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef CPU_XMC48

bool DMA_CH::CheckMemCopyComplete()
{
	if (CheckComplete())
	{
		if (_dataLen > 0) 
		{
			if (_dataLen > BLOCK_TS(~0))
			{
				_dmach->CTLH = BLOCK_TS(~0);
				_dataLen -= BLOCK_TS(~0);
			}
			else
			{
				_dmach->CTLH = BLOCK_TS(_dataLen);
				_dataLen = 0;
			};

			Enable();
		}
		else
		{
			return true;
		};
	};

	return false;
}

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void DMA_CH::WritePeripheral(const volatile void *src, volatile void *dst, u16 len, u32 ctrl1, u32 ctrl2)
{
	//using namespace HW;

#ifdef CPU_SAME53

	_dmach->CTRLA = 0;

	_dmadsc->SRCADDR = (byte*)src+(len<<((ctrl2>>8)&3));
	_dmadsc->DSTADDR = dst;
	_dmadsc->DESCADDR = 0;
	_dmadsc->BTCNT = _dmawrb->BTCNT = len;
	_dmadsc->BTCTRL = ctrl2 | DMDSC_VALID|DMDSC_SRCINC;//|DMDSC_BEATSIZE_BYTE;

	_dmach->INTENCLR = ~0;
	_dmach->INTFLAG = ~0;

	_dmach->CTRLA = ctrl1 | DMCH_ENABLE; //DMCH_TRIGACT_BURST|_dma_trgsrc_tx;
	
#elif defined(CPU_XMC48)

	HW::DLR->LNEN |= _dlr_lnen_mask;

	_GPDMA->DMACFGREG = 1;

	_InitLLI(src, dst, len, ctrl1);

	_dmach->CFGL = HS_SEL_SRC;
	_dmach->CFGH = PROTCTL(1)|DEST_PER(_drl&7);

	Enable();

#elif defined(CPU_LPC824)

	HW::DMA->ENABLESET0 = _act_mask;	

	_dmach->CFG = ctrl1;	

	_dmadsc->SEA = (byte*)src + len - 1;	
	_dmadsc->DEA = dst;			
	_dmadsc->NEXT = 0;

	_dmach->XFERCFG = ctrl2 | XFERCOUNT(len);

	HW::DMA->SETVALID0 = _act_mask;

#endif

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void DMA_CH::ReadPeripheral(const volatile void *src, volatile void *dst, u16 len, u32 ctrl1, u32 ctrl2)
{
	//using namespace HW;

#ifdef CPU_SAME53

		_dmach->CTRLA = 0;

		_dmadsc->SRCADDR = src;
		_dmadsc->DSTADDR = (byte*)dst+(len<<((ctrl2>>8)&3));
		_dmadsc->DESCADDR = 0;
		_dmadsc->BTCNT = _dmawrb->BTCNT = len;
		_dmadsc->BTCTRL = ctrl2 | DMDSC_VALID|DMDSC_DSTINC; //|DMDSC_BEATSIZE_BYTEC;

		_dmach->INTENCLR = ~0;
		_dmach->INTFLAG = ~0;

		_dmach->CTRLA = ctrl1 | DMCH_ENABLE; //|DMCH_TRIGACT_BURST|_dma_trgsrc_rx;
	
#elif defined(CPU_XMC48)

	_GPDMA->DMACFGREG = 1;

	_InitLLI(src, dst, len, ctrl1);

	_dmach->CFGL = HS_SEL_DST;
	_dmach->CFGH = PROTCTL(1)|SRC_PER(_drl&7);

	Enable();

	HW::DLR->LNEN |= _dlr_lnen_mask;

#elif defined(CPU_LPC824)

	HW::DMA->ENABLESET0 = _act_mask;	

	_dmach->CFG = ctrl1;	

	_dmadsc->SEA = (void*)src;		
	_dmadsc->DEA = (byte*)dst + len - 1;	
	_dmadsc->NEXT = 0;

	_dmach->XFERCFG = ctrl2 | XFERCOUNT(len);

	HW::DMA->SETVALID0 = _act_mask;

#endif

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef CPU_SAME53

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void DMA_CH::SystemInit()
{
	HW::MCLK->AHBMASK |= AHB_DMAC;
	HW::DMAC->CTRL = 0;
	HW::DMAC->CTRL = DMAC_SWRST;
	HW::DMAC->DBGCTRL = DMAC_DBGRUN;
	HW::DMAC->BASEADDR	= DmaTable;
	HW::DMAC->WRBADDR	= DmaWRB;
	HW::DMAC->CTRL = DMAC_DMAENABLE|DMAC_LVLEN0|DMAC_LVLEN1|DMAC_LVLEN2|DMAC_LVLEN3;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void DMA_CH::WritePeripheral(const volatile void *src, volatile void *dst, u16 len, const volatile void *src2, u16 len2, u32 ctrl1, u32 ctrl2)
{
	using namespace HW;

	_dmach->CTRLA = 0;

	if (src2 != 0 && len2 > 0)
	{
		T_HW::DMADESC &wrdsc = _wr_dmadsc[_chnum];
		wrdsc.SRCADDR = (byte*)src2+len2;
		wrdsc.DSTADDR = dst;
		wrdsc.BTCNT = len2;
		wrdsc.BTCTRL = ctrl2 | DMDSC_VALID|DMDSC_SRCINC;//|DMDSC_BEATSIZE_BYTE;
		_dmadsc->DESCADDR = &wrdsc;
	}
	else
	{
		_dmadsc->DESCADDR = 0;
	};

	_dmadsc->SRCADDR = (byte*)src+len;
	_dmadsc->DSTADDR = dst;
	_dmadsc->BTCNT = _dmawrb->BTCNT = len;
	_dmadsc->BTCTRL = ctrl2 | DMDSC_VALID|DMDSC_SRCINC;//|DMDSC_BEATSIZE_BYTE;

	_dmach->INTENCLR = ~0;
	_dmach->INTFLAG = ~0;

	_dmach->CTRLA = ctrl1 | DMCH_ENABLE; //DMCH_TRIGACT_BURST|_dma_trgsrc_tx;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
#elif defined(CPU_XMC48)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void DMA_CH::_InitLLI(const volatile void *src, volatile void *dst, u16 len, u32 ctrl)
{
	using namespace HW;

	_dmach->SAR = _startSrcAdr = (u32)src;
	_dmach->DAR = _startDstAdr = (u32)dst;

	if (_chnumabs < 2)
	{
		if (len > BLOCK_TS(~0))
		{
			u32 i = 0;

			LLI *lli = &_lli[0];

			for ( ; i < ArraySize(_lli); i++)
			{
				lli = _lli+i;

				lli->SAR = (void*)src;
				lli->CTLL = ctrl;
				lli->DAR = dst;

				if (len > BLOCK_TS(~0))
				{
					lli->LLP = _lli+i+1;
					lli->CTLH = BLOCK_TS(~0);
					len -= BLOCK_TS(~0);
					*((byte**)&src) += BLOCK_TS(~0);
					*((byte**)&dst) += BLOCK_TS(~0);
				}
				else
				{
					lli->LLP = 0;
					lli->CTLH = len;
					len = 0;
					break;
				};
			};

			lli->LLP = 0;

			_dmach->CTLH = BLOCK_TS(~0);
			_dmach->CTLL = ctrl;
			((T_HW::GPDMA0_CH_Type*)_dmach)->LLP = (u32)&_lli[0];

			_dataLen = len;
		}
		else
		{
			_dmach->CTLH = len;
			_dmach->CTLL = ctrl;
			((T_HW::GPDMA0_CH_Type*)_dmach)->LLP = 0;
			_dataLen = 0;
		};
	}
	else
	{
		if (len > BLOCK_TS(~0))
		{
			_dataLen = len - BLOCK_TS(~0);
			len = BLOCK_TS(~0);
		}
		else
		{
			_dataLen = 0;
		};

		_dmach->CTLH = len;
		_dmach->CTLL = ctrl;
	};
}

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // #if !defined(CPU_LPC812)
