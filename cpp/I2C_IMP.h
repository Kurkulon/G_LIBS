#ifndef I2C_IMP_H__17_04_2025__16_10
#define I2C_IMP_H__17_04_2025__16_10

#pragma once

#include "i2c.h"
#include "core.h"
#include "SEGGER_RTT\SEGGER_RTT.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef __ADSPBF59x__ //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//#pragma diag(push)
//#pragma diag(suppress: 1970)

static volatile u16 twiWriteCount = 0;
static volatile u16 twiReadCount = 0;
static volatile byte * volatile twiWriteData = 0;
static volatile byte * volatile twiReadData;
static volatile DSCI2C* volatile twi_dsc = 0;
static volatile DSCI2C* volatile twi_lastDsc = 0;

EX_INTERRUPT_HANDLER(TWI_ISR)
{
	u16 stat = *pTWI_INT_STAT;

	if (stat & RCVSERV)
	{
		if (twiReadCount > 0)
		{
			*twiReadData++ = *pTWI_RCV_DATA8;
			twiReadCount--;
		};

		if (twiReadCount == 0)
		{
			//*pTWI_INT_MASK = MERR|MCOMP;
			*pTWI_MASTER_CTL |= STOP;
			*pTWI_FIFO_CTL = XMTFLUSH|RCVFLUSH;
		};
	};
	
	if (stat & XMTSERV)
	{
		if (twiWriteCount == 0 && twi_dsc->wlen2 != 0)
		{
			twiWriteData = (byte*)twi_dsc->wdata2;
			twiWriteCount = twi_dsc->wlen2;
			twi_dsc->wlen2 = 0;
		};

		if (twiWriteCount > 0)
		{
			*pTWI_XMT_DATA8 = *twiWriteData++;
			twiWriteCount--;

		};
		//else if (twiReadCount > 0)
		//{
		//	*pTWI_INT_MASK = MERR|MCOMP;
		//	*pTWI_MASTER_CTL |= RSTART|MDIR;
		//}
	};
	
	//if (stat & MERR)
	//{
	//	*pTWI_INT_STAT = MERR;
	//};

	if (stat & (MCOMP|MERR))
	{
		twi_dsc->ack = ((stat & MERR) == 0);

		if (twi_dsc->ack && twiReadCount > 0)
		{
			*pTWI_INT_MASK = RCVSERV|MERR|MCOMP;
			*pTWI_MASTER_CTL = ((twiReadCount<<6)&DCNT)|MDIR|FAST|MEN;
		}
		else
		{
			twi_dsc->ready = true;
			twi_dsc->readedLen = twi_dsc->rlen - twiReadCount;
			//twi_dsc->master_stat = *pTWI_MASTER_STAT;

			DSCI2C *ndsc = twi_dsc->next;

			if (ndsc != 0)
			{
				twi_dsc->next = 0;
				twi_dsc = ndsc;

				twi_dsc->ready = false;
				twi_dsc->ack = false;
				twi_dsc->readedLen = 0;

				if (twi_dsc->wdata2 == 0) twi_dsc->wlen2 = 0;

				twiWriteData = (byte*)twi_dsc->wdata;
				twiWriteCount = twi_dsc->wlen;
				twiReadData = (byte*)twi_dsc->rdata;
				twiReadCount = twi_dsc->rlen;

				u16 len = twiWriteCount + twi_dsc->wlen2;

				*pTWI_MASTER_STAT = ~0;
				*pTWI_FIFO_CTL = 0;

				*pTWI_MASTER_ADDR = twi_dsc->adr;

				if (len != 0)
				{
					*pTWI_XMT_DATA8 = *twiWriteData++; twiWriteCount--;
					*pTWI_INT_MASK = XMTSERV|MERR|MCOMP;
					*pTWI_MASTER_CTL = ((len<<6)&DCNT)|FAST|MEN|((twiReadCount>0) ? RSTART : 0);
				}
				else
				{
					*pTWI_INT_MASK = RCVSERV|MERR|MCOMP;
					*pTWI_MASTER_CTL = (twiReadCount<<6)|MDIR|FAST|MEN;
				};
			}
			else
			{
				*pTWI_MASTER_CTL = 0;
				*pTWI_MASTER_STAT = ~0;
				*pTWI_FIFO_CTL = XMTFLUSH|RCVFLUSH;

				*pTWI_INT_MASK = 0;

				twi_lastDsc = twi_dsc = 0;
			};

		};
	};

	*pTWI_INT_STAT = stat;

	//ssync();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void I2C_Init(u32 sclk_mhz, u32 ivg_twi, u32 pid_twi)
{
	*pTWI_CONTROL = TWI_ENA | (sclk_mhz/10);
	*pTWI_CLKDIV = CLKHI(150/(sclk_mhz/10))|CLKLOW(150/(sclk_mhz/10));
	*pTWI_INT_MASK = 0;
	*pTWI_MASTER_ADDR = 0;

	InitIVG(ivg_twi, pid_twi, TWI_ISR);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool TWI_Write(DSCI2C *d)
{
//	using namespace HW;

	if (twi_dsc != 0 || d == 0) { return false; };
	if ((d->wdata == 0 || d->wlen == 0) && (d->rdata == 0 || d->rlen == 0)) { return false; }

	twi_dsc = d;

	twi_dsc->ready = false;
	twi_dsc->ack = false;
	twi_dsc->readedLen = 0;

	if (twi_dsc->wdata2 == 0) twi_dsc->wlen2 = 0;

	u32 t = cli();

	*pTWI_MASTER_CTL = 0;
	*pTWI_MASTER_STAT = ~0;
	*pTWI_FIFO_CTL = 0;//XMTINTLEN|RCVINTLEN;

	twiWriteData = (byte*)twi_dsc->wdata;
	twiWriteCount = twi_dsc->wlen;
	twiReadData = (byte*)twi_dsc->rdata;
	twiReadCount = twi_dsc->rlen;

	u16 len = twiWriteCount + twi_dsc->wlen2;

	*pTWI_MASTER_ADDR = twi_dsc->adr;

	if (len != 0)
	{
		*pTWI_XMT_DATA8 = *twiWriteData++; twiWriteCount--;
		*pTWI_INT_MASK = XMTSERV|MERR|MCOMP;
		*pTWI_MASTER_CTL = ((len<<6)&DCNT)|FAST|MEN|((twiReadCount>0) ? RSTART : 0);
	}
	else
	{
		*pTWI_INT_MASK = RCVSERV|MERR|MCOMP;
		*pTWI_MASTER_CTL = (twiReadCount<<6)|MDIR|FAST|MEN;
	};

	sti(t);

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool I2C_AddRequest(DSCI2C *d)
{
	if (d == 0) { return false; };
	if ((d->wdata == 0 || d->wlen == 0) && (d->rdata == 0 || d->rlen == 0)) { return false; }

	d->next = 0;
	d->ready = false;

	if (d->wdata2 == 0) d->wlen2 = 0;

	u32 t = cli();

	if (twi_lastDsc == 0)
	{
		twi_lastDsc = d;

		sti(t);

		return TWI_Write(d);
	}
	else
	{
		twi_lastDsc->next = d;
		twi_lastDsc = d;

		sti(t);
	};

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#elif defined(__ADSPBF70x__)

static volatile u16 twiWriteCount = 0;
static volatile u16 twiReadCount = 0;
static volatile byte * volatile twiWriteData = 0;
static volatile byte * volatile twiReadData;
static volatile DSCI2C* volatile twi_dsc = 0;
static volatile DSCI2C* volatile twi_lastDsc = 0;

//#pragma optimize_for_speed

SEC_INTERRUPT_HANDLER(TWI_ISR)
//void TWI_ISR()
{
	u16 stat = HW::TWI->ISTAT;

	//HW::PIOB->DATA_SET = PB4;

	if (stat & TWI_RXSERV)
	{
		if (twiReadCount > 0)
		{
			*twiReadData++ = HW::TWI->RXDATA8;
			twiReadCount--;
		};

		if (twiReadCount == 0)
		{

			HW::TWI->MSTRCTL |= TWI_MST_STOP;
			HW::TWI->FIFOCTL  = TWI_TXFLUSH|TWI_RXFLUSH;
		};
	};

	if (stat & TWI_TXSERV)
	{
		if (twiWriteCount == 0 && twi_dsc->wlen2 != 0)
		{
			twiWriteData = (byte*)twi_dsc->wdata2;
			twiWriteCount = twi_dsc->wlen2;
			twi_dsc->wlen2 = 0;
		};

		if (twiWriteCount > 0)
		{
			HW::TWI->TXDATA8 = *twiWriteData++;
			twiWriteCount--;

		};
	};

	if (stat & (TWI_MCOMP|TWI_MERR))
	{
		twi_dsc->ack = ((stat & TWI_MERR) == 0);

		if (twi_dsc->ack && twiReadCount > 0)
		{
			HW::TWI->IMSK		= TWI_RXSERV|TWI_MCOMP|TWI_MERR;
			HW::TWI->MSTRCTL	= TWI_MST_DCNT(twiReadCount)|TWI_MST_DIR|TWI_MST_FAST|TWI_MST_EN;
		}
		else
		{
			twi_dsc->ready = true;
			twi_dsc->readedLen = twi_dsc->rlen - twiReadCount;
			//twi_dsc->master_stat = *pTWI_MASTER_STAT;

			DSCI2C *ndsc = twi_dsc->next;

			if (ndsc != 0)
			{
				twi_dsc->next = 0;
				twi_dsc = ndsc;

				twi_dsc->ready = false;
				twi_dsc->ack = false;
				twi_dsc->readedLen = 0;

				if (twi_dsc->wdata2 == 0) twi_dsc->wlen2 = 0;

				twiWriteData = (byte*)twi_dsc->wdata;
				twiWriteCount = twi_dsc->wlen;
				twiReadData = (byte*)twi_dsc->rdata;
				twiReadCount = twi_dsc->rlen;

				u16 len = twiWriteCount + twi_dsc->wlen2;

				HW::TWI->MSTRSTAT	= ~0;
				HW::TWI->FIFOCTL	= 0;

				HW::TWI->MSTRADDR = twi_dsc->adr;

				if (len != 0)
				{
					HW::TWI->TXDATA8	= *twiWriteData++; twiWriteCount--;
					HW::TWI->IMSK		= TWI_TXSERV|TWI_MCOMP|TWI_MERR;
					HW::TWI->MSTRCTL	= TWI_MST_DCNT(len)|TWI_MST_FAST|TWI_MST_EN;
				}
				else
				{
					HW::TWI->IMSK		= TWI_RXSERV|TWI_MCOMP|TWI_MERR;
					HW::TWI->MSTRCTL	= TWI_MST_DCNT(twiReadCount)|TWI_MST_DIR|TWI_MST_FAST|TWI_MST_EN;
				};
			}
			else
			{
				HW::TWI->MSTRCTL	= 0;
				HW::TWI->MSTRSTAT	= ~0;
				HW::TWI->FIFOCTL	= TWI_TXFLUSH|TWI_RXFLUSH;
				HW::TWI->IMSK		= 0;

				twi_lastDsc = twi_dsc = 0;
			};

		};
	};

	HW::TWI->ISTAT = stat;

	//HW::PIOB->DATA_CLR = PB4;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma never_inline
void I2C_Init()
{
	//u32 sclk_mhz = Get_SCLK0_MHz();

	HW::TWI->CTL		= TWI_CTL_EN | TWI_CTL_PRESCALE(SCLK0_MHz/10);
	HW::TWI->CLKDIV		= TWI_CLKHI(10)|TWI_CLKLO(12);
	HW::TWI->IMSK		= 0;
	HW::TWI->MSTRADDR	= 0;

#ifdef __ADSPBF60x__
	InitSEC(PID_TWI0_DATA, TWI_ISR, 0);
#else
	InitSEC(PID_TWI0_DATA, TWI_ISR);
#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool TWI_Write(DSCI2C *d)
{
	//	using namespace HW;

	if (twi_dsc != 0 || d == 0) { return false; };
	if ((d->wdata == 0 || d->wlen == 0) && (d->rdata == 0 || d->rlen == 0)) { return false; }

	twi_dsc = d;

	twi_dsc->ready = false;
	twi_dsc->ack = false;
	twi_dsc->readedLen = 0;

	if (twi_dsc->wdata2 == 0) twi_dsc->wlen2 = 0;

	u32 t = cli();

	HW::TWI->MSTRCTL	= 0;
	HW::TWI->MSTRSTAT	= ~0;
	HW::TWI->FIFOCTL	= 0;//XMTINTLEN|RCVINTLEN;

	twiWriteData = (byte*)twi_dsc->wdata;
	twiWriteCount = twi_dsc->wlen;
	twiReadData = (byte*)twi_dsc->rdata;
	twiReadCount = twi_dsc->rlen;

	u16 len = twiWriteCount + twi_dsc->wlen2;

	HW::TWI->MSTRADDR = twi_dsc->adr;
	HW::TWI->FIFOCTL = 0;

	if (len != 0)
	{
		HW::TWI->TXDATA8 = *twiWriteData++; twiWriteCount--;
		HW::TWI->IMSK = TWI_TXSERV|TWI_MERR|TWI_MCOMP;
		HW::TWI->MSTRCTL = TWI_MST_DCNT(len)|TWI_MST_FAST|TWI_MST_EN;
	}
	else
	{
		HW::TWI->IMSK = TWI_RXSERV|TWI_MERR|TWI_MCOMP;
		HW::TWI->MSTRCTL = TWI_MST_DCNT(twiReadCount)|TWI_MST_DIR|TWI_MST_FAST|TWI_MST_EN;
	};

	sti(t);

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma never_inline

bool I2C_AddRequest(DSCI2C *d)
{
	if (d == 0) { return false; };
	if ((d->wdata == 0 || d->wlen == 0) && (d->rdata == 0 || d->rlen == 0)) { return false; }

	d->next = 0;
	d->ready = false;

	if (d->wdata2 == 0) d->wlen2 = 0;

	u32 t = cli();

	if (twi_lastDsc == 0)
	{
		twi_lastDsc = d;

		sti(t);

		return TWI_Write(d);
	}
	else
	{
		twi_lastDsc->next = d;
		twi_lastDsc = d;

		sti(t);
	};

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#else // #ifdef ADSP_BLACKFIN //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef CORETYPE_LPC8XX

	S_I2C *S_I2C::_i2c0 = 0;
	__irq void S_I2C::I2C0_Handler() { if (_i2c0 != 0) _i2c0->IRQ_Handler(); }

	#ifdef CPU_LPC824

		S_I2C *S_I2C::_i2c1 = 0;
		S_I2C *S_I2C::_i2c2 = 0;
		S_I2C *S_I2C::_i2c3 = 0;

		__irq void S_I2C::I2C1_Handler() { if (_i2c1 != 0) _i2c1->IRQ_Handler(); }
		__irq void S_I2C::I2C2_Handler() { if (_i2c2 != 0) _i2c2->IRQ_Handler(); }
		__irq void S_I2C::I2C3_Handler() { if (_i2c3 != 0) _i2c3->IRQ_Handler(); }

	#endif

void S_I2C::IRQ_Handler()
{
	using namespace HW;

	T_HW::S_TWI* uhw = _uhw.i2c;

	if(uhw->INTSTAT & MSTPENDING)
	{
		uhw->STAT = MSTPENDING;

		u32 state = uhw->STAT & MSTSTATE;

		if(state == MSTST_IDLE) // Address plus R/W received
		{
			if (rlen == 0 && wlen == 0)
			{
				_dsc->readedLen = rdPtr - (byte*)_dsc->rdata;
				_dsc->ready = true;

				_dsc = _reqList.Get();

				if (_dsc != 0) 
				{
					Write(_dsc);
				}
				else
				{
					uhw->INTENCLR = MSTPENDING;
					uhw->CFG = 0;
				};
			};
		}
		else if(state == MSTST_RX) // Received data is available
		{
			*rdPtr++ = uhw->MSTDAT; // receive data

			rlen--;

			uhw->MSTCTL = (rlen > 0) ? MSTCONTINUE : MSTSTOP; 
		}
		else if(state == MSTST_TX) // Data can be transmitted 
		{
			_dsc->ack = true;

			if (wlen > 0)
			{
				uhw->MSTDAT = *wrPtr++;
				uhw->MSTCTL = MSTCONTINUE;
				wlen--;

				if(wlen == 0 && wlen2 != 0)
				{
					wrPtr = wrPtr2;
					wlen = wlen2;
					wlen2 = 0;
				};
			}
			else if (rlen > 0)
			{
				uhw->MSTDAT = (_dsc->adr << 1) | 1;
				uhw->MSTCTL = MSTSTART;
			}
			else
			{
				uhw->MSTCTL = MSTSTOP;
			};
		}
		else
		{
			rlen = 0;
			wlen = 0;

			uhw->MSTCTL = MSTSTOP;
		};
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void S_I2C::Write(DSCI2C *d)
{
	using namespace HW;

	_dsc = d;

	d->ready = false;

	wrPtr	= (byte*)d->wdata;	
	rdPtr	= (byte*)d->rdata;	
	wrPtr2	= (byte*)d->wdata2;	
	wlen	= d->wlen;
	wlen2	= d->wlen2;
	rlen	= d->rlen;

	T_HW::S_TWI* uhw = _uhw.i2c;

	uhw->INTENSET = MSTPENDING;
	uhw->CFG = _CFG|MSTEN;

	uhw->MSTDAT = (d->adr << 1) | ((wlen == 0) ? 1 : 0);
	uhw->MSTCTL = MSTSTART;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // #if defined(CPU_LPC824) || defined(CPU_LPC812)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void S_I2C::InitHW()
{
#ifdef CPU_SAME53	

	HW::GCLK->PCHCTRL[_ugclk] = GCLK_GEN(_GEN_SRC)|GCLK_CHEN;	// 25 MHz

	HW::MCLK->ClockEnable(_upid);

	_PIO_SCL->SetWRCONFIG(_MASK_SCL, _PMUX_SCL | PORT_PMUXEN | PORT_WRPMUX | PORT_PULLEN | PORT_WRPINCFG);
	_PIO_SDA->SetWRCONFIG(_MASK_SDA, _PMUX_SDA | PORT_PMUXEN | PORT_WRPMUX | PORT_PULLEN | PORT_WRPINCFG);
	_PIO_SCL->SET(_MASK_SCL);
	_PIO_SDA->SET(_MASK_SDA);

	T_HW::S_I2C* i2c = _uhw.i2c;

	i2c->CTRLA = I2C_SWRST;

	while(i2c->SYNCBUSY);

	i2c->CTRLA = SERCOM_MODE_I2C_MASTER;

	i2c->CTRLA = SERCOM_MODE_I2C_MASTER|I2C_INACTOUT_205US|I2C_SPEED_SM;
	i2c->CTRLB = I2C_SMEN;

	i2c->BAUD = _baud;

	i2c->CTRLA |= I2C_ENABLE;

	while(i2c->SYNCBUSY);

	i2c->STATUS = 0;
	i2c->STATUS.BUSSTATE = BUSSTATE_IDLE;
	
#elif defined(CPU_SAM4SA)

	HW::PMC->ClockEnable(_upid);

	if(_usic_num == 1) // TWI0
	{
		HW::PIOA->PDR		=	PA3|PA4;
		HW::PIOA->ABCDSR1	&=	~(PA3|PA4);
		HW::PIOA->ABCDSR2	&=	~(PA3|PA4);
	}
	else if(_usic_num == 2)  // TWI1
	{
		HW::PIOB->PDR		=	PB4|PB5;
		HW::PIOB->ABCDSR1	&=	~(PB4|PB5);
		HW::PIOB->ABCDSR2	&=	~(PB4|PB5);
	};

	_uhw->i2c.CWGR = _baud;

#elif defined(CPU_XMC48)

		HW::Peripheral_Enable(_upid);

 	//	P5->ModePin0(A1OD);
		//P5->ModePin2(A1PP);

		_uhw->KSCFG = USIC_MODEN|USIC_BPMODEN|USIC_BPNOM|USIC_NOMCFG(0);

		__dsb(15);

		_uhw->SCTR = USIC_SDIR(1) | USIC_TRM(3) | USIC_FLE(0x3F) | USIC_WLE(7);

		_uhw->FDR = _FDR;
		_uhw->BRG = USIC_DCTQ(24)|USIC_SCLKCFG(0);
	    
		_uhw->TCSR = USIC_TDEN(1)|USIC_TDSSM(1);

		_uhw->PSCR = ~0;

		_uhw->CCR = 0;

		_uhw->DX0CR = _DX0CR;
		_uhw->DX1CR = _DX1CR;

		_uhw->CCR = USIC_MODE_I2C;

		_uhw->PCR_IICMode = I2C_STIM;

		_PIO_SCL->ModePin(_PIN_SCL, _MUX_SCL);
		_PIO_SDA->ModePin(_PIN_SDA, _MUX_SDA);

		//VectorTableExt[I2C_IRQ] = I2C_Handler;
		//CM4::NVIC->CLR_PR(I2C_IRQ);
		//CM4::NVIC->SET_ER(I2C_IRQ);

#elif defined(CORETYPE_LPC8XX)

	#ifndef I2C0_IRQ
	#define I2C0_IRQ I2C_IRQ
	#endif

	HW::SYSCON->SYSAHBCLKCTRL |= 1UL<<_upid;

	if(_usic_num == 5)
	{
		HW::IOCON->PIO0_10.B.I2CMODE = 0;
		HW::IOCON->PIO0_11.B.I2CMODE = 0;

		_i2c0 = this;

		VectorTableExt[I2C0_IRQ] = I2C0_Handler;
		CM0::NVIC->CLR_PR(I2C0_IRQ);
		CM0::NVIC->SET_ER(I2C0_IRQ);
	}
	else
	{
	#ifdef CPU_LPC824
		IOCON_PIN_INDEX_TABLE_DEF;

		T_HW::S_IOCON::S_PIO *iocon = &(HW::IOCON->PIO0_17);

		(iocon+iocon_Pin_Index[_PIN_SCL])->B.OD = 1;
		(iocon+iocon_Pin_Index[_PIN_SDA])->B.OD = 1;

		if(_usic_num == 6)
		{
			HW::SWM->I2C1_SCL	= _PIN_SCL;
			HW::SWM->I2C1_SDA	= _PIN_SDA;

			_i2c1 = this;

			VectorTableExt[I2C1_IRQ] = I2C1_Handler;
			CM0::NVIC->CLR_PR(I2C1_IRQ);
			CM0::NVIC->SET_ER(I2C1_IRQ);
		}
		else if(_usic_num == 7)
		{
			HW::SWM->I2C2_SCL	= _PIN_SCL;
			HW::SWM->I2C2_SDA	= _PIN_SDA;

			_i2c2 = this;

			VectorTableExt[I2C2_IRQ] = I2C2_Handler;
			CM0::NVIC->CLR_PR(I2C2_IRQ);
			CM0::NVIC->SET_ER(I2C2_IRQ);
		}
		else if(_usic_num == 8)
		{
			HW::SWM->I2C3_SCL	= _PIN_SCL;
			HW::SWM->I2C3_SDA	= _PIN_SDA;

			_i2c3 = this;

			VectorTableExt[I2C3_IRQ] = I2C3_Handler;
			CM0::NVIC->CLR_PR(I2C3_IRQ);
			CM0::NVIC->SET_ER(I2C3_IRQ);
		};
	#endif
	};

	T_HW::S_TWI* &uhw = _uhw.i2c;

	uhw->CLKDIV = _CLKDIV;
	uhw->MSTTIME = _MSTTIME;
	uhw->CFG = _CFG;

#elif defined(__ADSPBF60x__)

	_hw->CTL		= TWI_CTL_EN | TWI_CTL_PRESCALE(SCLK0_MHz/10);
	_hw->CLKDIV		= TWI_CLKHI(10)|TWI_CLKLO(12);
	_hw->IMSK		= 0;
	_hw->MSTRADDR	= 0;

#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool S_I2C::Connect(u32 baudrate)
{
#ifndef WIN32

	using namespace HW;

	SEGGER_RTT_printf(0, RTT_CTRL_TEXT_BRIGHT_CYAN "I2C%u Init ... ", _usic_num);

	if (!Usic_Connect())
	{
		SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_RED "!!! ERROR !!!\n");

		return false;
	};

	if (baudrate == 0) baudrate = 1;

	#ifdef CPU_SAME53	

		u32 baud = (_GEN_CLK + baudrate/2) / baudrate;

		u32 trim = 10 + (_GEN_CLK+1500000)/3000000;

		baud = (baud > trim) ? ((baud - trim + 1) / 2) : 1;

		if (baud > 0xFF) baud = 0xFF;

		_baud = baud;

		InitHW();

	#elif defined(CPU_SAM4SA)
		
		u32 t = (_GEN_CLK + baudrate/2) / baudrate / 2;

		_baud = TWI_CWGR_CLDIV(t)|TWI_CWGR_CHDIV(t);

		InitHW();

	#elif defined(CPU_XMC48)

		_FDR = (1024 - (((_GEN_CLK + baudrate/2) / baudrate + 8) / 16)) | USIC_DM(1);

		InitHW();

	#elif defined(CORETYPE_LPC8XX)

		baudrate *= 4;

		_CLKDIV = (_GEN_CLK + baudrate/2) / baudrate;
		if (_CLKDIV > 0) _CLKDIV -= 1;
		_MSTTIME = 0;
		_CFG = 0;

		InitHW();

	#elif defined(__ADSPBF60x__)

		InitHW();

	#endif

#else

	//HANDLE h;

	//h = CreateFile("FRAM_I2C_STORE.BIN", GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);

	//if (h == INVALID_HANDLE_VALUE)
	//{
	//	return false;
	//};

	//dword bytes;

	//ReadFile(h, fram_I2c_Mem, sizeof(fram_I2c_Mem), &bytes, 0);
	//CloseHandle(h);

#endif

	SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_GREEN "OK\n");

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool S_I2C::AddRequest(DSCI2C *d)
{
#ifndef WIN32

	if (d == 0) { return false; };
	if ((d->wdata == 0 || d->wlen == 0) && (d->rdata == 0 || d->rlen == 0)) { return false; }

	d->next = 0;
	d->ready = false;

	if (d->wdata2 == 0 || d->wlen2 == 0) d->wdata2 = 0, d->wlen2 = 0;

	#if defined(CORETYPE_LPC8XX)
		__disable_irq(); if (_dsc == 0)	Write(d); else _reqList.Add(d); __enable_irq();
	#else
		_reqList.Add(d);
	#endif

#endif

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool S_I2C::Update()
{
	bool result = false;

#ifdef CPU_SAME53	

	//static byte *wrPtr = 0;
	//static byte *rdPtr = 0;
	//static u16 	wrCount = 0;
	//static u16 	rdCount = 0;
	//static byte *wrPtr2 = 0;
	//static u16	wrCount2 = 0;
	//static byte adr = 0;

	T_HW::S_I2C* i2c = _uhw.i2c;

	switch (_state)
	{
		case I2C_WAIT:

			if (CheckReset())
			{
				Usic_Update();
			}
			else
			{
				_dsc = _reqList.Get();

				if (_dsc != 0)
				{
					Usic_Lock();

					DSCI2C &dsc = *_dsc;

					dsc.ready = false;
					dsc.ack = false;
					dsc.readedLen = 0;

					i2c->CTRLB = I2C_SMEN;
					i2c->STATUS.BUSSTATE = BUSSTATE_IDLE;

					i2c->INTFLAG = ~0;

					if (_dsc->wlen == 0)
					{
						_DMA.ReadPeripheral(&i2c->DATA, _dsc->rdata, _dsc->rlen, DMCH_TRIGACT_BURST|(((DMCH_TRIGSRC_SERCOM0_RX>>8)+_usic_num*2)<<8), DMDSC_BEATSIZE_BYTE);

						i2c->ADDR = ((_dsc->rlen <= 255) ? (I2C_LEN(_dsc->rlen)|I2C_LENEN) : 0) | (_dsc->adr << 1) | 1;
						_state = I2C_READ; 
					}
					else
					{
						i2c->ADDR = (_dsc->adr << 1);

						_DMA.WritePeripheral(_dsc->wdata, &i2c->DATA, _dsc->wlen, _dsc->wdata2, _dsc->wlen2, DMCH_TRIGACT_BURST|(((DMCH_TRIGSRC_SERCOM0_TX>>8)+_usic_num*2)<<8), DMDSC_BEATSIZE_BYTE);

						_state = I2C_WRITE; 
					};

					_prevCount = 0;
					_tm.Reset();
				};
			};

			break;

		case I2C_WRITE:

			if(i2c->INTFLAG & I2C_ERROR) // || i2c->STATUS.RXNACK)
			{
				i2c->CTRLB = I2C_SMEN|I2C_CMD_STOP;
				
				_state = I2C_STOP; 
			}
			else if (_tm.Timeout(100))
			{
				i2c->CTRLB = I2C_SMEN|I2C_CMD_STOP;
				
				_state = I2C_RESET; 
			}
			else
			{
				DSCI2C &dsc = *_dsc;

				__disable_irq();

				bool c = _DMA.CheckComplete() && (i2c->INTFLAG & I2C_MB);
				
				__enable_irq();

				if (c)
				{
					_DMA.Disable();

					dsc.ack = (i2c->STATUS.RXNACK == 0);

					if (dsc.ack && _dsc->rlen > 0)
					{
						_DMA.ReadPeripheral(&i2c->DATA, _dsc->rdata, _dsc->rlen, DMCH_TRIGACT_BURST|(((DMCH_TRIGSRC_SERCOM0_RX>>8)+_usic_num*2)<<8), DMDSC_BEATSIZE_BYTE);

						i2c->ADDR = ((_dsc->rlen <= 255) ? (I2C_LEN(_dsc->rlen)|I2C_LENEN) : 0) | (_dsc->adr << 1) | 1;
		
						_state = I2C_READ; 
					}
					else
					{
						i2c->CTRLB = I2C_SMEN|I2C_ACKACT|I2C_CMD_STOP;
						
						_state = I2C_STOP; 
					};

					_prevCount = 0;
					_tm.Reset();
				}
				else
				{
					u32 t = _DMA.GetBytesLeft();

					if (t != _prevCount) _prevCount = t, _tm.Reset();
				};
			};

			break;

		case I2C_READ:

			if(i2c->INTFLAG & I2C_ERROR) // || i2c->STATUS.RXNACK)
			{
				i2c->CTRLB = I2C_SMEN|I2C_ACKACT|I2C_CMD_STOP;
				
				_state = I2C_STOP; 
			}
			else if (_tm.Timeout(100))
			{
				i2c->CTRLB = I2C_SMEN|I2C_CMD_STOP;
				
				_state = I2C_RESET; 
			}
			else
			{
				DSCI2C &dsc = *_dsc;

				__disable_irq();

				bool c = _DMA.CheckComplete();
				
				__enable_irq();

				if (c)
				{
					_DMA.Disable();

					dsc.ack = (i2c->STATUS.RXNACK == 0);

					while (i2c->SYNCBUSY) __nop();

					i2c->CTRLB = I2C_SMEN|I2C_ACKACT|I2C_CMD_STOP;
						
					_state = I2C_STOP; 
				}
				else
				{
					u32 t = _DMA.GetBytesLeft();

					if (t != _prevCount) _prevCount = t, _tm.Reset();
				};
			};

			_dsc->readedLen = _DMA.GetBytesReady(); //_dsc->rlen - DmaWRB[I2C_DMACH].BTCNT;

			break;

		case I2C_STOP:

			if (i2c->STATUS.BUSSTATE == BUSSTATE_IDLE)
			{
				_dsc->ready = true;
				
				_dsc = 0;
				
				i2c->CTRLB = I2C_SMEN;

				_state = I2C_WAIT; 

				Usic_Unlock();
			}
			else if (i2c->SYNCBUSY == 0)
			{
				i2c->CTRLB = I2C_SMEN|I2C_ACKACT|I2C_CMD_STOP;
			};

			break;

		case I2C_RESET:

			if (i2c->STATUS.BUSSTATE == BUSSTATE_IDLE)
			{
				_DMA.Disable();

				_dsc->ready = false;

				_reqList.Add(_dsc);
					
				_dsc = 0;
					
				_state = I2C_WAIT;

				Usic_Unlock();
			}
			else if (i2c->SYNCBUSY == 0)
			{
				i2c->CTRLB = I2C_SMEN|I2C_ACKACT|I2C_CMD_STOP;
			};

			break;
	};

#elif defined(CPU_SAM4SA)

	T_HW::S_TWI* i2c = &_uhw->i2c;

	u32 i2cSR = i2c->SR;

	switch (_state)
	{
		case I2C_WAIT:

			if (CheckReset())
			{
				Usic_Update();
			}
			else
			{
				_dsc = _reqList.Get();

				if (_dsc != 0)
				{
					Usic_Lock();

					DSCI2C &dsc = *_dsc;

					dsc.ready = false;
					dsc.ack = false;
					dsc.readedLen = 0;

					i2c->CR = TWI_MSEN|TWI_SVDIS;					// 0x24;
					i2c->CWGR = _baud;
					i2c->IDR = ~0;

					HW::ReadMem32(&i2c->RHR);

					if (dsc.wlen == 0 || dsc.rlen != 0)
					{
						u16 len = dsc.wlen;
						if (len > 3) len = 3;

						u32 iadr = 0;
						byte *p = (byte*)dsc.wdata;

						for (u16 i = 0; i < len; i++) iadr = (iadr << 8)|*(p++);

						i2c->MMR = TWI_DADR(_dsc->adr)|TWI_IADRSZ(len)|TWI_MREAD;	
						i2c->IADR = iadr;

						_dma.ReadPeripheral(dsc.rdata, dsc.rlen, 0, 0);

						i2c->CR = TWI_START;

						_state = I2C_READ; 
					}
					else
					{
						i2c->MMR = TWI_DADR(_dsc->adr)|TWI_IADRSZ_NONE;	// dsc->MMR & ~0x1000;

						_dma.WritePeripheral(_dsc->wdata, _dsc->wlen, _dsc->wdata2, _dsc->wlen2);
	
						_state = I2C_WRITE; 
					};

					_prevCount = 0;
					_tm.Reset();
				};
			};

			break;

		case I2C_WRITE:

			if(i2cSR & TWI_NACK)
			{
				i2c->CR = TWI_STOP;
				
				_state = I2C_STOP; 
			}
			else if (_tm.Timeout(100))
			{
				i2c->CR = TWI_STOP;

				_state = I2C_RESET; 
			}
			else
			{
				DSCI2C &dsc = *_dsc;

				if (i2cSR & TWI_TXBUFE)
				{
					_dma.Disable();

					dsc.ack = true;

					i2c->CR = TWI_STOP;
						
					_state = I2C_STOP; 

					_prevCount = 0;
					_tm.Reset();
				}
				else
				{
					u32 t = _dma.GetWriteBytesLeft();

					if (t != _prevCount) _prevCount = t, _tm.Reset();
				};
			};

			break;

		case I2C_READ:

			if(i2cSR & TWI_NACK)
			{
				i2c->CR = TWI_STOP;

				_state = I2C_STOP; 
			}
			else if (_tm.Timeout(100))
			{
				i2c->CR = TWI_STOP;

				_state = I2C_RESET; 
			}
			else
			{
				if (i2cSR & TWI_RXBUFF)
				{
					_dma.Disable();

					_dsc->readedLen = _dsc->rlen - _dma.GetReadBytesLeft();

					HW::ReadMem32(&i2c->RHR);

					_dsc->ack = true;

					i2c->CR = TWI_STOP;

					_state = I2C_STOP; 
				}
				else
				{
					u32 t = _dma.GetReadBytesLeft();

					if (t != _prevCount) _prevCount = t, _tm.Reset();
				};
			};

			//_dsc->readedLen = _dsc->rlen - 1 - _dma.GetReadBytesLeft(); //_dsc->rlen - DmaWRB[I2C_DMACH].BTCNT;

			break;

		case I2C_STOP:

			if (i2cSR & TWI_TXCOMP)
			{
				HW::ReadMem32(&i2c->RHR);

				_dsc->ready = true;

				_dsc = 0;
				
				i2c->CR = TWI_MSDIS;

				_state = I2C_WAIT; 

				Usic_Unlock();
			};

			break;

		case I2C_RESET:

			if (i2cSR & TWI_TXCOMP)
			{
				_dma.Disable();

				i2c->CR = TWI_MSDIS;

				_dsc->ready = false;

				_reqList.Add(_dsc);
					
				_dsc = 0;
					
				_state = I2C_WAIT;

				Usic_Unlock();
			};

			break;
	};


#elif defined(CPU_XMC48)

	USICHWT	&I2C = _uhw;

	u32 psr = I2C->PSR_IICMode;

	switch (_state)
	{
		case I2C_WAIT:

			if (CheckReset())
			{
				Usic_Update();
			}
			else
			{
				_dsc = _reqList.Get();

				if (_dsc != 0)
				{
					Usic_Lock();

					DSCI2C &dsc = *_dsc;

					dsc.ready = false;
					dsc.ack = false;
					dsc.readedLen = 0;

					wrPtr	= (byte*)dsc.wdata;
					wrPtr2	= (byte*)dsc.wdata2;
					rdPtr	= (byte*)dsc.rdata;
					wlen	= dsc.wlen;
					wlen2	= dsc.wlen2;
					rlen	= dsc.rlen;

					I2C->PSCR = ~I2C_PCR;//RIF|AIF|TBIF|ACK|NACK|PCR;

					if (wlen == 0)
					{
						I2C->TBUF[0] = I2C_TDF_MASTER_START | (dsc.adr << 1) | 1;

						_state = I2C_READ;
					}
					else
					{
						I2C->TBUF[0] = I2C_TDF_MASTER_START | (dsc.adr << 1) | 0;

						_state = I2C_WRITE;
					};
				};
			};

			break;

		case I2C_WRITE:

			if (psr & (I2C_NACK | I2C_ERR | I2C_ARL | I2C_WTDF))
			{
				I2C->TBUF[0] = I2C_TDF_MASTER_STOP;

				_state = I2C_STOP;
			}
			else if (psr & I2C_ACK)
			{
				DSCI2C& dsc = *_dsc;

				dsc.ack = true;

				I2C->PSCR = I2C_ACK | I2C_PCR;

				if (wlen != 0)
				{
					I2C->TBUF[0] = I2C_TDF_MASTER_SEND | *wrPtr++; wlen--;

					if (wlen == 0 && wlen2 != 0)
					{
						wrPtr = wrPtr2;
						wlen = wlen2;
						wlen2 = 0;
					};
				}
				else if (rlen > 0)
				{
					I2C->TBUF[0] = I2C_TDF_MASTER_RESTART | (dsc.adr << 1) | 1;

					_state = I2C_READ;
				}
				else
				{
					I2C->TBUF[0] = I2C_TDF_MASTER_STOP;

					_state = I2C_STOP;
				};
			}
			else if (psr == 0)
			{
				_state = I2C_RESET;
			};

			break;

		case I2C_READ:

			if (psr & I2C_ACK)
			{
				if (psr & (I2C_SCR|I2C_RSCR))
				{
					I2C->TBUF[0] = I2C_TDF_MASTER_RECEIVE_ACK;

					I2C->PSCR = I2C_ACK | I2C_PCR;
				};
			}
			else if (psr & (I2C_RIF|I2C_AIF))
			{
//				twi_timestamp = GetMilliseconds();

				byte t = I2C->RBUF;

				if (rlen > 0)
				{
					*rdPtr++ = t; // receive data
					rlen--;
				};
					
				I2C->TBUF[0] = (rlen > 0) ? I2C_TDF_MASTER_RECEIVE_ACK : I2C_TDF_MASTER_RECEIVE_NACK; 

				I2C->PSCR = I2C_RIF | I2C_AIF | I2C_PCR;
			}
			else if(psr & (I2C_NACK|I2C_ERR|I2C_ARL|I2C_WTDF))
			{
				I2C->TBUF[0] = I2C_TDF_MASTER_STOP;
				
				_state = I2C_STOP;
			}
			else if (psr == 0)
			{
				_state = I2C_RESET;
			};

			break;

		case I2C_STOP:

			I2C->PSCR = ~I2C_PCR;

			_dsc->readedLen = _dsc->rlen - rlen;
			_dsc->ready = true;
				
			_dsc = 0;
				
			_state = I2C_WAIT;

			Usic_Unlock();

			break;

		case I2C_RESET:

			RequestReset();

			_dsc->ready = false;

			_reqList.Add(_dsc);
				
			_dsc = 0;
				
			_state = I2C_WAIT;

			Usic_Unlock();

			break;
	};

#elif defined(CPU_BF607)

	u16 stat = _hw->ISTAT;
	u16 mstat = _hw->MSTRSTAT;

	switch (_state)
	{
		case I2C_WAIT:
		{
			_dsc = _reqList.Get();

			if (_dsc != 0)
			{
				DSCI2C &dsc = *_dsc;

				dsc.ready = false;
				dsc.ack = false;
				dsc.readedLen = 0;

				wrPtr	= (byte*)dsc.wdata;
				wrPtr2	= (byte*)dsc.wdata2;
				rdPtr	= (byte*)dsc.rdata;
				wlen	= dsc.wlen;
				wlen2	= dsc.wlen2;
				rlen	= dsc.rlen;

				_hw->MSTRCTL	= 0;
				_hw->MSTRSTAT	= ~0;
				_hw->FIFOCTL	= 0;//XMTINTLEN|RCVINTLEN;
				_hw->MSTRADDR	= dsc.adr;
				_hw->IMSK = 0; 

				if (wlen == 0)
				{
					_hw->MSTRCTL = TWI_MST_DCNT(~0)|TWI_MST_DIR|TWI_MST_FAST|TWI_MST_EN;

					_state = I2C_READ;
				}
				else
				{
					if (wlen > 1)
					{
						_hw->TXDATA16 = wrPtr[0]|(wrPtr[1]<<8); wrPtr += 2; wlen -= 2;
					}
					else
					{
						_hw->TXDATA8 = *wrPtr++; wlen--;
					};

					_hw->MSTRCTL = TWI_MST_DCNT(~0)|TWI_MST_FAST|TWI_MST_EN;

					_state = I2C_WRITE;
				};

				_tm.Reset();
			};

			break;
		};

		case I2C_WRITE:
					
			if (stat & TWI_MERR)
			{
				_hw->MSTRCTL |= TWI_MST_STOP;
				_hw->FIFOCTL  = TWI_TXFLUSH|TWI_RXFLUSH;

				_state = I2C_STOP;
			}
			else if (stat & TWI_TXSERV)
			{
				_dsc->ack = true;

				if (wlen != 0)
				{
					if (wlen > 1 && _hw->FIFOSTAT._TXSTAT == TWI_TXSTAT_EMPTY)
					{
						_hw->TXDATA16 = wrPtr[0]|(wrPtr[1]<<8); wrPtr += 2; wlen -= 2;
					}
					else
					{
						_hw->TXDATA8 = *wrPtr++; wlen--;
					};

					//_hw->ISTAT = TWI_TXSERV;

					if (wlen == 0 && wlen2 != 0)
					{
						wrPtr = wrPtr2;
						wlen = wlen2;
						wlen2 = 0;
					};
				};

				_tm.Reset();
			}
			else if (stat & (TWI_SCLI|TWI_SDAI))
			{
				_tm.Reset();
			}
			else if (_tm.Check(US2CTM(5)))
			{
				if (rlen > 0)
				{
					_hw->MSTRCTL = TWI_MST_DCNT(0)|TWI_MST_RSTART|TWI_MST_FAST|TWI_MST_DIR|TWI_MST_EN;

					_state = I2C_READ;
				}
				else
				{
					HW::TWI->MSTRCTL = TWI_MST_STOP|TWI_MST_DCNT(0)|TWI_MST_FAST|TWI_MST_EN;

					_state = I2C_STOP;
				};
			};

			break;

		case I2C_READ:

			if (stat & TWI_MERR)
			{
				_hw->MSTRCTL |= TWI_MST_STOP;
				_hw->FIFOCTL  = TWI_TXFLUSH|TWI_RXFLUSH;

				_state = I2C_STOP;
			}
			else if (stat & TWI_MCOMP)
			{
				_hw->MSTRCTL = TWI_MST_DCNT(~0)|TWI_MST_FAST|TWI_MST_DIR|TWI_MST_EN;
				//_hw->MSTRCTL &= ~TWI_MST_RSTART;
				//_hw->MSTRCTL |= TWI_MST_EN;
			}
			else if (stat & TWI_RXSERV)
			{
				if (rlen > 0)
				{
					_dsc->ack = true;

					if (rlen > 1 && _hw->FIFOSTAT._RXSTAT == TWI_RXSTAT_FULL)
					{
						u16 t = _hw->RXDATA16; *rdPtr++ = t; *rdPtr++ = t>>8; rlen -= 2;
					}
					else
					{
						*rdPtr++ = _hw->RXDATA8; rlen--;
					};

					//_hw->ISTAT = TWI_RXSERV;
				};

				if (rlen == 0)
				{
					_hw->MSTRCTL = TWI_MST_STOP|TWI_MST_DCNT(0)|TWI_MST_FAST|TWI_MST_DIR|TWI_MST_EN;;

					_state = I2C_STOP;
				};
			};				

			break;

		case I2C_STOP:

			if ((_hw->MSTRCTL & TWI_MST_EN) == 0)
			{
				_hw->FIFOCTL = TWI_TXFLUSH|TWI_RXFLUSH;
				_hw->MSTRCTL	= 0;
				_hw->MSTRSTAT	= ~0;

				_dsc->readedLen = _dsc->rlen - rlen;
				_dsc->ready = true;
				
				_dsc = 0;
				
				_state = I2C_WAIT;
			};

			break;

		case I2C_RESET:

			_state = I2C_WAIT;

			break;
	};

	_hw->ISTAT		= stat; // & ~(TWI_TXSERV|TWI_RXSERV);
	_hw->MSTRSTAT	= mstat;
	
#endif

	return result;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // #else // #ifdef ADSP_BLACKFIN //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // I2C_IMP_H__17_04_2025__16_10
