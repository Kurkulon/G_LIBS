#ifndef DMA_H__26_05_2022__18_11
#define DMA_H__26_05_2022__18_11

#if !defined(CPU_LPC812)

#include "types.h"
#include "core.h"
#include "time.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class DMA_CH
{
protected:

#ifdef __ADSPBF59x__

	T_HW::S_DMACH*				const _dmach;
	T_HW::S_DMACH*				const _dstch;

	T_HW::DMADSC_LLM			_dsc1;
	T_HW::DMADSC_LLM			_dsc2;

#elif defined(__ADSPBF70x__)

	T_HW::S_DMACH*				const _dmach;
	T_HW::S_DMACH*				const _dstch;

	T_HW::DMADSC_LM				_dsc1;
	T_HW::DMADSC_LM				_dsc2;

#elif defined(__ADSPBF60x__)

	typedef T_HW::S_DMACH* HWPTR_DMACH;

	static const HWPTR_DMACH DMACH_TBL[35];

	T_HW::S_DMACH*				const _dmach;
	T_HW::S_DMACH*				const _dstch;

	T_HW::DMADSC_LM				_dsc1;
	T_HW::DMADSC_LM				_dsc2;

#elif defined(CPU_SAME53)

	//static T_HW::DMADESC _DmaTable[32];
	//static T_HW::DMADESC _DmaWRB[32];

	static void SystemInit();
	
	T_HW::S_DMAC::S_DMAC_CH*	const _dmach;

	T_HW::DMADESC*				const _dmadsc;
	T_HW::DMADESC*				const _dmawrb;

	const u32					_act_mask;

	static T_HW::DMADESC		_wr_dmadsc[32];

	CTM32	tm;

	u16 _prevBTCNT;
	u16 _errCount;
	u16 _timeoutCount;

	byte _err;

#elif defined(CPU_SAM4SA)

	T_HW::S_PDC*				const _dmach;

#elif defined(CPU_XMC48)

	struct LLI
	{
		volatile void*	SAR;
		volatile void*	DAR;
		LLI*	LLP;
		u32		CTLL;
		u32		CTLH;
		u32		DSTAT;
	};

	LLI						_lli[4];

	T_HW::GPDMA_Type*		const _GPDMA; 
	T_HW::GPDMA_CH_Type*	const _dmach;

	u32		_dlr_lnen_mask;
	u32		_startSrcAdr;
	u32		_startDstAdr;
	u16		_dataLen;

	const	byte	_chnumabs;
	byte	_drl;
	
	void _InitLLI(const volatile void *src, volatile void *dst, u16 len, u32 ctrl);

#elif defined(CPU_LPC824)

	T_HW::S_DMA::S_CHNL*		const _dmach;
	T_HW::DMADESC*				const _dmadsc;

	const u32					_act_mask;

#endif

#if defined(CPU_SAME53) || defined(CPU_XMC48)

	const byte		_chnum;

#endif
		
	void _MemCopy(const volatile void *src, volatile void *dst, u16 len, u32 ctrl);

public:

	void WritePeripheral(const volatile void *src, volatile void *dst, u16 len, u32 ctrl1, u32 ctrl2);
	void ReadPeripheral(const volatile void *src, volatile void *dst, u16 len, u32 ctrl1, u32 ctrl2);
	

#ifdef ADSP_BLACKFIN //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	void Trans(volatile void *stadr, u16 len, u16 ctrl);
	void Trans(volatile void *stadr1, u16 len1, u16 mdfy1, u16 ctrl1, volatile void *stadr2, u16 len2, u16 mdfy2, u16 ctrl2);
	void Trans2D(volatile void *stadr1, u16 xcount1, u16 xmdfy1, u16 ycount1, u16 ymdfy1, volatile void *stadr2, u16 xcount2, u16 xmdfy2, u16 ycount2, u16 ymdfy2, u16 ctrl);

	//void Write(const volatile void *src1, u16 len1, const volatile void *src2, u16 len2, u16 ctrl);
	//void Read(volatile void *dst, u16 len, u16 ctrl);

	void MemCopy(volatile void* src, volatile void* dst, u16 len)		{ _MemCopy(src, dst, len, 0x10001); }
	void MemCopySrcInc(volatile void *src, volatile void *dst, u16 len)	{ _MemCopy(src, dst, len, 0x00001); }
	void MemCopyDstInc(volatile void *src, volatile void *dst, u16 len)	{ _MemCopy(src, dst, len, 0x10000); }
	void Reset() {  }

	#ifdef __ADSPBF60x__
		DMA_CH(byte chnum) : _dmach(DMACH_TBL[chnum]), _dstch(DMACH_TBL[((chnum - MDMA0_SRC) < 7) ? (chnum+1) : 12]){ }
	#else
		DMA_CH(byte chnum) : _dmach(&HW::DMA->CH[chnum]), _dstch(&HW::DMA->CH[chnum+1]) { }
	#endif

	//void Enable() {  }
	
	#ifdef __ADSPBF59x__
		
		inline void Disable()		{ _dmach->CONFIG = 0; _dmach->IRQ_STATUS = ~0; }
		inline bool CheckComplete() { return _dmach->IRQ_STATUS & (DMA_DONE|DMA_ERR); }
		inline bool CheckMemCopyComplete() { return _dstch->IRQ_STATUS & (DMA_DONE|DMA_ERR); }
		inline u32 GetBytesLeft()	{ return _dmach->CURR_X_COUNT; }
		inline u32 GetBytesReady()	{ return _dmach->X_COUNT-_dmach->CURR_X_COUNT; }

		inline void Write8(volatile void *src, u16 len)		{ Trans(src, len, WDSIZE_8);	}
		inline void Write16(volatile void *src, u16 len)	{ Trans(src, len, WDSIZE_16);	}
		inline void Write32(volatile void *src, u16 len)	{ Trans(src, len, WDSIZE_32);	}

		inline void Read8(volatile void *dst, u16 len)		{ Trans(dst, len, WDSIZE_8|WNR);	}
		inline void Read16(volatile void *dst, u16 len) 	{ Trans(dst, len, WDSIZE_16|WNR);	}
		inline void Read32(volatile void *dst, u16 len) 	{ Trans(dst, len, WDSIZE_32|WNR);	}

		inline void Write8(volatile void *src1, u16 len1, volatile void *src2, u16 len2)	{ Trans(src1, len1, 1, WDSIZE_8,  src2, len2, 1, WDSIZE_8);	 }
		inline void Write16(volatile void *src1, u16 len1, volatile void *src2, u16 len2)	{ Trans(src1, len1, 2, WDSIZE_16, src2, len2, 2, WDSIZE_16); }
		inline void Write32(volatile void *src1, u16 len1, volatile void *src2, u16 len2)	{ Trans(src1, len1, 4, WDSIZE_32, src2, len2, 4, WDSIZE_32); }

		inline void Read8(volatile void *dst1, u16 len1, volatile void *dst2, u16 len2)		{ Trans(dst1, len1, 1, WDSIZE_8|WNR,  dst2, len2, 1, WDSIZE_8|WNR);	 }
		inline void Read16(volatile void *dst1, u16 len1, volatile void *dst2, u16 len2)	{ Trans(dst1, len1, 2, WDSIZE_16|WNR, dst2, len2, 2, WDSIZE_16|WNR); }
		inline void Read32(volatile void *dst1, u16 len1, volatile void *dst2, u16 len2)	{ Trans(dst1, len1, 4, WDSIZE_32|WNR, dst2, len2, 4, WDSIZE_32|WNR); }

		inline void Read8(	volatile void *dst,  u16 dummy_len, u16 len)	{ Trans(dst, dummy_len, 0, WDSIZE_8|WNR,  dst, len, 1, WDSIZE_8|WNR);	}
		inline void Read16(	volatile void *dst,  u16 dummy_len, u16 len)	{ Trans(dst, dummy_len, 0, WDSIZE_16|WNR, dst, len, 2, WDSIZE_16|WNR);	}
		inline void Read32(	volatile void *dst,  u16 dummy_len, u16 len)	{ Trans(dst, dummy_len, 0, WDSIZE_32|WNR, dst, len, 4, WDSIZE_32|WNR);	}

		inline void ReadInterleaved8( volatile void *dst, u16 dummy_len, u16 len) { Trans2D(dst, dummy_len*2, 0, 0, 0, dst, 2, len,   len, 1-len,   WDSIZE_8|WNR);	}
		inline void ReadInterleaved16(volatile void *dst, u16 dummy_len, u16 len) { Trans2D(dst, dummy_len*2, 0, 0, 0, dst, 2, len*2, len, 2-len*2, WDSIZE_16|WNR);	}
		inline void ReadInterleaved32(volatile void *dst, u16 dummy_len, u16 len) { Trans2D(dst, dummy_len*2, 0, 0, 0, dst, 2, len*4, len, 4-len*4, WDSIZE_32|WNR);	}
	
	#elif defined(__ADSPBF70x__) || defined(__ADSPBF60x__)

		inline void Disable()		{ _dmach->CFG = 0; _dmach->STAT = ~0; }
		inline bool CheckComplete() { return _dmach->STAT & (DMA_STAT_IRQDONE|DMA_STAT_IRQERR); }
		inline bool CheckMemCopyComplete() { return _dstch->STAT & (DMA_STAT_IRQDONE|DMA_STAT_IRQERR); }
		inline u32 GetBytesLeft()	{ return _dmach->XCNT_CUR; }
		inline u32 GetBytesReady()	{ return _dmach->XCNT - _dmach->XCNT_CUR; }

		inline void Write8(volatile void *src, u16 len)	{ Trans(src, len, DMA_PSIZE8 );	}
		inline void Write16(volatile void *src, u16 len)	{ Trans(src, len, DMA_PSIZE16|DMA_MSIZE16);	}
		inline void Write32(volatile void *src, u16 len)	{ Trans(src, len, DMA_PSIZE32|DMA_MSIZE32);	}

		inline void Read8(volatile void *dst, u16 len)	{ Trans(dst, len, DMA_PSIZE8 |DMA_WNR);	}
		inline void Read16(volatile void *dst, u16 len) 	{ Trans(dst, len, DMA_PSIZE16|DMA_MSIZE16|DMA_WNR);	}
		inline void Read32(volatile void *dst, u16 len) 	{ Trans(dst, len, DMA_PSIZE32|DMA_MSIZE32|DMA_WNR);	}

		inline void Write8(volatile void *src1, u16 len1, volatile void *src2, u16 len2)	{ Trans(src1, len1, 1, DMA_PSIZE8,					src2, len2, 1, DMA_PSIZE8 );				}
		inline void Write16(volatile void *src1, u16 len1, volatile void *src2, u16 len2)	{ Trans(src1, len1, 2, DMA_PSIZE16|DMA_MSIZE16,		src2, len2, 2, DMA_PSIZE16|DMA_MSIZE16);	}
		inline void Write32(volatile void *src1, u16 len1, volatile void *src2, u16 len2)	{ Trans(src1, len1, 4, DMA_PSIZE32|DMA_MSIZE32,		src2, len2, 4, DMA_PSIZE32|DMA_MSIZE32);	}

		inline void Read8(volatile void *dst1, u16 len1, volatile void *dst2, u16 len2)	{ Trans(dst1, len1, 1, DMA_PSIZE8 |DMA_WNR,					dst2, len2, 1, DMA_PSIZE8 |DMA_WNR);				}
		inline void Read16(volatile void *dst1, u16 len1, volatile void *dst2, u16 len2)	{ Trans(dst1, len1, 2, DMA_PSIZE16|DMA_MSIZE16|DMA_WNR,		dst2, len2, 2, DMA_PSIZE16|DMA_MSIZE16|DMA_WNR);	}
		inline void Read32(volatile void *dst1, u16 len1, volatile void *dst2, u16 len2)	{ Trans(dst1, len1, 4, DMA_PSIZE32|DMA_MSIZE32|DMA_WNR,		dst2, len2, 4, DMA_PSIZE32|DMA_MSIZE32|DMA_WNR);	}

		inline void Read8(	volatile void *dst,  u16 dummy_len, u16 len)		{ Trans(dst, dummy_len, 0, DMA_PSIZE8 |DMA_WNR,				dst, len, 1, DMA_PSIZE8 |DMA_WNR);				}
		inline void Read16(	volatile void *dst,  u16 dummy_len, u16 len)	{ Trans(dst, dummy_len, 0, DMA_PSIZE16|DMA_MSIZE16|DMA_WNR,	dst, len, 2, DMA_PSIZE16|DMA_MSIZE16|DMA_WNR);	}
		inline void Read32(	volatile void *dst,  u16 dummy_len, u16 len)	{ Trans(dst, dummy_len, 0, DMA_PSIZE32|DMA_MSIZE32|DMA_WNR,	dst, len, 4, DMA_PSIZE32|DMA_MSIZE32|DMA_WNR);	}

		inline void ReadInterleaved8( volatile void *dst, u16 dummy_len, u16 len) { Trans2D(dst, dummy_len*2, 0, 0, 0, dst, 2, len,   len, 1-len,   DMA_PSIZE8 |DMA_WNR);				}
		inline void ReadInterleaved16(volatile void *dst, u16 dummy_len, u16 len) { Trans2D(dst, dummy_len*2, 0, 0, 0, dst, 2, len*2, len, 2-len*2, DMA_PSIZE16|DMA_MSIZE16|DMA_WNR);	}
		inline void ReadInterleaved32(volatile void *dst, u16 dummy_len, u16 len) { Trans2D(dst, dummy_len*2, 0, 0, 0, dst, 2, len*4, len, 4-len*4, DMA_PSIZE32|DMA_MSIZE32|DMA_WNR);	}

	#endif



#elif defined(CPU_SAME53) //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	DMA_CH(byte chnum) : _dmach(&HW::DMAC->CH[chnum]), _dmadsc(&DmaTable[chnum]), _dmawrb(&DmaWRB[chnum]), _act_mask(0x8000|(chnum<<8)), _errCount(0), _timeoutCount(0), _err(0), _chnum(chnum) { _dmach->PRILVL = DMCH_PRILVL_LVL0; }

	//void Enable() {  }
	
	void Disable() { while (_dmach->CTRLA & DMCH_ENABLE) _dmach->CTRLA &= ~DMCH_ENABLE; _dmach->CTRLA = DMCH_SWRST; }
	void Reset() { _dmach->CTRLA = DMCH_SWRST; }
	void Suspend() { _dmach->CTRLB = DMCH_CMD_SUSPEND; }

	byte CheckComplete(u32 timeout = 2000000);

	void Update() { if (_dmach->STATUS & DMCH_FERR) _errCount++, _dmach->CTRLB = DMCH_CMD_RESUME; }
	
	void MemCopy(volatile void *src, volatile void *dst, u16 len)		{ _MemCopy((byte*)src+len, (byte*)dst+len, len, DMDSC_VALID|DMDSC_BEATSIZE_BYTE|DMDSC_DSTINC|DMDSC_SRCINC); }
	void MemCopySrcInc(volatile void *src, volatile void *dst, u16 len) { _MemCopy((byte*)src+len, dst, len, DMDSC_VALID|DMDSC_BEATSIZE_BYTE|DMDSC_SRCINC); }
	void MemCopyDstInc(volatile void *src, volatile void *dst, u16 len) { _MemCopy(src, (byte*)dst+len, len, DMDSC_VALID|DMDSC_BEATSIZE_BYTE|DMDSC_DSTINC); }
	byte CheckMemCopyComplete() { return CheckComplete(); }

	u32 GetSrcBytesReady()	{ return 0; }
	u32 GetDstBytesReady()	{ return 0; }
	u32 GetSrcBytesLeft()	{ return 0; }
	u32 GetDstBytesLeft()	{ return 0; }

	u32 GetBytesLeft()	{ u32 t = HW::DMAC->ACTIVE; return ((t & 0x9F00) == _act_mask) ? (t >> 16) : _dmawrb->BTCNT; }
	u32 GetBytesReady()	{ return _dmadsc->BTCNT - GetBytesLeft(); }

	void SoftwareTrigger() { HW::DMAC->SWTRIGCTRL = 1UL<<_chnum; }
	void SetEvCtrl(byte v) { _dmach->EVCTRL = v; }
	//void SetPriLvl(byte v) { _dmach->PRILVL = v; }

	void WritePeripheral(const volatile void *src, volatile void *dst, u16 len, const volatile void *src2, u16 len2, u32 ctrl1, u32 ctrl2);

	u16 Get_CRC_CCITT_Result() { return ReverseWord(HW::DMAC->CRCCHKSUM); }

	//void CRC_CCITT(const void* data, u16 len, u16 init)
	//{
	//	init = ReverseWord(init);
	//	HW::DMAC->CRCCTRL = DMAC_CRCBEATSIZE_BYTE | DMAC_CRCPOLY_CRC16 | DMAC_CRCMODE_CRCGEN | DMAC_CRCSRC(0x20 + _chnum);
	//	WritePeripheral(data, (void*)init, len, DMCH_TRIGACT_TRANSACTION, DMDSC_BEATSIZE_BYTE);
	//	SoftwareTrigger(); //HW::DMAC->SWTRIGCTRL = 1UL << CRC_DMACH;
	//}

#elif defined(CPU_SAM4SA) //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	DMA_CH(T_HW::S_PDC* ch) : _dmach(ch) { }

	void Disable() { _dmach->PTCR = PDC_RXTDIS|PDC_TXTDIS; }
	//void Reset() { _dmach->PTCR = 0x202; }
	//void Suspend() { _dmach->CTRLB = DMCH_CMD_SUSPEND; }

	bool CheckWriteComplete()	{ return _dmach->TCR == 0 && _dmach->TNCR == 0; }
	bool CheckReadComplete()	{ return _dmach->RCR == 0 && _dmach->RNCR == 0; }
	bool CheckComplete()		{ return CheckWriteComplete() && CheckReadComplete(); }

	void Update() {  }

	//void MemCopy(volatile void *src, volatile void *dst, u16 len)		{ _MemCopy((byte*)src+len, (byte*)dst+len, len, DMDSC_VALID|DMDSC_BEATSIZE_BYTE|DMDSC_DSTINC|DMDSC_SRCINC); }
	//void MemCopySrcInc(volatile void *src, volatile void *dst, u16 len) { _MemCopy((byte*)src+len, dst, len, DMDSC_VALID|DMDSC_BEATSIZE_BYTE|DMDSC_SRCINC); }
	//void MemCopyDstInc(volatile void *src, volatile void *dst, u16 len) { _MemCopy(src, (byte*)dst+len, len, DMDSC_VALID|DMDSC_BEATSIZE_BYTE|DMDSC_DSTINC); }
	//bool CheckMemCopyComplete() { return CheckComplete(); }

	//u32 GetSrcBytesReady()	{ return 0; }
	//u32 GetDstBytesReady()	{ return 0; }
	//u32 GetSrcBytesLeft()	{ return 0; }
	//u32 GetDstBytesLeft()	{ return 0; }

	u16 GetWriteBytesLeft()	{ return _dmach->TCR; }
	u16 GetReadBytesLeft()	{ return _dmach->RCR; }
	//u32 GetBytesReady()	{ return _dmach->TCR; }

	//void SoftwareTrigger() { HW::DMAC->SWTRIGCTRL = 1UL<<_chnum; }
	//void SetEvCtrl(byte v) { _dmach->EVCTRL = v; }
	//void SetPriLvl(byte v) { _dmach->PRILVL = v; }

	void WritePeripheral(const volatile void *src, u16 len, const volatile void *src2, u16 len2);
	void ReadPeripheral(volatile void *dst, u16 len, volatile void *dst2, u16 len2);


#elif defined(CPU_XMC48)	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	DMA_CH(byte chnum) : _GPDMA((chnum>7) ? HW::DMA1 : HW::DMA0), _dmach(&_GPDMA->CH[chnum&7]), _chnumabs(chnum), _chnum(chnum&7) {}

	void Enable() { _GPDMA->CHENREG = ((1<<GPDMA0_CHENREG_CH_Pos)|(1<<GPDMA0_CHENREG_WE_CH_Pos)) << _chnum; }
	void Disable() { _GPDMA->CHENREG = (1<<GPDMA0_CHENREG_WE_CH_Pos) << _chnum; HW::DLR->LNEN &= ~_dlr_lnen_mask; }
	void Reset() {  }
	bool CheckComplete() { return (_GPDMA->CHENREG & ((1<<GPDMA0_CHENREG_CH_Pos)<<_chnum)) == 0; }
	void SetDlrLineNum(byte num) { _dlr_lnen_mask = 1UL << (_drl = num);  }
	void Enable_DLR() { HW::DLR->LNEN |= _dlr_lnen_mask; }
	void Disable_DLR() { HW::DLR->LNEN &= ~_dlr_lnen_mask; }
	
	void WritePeripheralByte(const volatile void *src, volatile void *dst, u16 len)	{ WritePeripheral(src, dst, len, DST_TR_WIDTH_8|SRC_TR_WIDTH_8|DST_NOCHANGE|SRC_INC|DEST_MSIZE_1|SRC_MSIZE_1|TT_FC_M2P_GPDMA|LLP_DST_EN, 0); }
	void ReadPeripheralByte(const volatile void *src, volatile void *dst, u16 len)	{ ReadPeripheral(src, dst, len, DST_TR_WIDTH_8|SRC_TR_WIDTH_8|DST_INC|SRC_NOCHANGE|DEST_MSIZE_1|SRC_MSIZE_1|TT_FC_P2M_GPDMA|LLP_SRC_EN, 0); }
	
	void MemCopy(const volatile void *src, volatile void *dst, u16 len)		{ _MemCopy(src, dst, len, DST_INC|SRC_INC|TT_FC_M2M_GPDMA|DEST_MSIZE_1|SRC_MSIZE_1|LLP_DST_EN|LLP_SRC_EN); }
	void MemCopySrcInc(const volatile void *src, volatile void *dst, u16 len) { _MemCopy(src, dst, len, DST_NOCHANGE|SRC_INC|TT_FC_M2M_GPDMA|DEST_MSIZE_1|SRC_MSIZE_1|LLP_SRC_EN); }
	void MemCopyDstInc(const volatile void *src, volatile void *dst, u16 len) { _MemCopy(src, dst, len, DST_INC|SRC_NOCHANGE|TT_FC_M2M_GPDMA|DEST_MSIZE_1|SRC_MSIZE_1|LLP_DST_EN); }
	bool CheckMemCopyComplete();

	u32 GetSrcCounter()	{ return _dmach->SAR - _startSrcAdr; }
	u32 GetDstCounter()	{ return _dmach->DAR - _startDstAdr; }

#elif defined(CPU_LPC824)	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	DMA_CH(byte chnum) : _dmach(HW::DMA->CH+chnum), _dmadsc(_DmaTable+chnum), _act_mask(1UL<<chnum) /*, _chnum(chnum)*/ {}

	void Enable() {  }
	void Disable() { HW::DMA->ENABLECLR0 = _act_mask; HW::DMA->ABORT0 = _act_mask; HW::DMA->ERRINT0 = _act_mask; }
	bool CheckComplete() { return (HW::DMA->ACTIVE0 & _act_mask) == 0; }
	u32 GetBytesLeft()	{ return ((_dmach->XFERCFG >> 16) + 1) & 0x3FF; }

	void WritePeripheralByte(const volatile void *src, volatile void *dst, u16 len)	{ WritePeripheral(src, dst, len, PERIPHREQEN | TRIGBURST_SNGL | CHPRIORITY(3), CFGVALID | SETINTA | SWTRIG | WIDTH_8 | SRCINC_1 | DSTINC_0); }
	void ReadPeripheralByte(const volatile void *src, volatile void *dst, u16 len)	{ ReadPeripheral(src, dst, len, PERIPHREQEN | TRIGBURST_SNGL | CHPRIORITY(3), CFGVALID | SWTRIG | WIDTH_8 | SRCINC_0 | DSTINC_1); }

#else

	DMA_CH() /*: _chnum(0)*/ {}

#endif

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef CPU_SAME53

//extern DMA_CH		DMA_CH0	;
//extern DMA_CH		DMA_CH1	;
//extern DMA_CH		DMA_CH2	;
//extern DMA_CH		DMA_CH3	;
//extern DMA_CH		DMA_CH4	;
//extern DMA_CH		DMA_CH5	;
//extern DMA_CH		DMA_CH6	;
//extern DMA_CH		DMA_CH7	;
//extern DMA_CH		DMA_CH8	;
//extern DMA_CH		DMA_CH9	;
//extern DMA_CH		DMA_CH10;
//extern DMA_CH		DMA_CH11;
//extern DMA_CH		DMA_CH12;
//extern DMA_CH		DMA_CH13;
//extern DMA_CH		DMA_CH14;
//extern DMA_CH		DMA_CH15;
//extern DMA_CH		DMA_CH16;
//extern DMA_CH		DMA_CH17;
//extern DMA_CH		DMA_CH18;
//extern DMA_CH		DMA_CH19;
//extern DMA_CH		DMA_CH20;
//extern DMA_CH		DMA_CH21;
//extern DMA_CH		DMA_CH22;
//extern DMA_CH		DMA_CH23;
//extern DMA_CH		DMA_CH24;
//extern DMA_CH		DMA_CH25;
//extern DMA_CH		DMA_CH26;
//extern DMA_CH		DMA_CH27;
//extern DMA_CH		DMA_CH28;
//extern DMA_CH		DMA_CH29;
//extern DMA_CH		DMA_CH30;
//extern DMA_CH		DMA_CH31;

#elif defined(CPU_XMC48)

//extern DMA_CH		DMA_CH0;
//extern DMA_CH		DMA_CH1;
//extern DMA_CH		DMA_CH2;
//extern DMA_CH		DMA_CH3;
//extern DMA_CH		DMA_CH4;
//extern DMA_CH		DMA_CH5;
//extern DMA_CH		DMA_CH6;
//extern DMA_CH		DMA_CH7;
//extern DMA_CH		DMA_CH8;
//extern DMA_CH		DMA_CH9;
//extern DMA_CH		DMA_CH10;
//extern DMA_CH		DMA_CH11;

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // #if !defined(CPU_LPC812)

#endif // DMA_H__26_05_2022__18_11
