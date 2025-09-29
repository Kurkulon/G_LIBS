#ifndef COMPORT_IMP_H__11_02_2024__23_09
#define COMPORT_IMP_H__11_02_2024__23_09
 
#pragma once

//#pragma O3
//#pragma Otime

//#include <stdio.h>
//#include <conio.h>

#include "ComPort.h"
#include "time.h"

#ifdef _DEBUG_
//	static const bool _debug = true;
#else
//	static const bool _debug = false;
#endif

extern dword millisecondsCount;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef CPU_SAME53	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#elif defined(CPU_XMC48)	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	#define __SCTR (USIC_PDL(1) | USIC_TRM(1) | USIC_FLE(7) | USIC_WLE(7))

	//#define __CCR (USIC_MODE(2))

	#define __BRG_ASYN	(USIC_DCTQ(15))
	#define __BRG_SYNC	(USIC_DCTQ(0)|USIC_CLKSEL(3))

	//#define __DX0CR (DSEL(0) | INSW(0) | DFEN(0) | DSEN(0) | DPOL(0) | SFSEL(0) | CM(0) | DXS(0))
	//#define __DX1CR (DSEL(0) | INSW(0) | DFEN(0) | DSEN(0) | DPOL(0) | SFSEL(0) | CM(0) | DXS(0))
	//#define __DX2CR (DSEL(0) | INSW(0) | DFEN(0) | DSEN(0) | DPOL(0) | SFSEL(0) | CM(0) | DXS(0))
	//#define __DX3CR (DSEL(0) | INSW(0) | DFEN(0) | DSEN(0) | DPOL(0) | SFSEL(0) | CM(0) | DXS(0))

	#define __PCR_ASYN	(UART_SMD(1) | UART_SP(9) | UART_RSTEN(1) | UART_TSTEN(1))
	#define __PCR_SYNC	(UART_SMD(0) | UART_SP(0) | UART_RSTEN(1) | UART_TSTEN(1))

	//#define __FDR (USIC_STEP(0x3FF) | USIC_DM(1))

	#define __TCSR (USIC_TDEN(1)|USIC_TDSSM(1))

	//ComPort::ComBase	ComPort::_bases[3] = { 
	////		used	dsel	USIC_CH			port_RTS	pin_RTS		USIC PID	INPR_SRx	GPDMA		dmaCh	DLR
	//	{	false, 	1, 		HW::USIC0_CH0,	HW::P1, 	1<<0,		PID_USIC0,  0,			HW::DMA0, 	0, 		0 	}, 
	//	{	false, 	2, 		HW::USIC0_CH1,	HW::P1, 	1<<13,		PID_USIC0, 	1,			HW::DMA0, 	1, 		2 	},
	//	{	false, 	4, 		HW::USIC2_CH1,	HW::P1, 	1<<8,		PID_USIC2,	1,			HW::DMA1, 	0, 		9 	}
	//};

	static u32 parityMask[3] = { USIC_PM(0), USIC_PM(3), USIC_PM(2) };

	//#define READ_PIN_SET()	HW::P1->BSET(0)
	//#define READ_PIN_CLR()	HW::P1->BCLR(0)

#elif defined(CPU_LPC812)

	ComPort *ComPort::_objCom0 = 0;
	ComPort *ComPort::_objCom1 = 0;
	ComPort *ComPort::_objCom2 = 0;

	const byte ComPort::_UART_IRQ[3] = { UART0_IRQ, UART1_IRQ, UART2_IRQ };
	const ComPort::T_IRQ_Handler ComPort::_IRQ_W_Handler[3] = { ComPort::WriteHandler_0,	ComPort::WriteHandler_1,	ComPort::WriteHandler_2	};
	const ComPort::T_IRQ_Handler ComPort::_IRQ_R_Handler[3] = { ComPort::ReadHandler_0,		ComPort::ReadHandler_1,		ComPort::ReadHandler_2	};

#endif	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef __ADSPBF59x__ //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if defined(PIO_RTS) && defined(MASK_RTS)
inline void	Set_RTS() { PIO_RTS->SET(MASK_RTS); }
inline void	Clr_RTS() { PIO_RTS->CLR(MASK_RTS); }
#else
inline void	Set_RTS() { }
inline void	Clr_RTS() { }
#endif
inline void Usic_Lock() {}
inline void Usic_Unlock() {}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool ComPort::Connect(CONNECT_TYPE ct, dword speed, byte parity, byte stopBits)
{
	if (_connected)
	{
		return false;
	};

	_BaudRateRegister = BoudToPresc(speed);

	_ModeRegister = WLS(8);

	switch (parity)
	{
		case 0:		// нет четности
			_ModeRegister |= 0;
			break;

		case 1:
			_ModeRegister |= PEN;
			break;

		case 2:
			_ModeRegister |= PEN|EPS;
			break;
	};

	*pUART0_GCTL = UCEN;
	*pUART0_LCR = _ModeRegister;
	SetBoudRate(_BaudRateRegister);

	HW::PIOF->ClrMUX(PF11|PF12);		// *pPORTF_MUX	&= ~(PF11|PF12);	
	HW::PIOF->SetFER(PF11|PF12);		// *pPORTF_FER |= PF11|PF12;	

	#if defined(PIO_RTS) && defined(MASK_RTS)

		PIO_RTS->ClrFER(MASK_RTS);			// PIO_RTS_FER &= ~MASK_RTS;
		PIO_RTS->DirSet(MASK_RTS);			//PIO_RTS_DIR |= MASK_RTS;
		PIO_RTS->CLR(MASK_RTS);				//PIO_RTS_CLR  = MASK_RTS;

	#endif

	_status485 = READ_END;

	return _connected = true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool ComPort::Disconnect()
{
	if (!_connected) return false;

	DisableReceive();
	DisableTransmit();

	*pUART0_GCTL = 0;

	_status485 = READ_END;

	_connected = false;

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void ComPort::SetBoudRate(word presc)
{
	union { word w; byte b[2]; };

	w = presc;

	*pUART0_LCR |= DLAB;
	*pUART0_DLL = b[0];
	*pUART0_DLH = b[1];
	*pUART0_LCR &= ~DLAB;
}

#else	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void ComPort::InitHW()
{
	#ifdef CPU_SAME53	

		HW::GCLK->PCHCTRL[_ugclk] = GCLK_GEN(_GEN_SRC)|GCLK_CHEN;	// 25 MHz

		HW::MCLK->ClockEnable(_upid);
		
		T_HW::S_USART* uhw = _uhw.usart;
		
		if (_PIO_SCK != 0) _PIO_SCK->SetWRCONFIG(1UL<<_PIN_SCK, _PMUX_SCK|PORT_WRPINCFG|PORT_PMUXEN|PORT_WRPMUX|PORT_PULLEN);
		if (_PIO_TX != 0) _PIO_TX->SetWRCONFIG(1UL<<_PIN_TX, _PMUX_TX|PORT_WRPINCFG|PORT_PMUXEN|PORT_WRPMUX|PORT_PULLEN);
		if (_PIO_RX != 0) _PIO_RX->SetWRCONFIG(1UL<<_PIN_RX, _PMUX_RX|PORT_WRPINCFG|PORT_PMUXEN|PORT_WRPMUX|PORT_PULLEN);

		if (_PIO_RTS != 0)
		{
			_PIO_RTS->SetWRCONFIG(_MASK_RTS, PORT_WRPINCFG|PORT_WRPMUX);
			_PIO_RTS->DIRSET = _MASK_RTS; 
			_PIO_RTS->SET(_MASK_RTS); 
		};

		uhw->CTRLA = USART_SWRST;

		while(uhw->SYNCBUSY);

		uhw->CTRLA = _CTRLA;
		uhw->CTRLB = _CTRLB;
		uhw->CTRLC = _CTRLC;
		uhw->BAUD = _BaudRateRegister;

		uhw->CTRLA |= USART_ENABLE;

		while(uhw->SYNCBUSY);

	#elif defined(CPU_SAM4SA)

		HW::PMC->ClockEnable(_upid);

		if(_usic_num == 3) // USART0
		{
			HW::PIOA->PDR		=	PA5|PA6;
			HW::PIOA->ABCDSR1	&=	~(PA5|PA6);
			HW::PIOA->ABCDSR2	&=	~(PA5|PA6);

			if (_cnType != ASYNC)
			{
				HW::PIOA->PDR		=	PA2;
				HW::PIOA->ABCDSR1	|=	PA2;
				HW::PIOA->ABCDSR2	&=	~PA2;
			};
		}
		else if(_usic_num == 4)  // USART1
		{
			HW::PIOA->PDR		=	PA21|PA22;
			HW::PIOA->ABCDSR1	&=	~(PA21|PA22);
			HW::PIOA->ABCDSR2	&=	~(PA21|PA22);
		
			if (_cnType != ASYNC)
			{
				HW::PIOA->PDR		=	PA23;
				HW::PIOA->ABCDSR1	&=	~PA23;
				HW::PIOA->ABCDSR2	&=	~PA23;
			};
		}
		else if(_usic_num == 5)  // UART0
		{
			HW::PIOA->PDR		=	PA9|PA10;
			HW::PIOA->ABCDSR1	&=	~(PA9|PA10);
			HW::PIOA->ABCDSR2	&=	~(PA9|PA10);
		}
		else if(_usic_num == 6)  // UART1
		{
			HW::PIOB->PDR		=	PB2|PB3;
			HW::PIOB->ABCDSR1	&=	~(PB2|PB3);
			HW::PIOB->ABCDSR2	&=	~(PB2|PB3);
		};

		T_HW::S_UART &uart = _uhw->uart;

		uart.CR = 3;
		uart.MR = _MR;
		uart.BRGR = _BaudRateRegister;

	#elif defined(CPU_XMC48)

		HW::Peripheral_Enable(_upid);

		if (_PIO_SCK != 0) _PIO_SCK->ModePin(_PIN_SCK, _MUX_SCK);
		_PIO_TX->ModePin(_PIN_TX, _MUX_TX);
		_PIO_RX->ModePin(_PIN_RX, I2DPU);
		_PIO_RTS->ModePin(_PIN_RTS, G_PP);

		_uhw->KSCFG = USIC_MODEN|USIC_BPMODEN|USIC_BPNOM|USIC_NOMCFG(0);

		__dsb(15);

		_uhw->BRG = __BRG;		
		_uhw->PCR_ASCMode = __PCR;

		_uhw->FDR			= __FDR; //_BaudRateRegister;
		_uhw->SCTR			= __SCTR;
		_uhw->DX0CR			= _DX0CR; // DSEL(_cb->dsel);//__DX0CR;
		_uhw->DX1CR			= USIC_DPOL(1);
		_uhw->TCSR			= __TCSR;
		_uhw->PSCR			= ~0;
		_uhw->CCR			= __CCR; //_ModeRegister;

		_DMA->SetDlrLineNum(_DRL);

	#elif defined(CORETYPE_LPC8XX)

		HW::SYSCON->SYSAHBCLKCTRL |= 1UL<<_upid;

		if(_usic_num == 0)
		{
			HW::SWM->U0_SCLK	= _PIN_SCK;
			HW::SWM->U0_RXD		= _PIN_RX;
			HW::SWM->U0_TXD		= _PIN_TX;

			#ifdef CPU_LPC812
				_objCom0 = this;
			#endif
		}
		else if(_usic_num == 1)
		{
			HW::SWM->U1_SCLK	= _PIN_SCK;
			HW::SWM->U1_RXD		= _PIN_RX;
			HW::SWM->U1_TXD		= _PIN_TX;

			#ifdef CPU_LPC812
				_objCom1 = this;
			#endif
		}
		else if(_usic_num == 2)
		{
			HW::SWM->U2_SCLK	= _PIN_SCK;
			HW::SWM->U2_RXD		= _PIN_RX;
			HW::SWM->U2_TXD		= _PIN_TX;

			#ifdef CPU_LPC812
				_objCom2 = this;
			#endif
		};

		#ifdef CPU_LPC812
			CM0::NVIC->SET_ER(UART0_IRQ+_usic_num);
		#endif

		HW::GPIO->DirSet(_MASK_RTS);

		T_HW::S_USART* &uhw = _uhw.usart;

		uhw->CFG = _CFG;
		uhw->BRG = _BRG;

	#elif defined(__ADSPBF70x__)

		_uhw->CLK = _BaudRateRegister;
		_uhw->CTL = _ModeRegister;

		if (_usic_num == 0)
		{
			HW::PIOB->SetFER(PB8|PB9);
		}
		else if (_usic_num == 1)
		{
			HW::PIOC->SetFER(PC0|PC1);
		};

		if (_PIO_RTS != 0)
		{
			_PIO_RTS->DirSet(_MASK_RTS); 
			_PIO_RTS->ClrFER(_MASK_RTS); 
			_PIO_RTS->CLR(_MASK_RTS); 
		};

	#endif

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool ComPort::Connect(CONNECT_TYPE ct, dword speed, byte parity, byte stopBits)
{
#ifndef WIN32

	if(!Usic_Connect())
	{
		return false;
	};

	#ifdef CPU_SAME53	

		switch (ct)
		{
			case ASYNC:

				_CTRLA = USART_MODE_INT_CLK|_TXPO|_RXPO|USART_DORD|USART_SAMPR_8x_ARITH;
				_BaudRateRegister = BoudToPresc(speed);

				break;

			case SYNC_M:

				_CTRLA = USART_MODE_INT_CLK|_TXPO|_RXPO|USART_CMODE|USART_DORD;
				_BaudRateRegister = (_GEN_CLK+speed) / (2*speed) - 1;

				break;

			case SYNC_S:

				_CTRLA = USART_MODE_EXT_CLK|_TXPO|_RXPO|USART_CMODE|USART_DORD;
				_BaudRateRegister = ~0;

				break;

		};

		_CTRLB = ((stopBits == 2) ? USART_SBMODE : 0);
		_CTRLC = 0;

		switch (parity)
		{
			case 0:		// нет четности

				break;

			case 1:
				_CTRLA |= USART_FORM_USART_PARITY;
				_CTRLB |= USART_PMODE;
				break;

			case 2:
				_CTRLA |= USART_FORM_USART_PARITY;
				_CTRLB &= ~USART_PMODE;
				break;
		};

		_CTRLB |= USART_SFDE;

	#elif defined(CPU_SAM4SA)

		_cnType = (_usic_num == 3 || _usic_num == 4) ? ct : ASYNC;

		switch (ct)
		{
		case ASYNC:

			_MR = US_CHRL_8_BIT|US_USCLKS_MCK;
			_BaudRateRegister = BoudToPresc(speed);

			break;

		case SYNC_M:

			_MR = US_SYNC|US_CHRL_8_BIT|US_USCLKS_MCK|US_CLKO;
			_BaudRateRegister = (MCK+speed/2) / speed;

			break;

		case SYNC_S:

			_MR = US_SYNC|US_CHRL_8_BIT|US_USCLKS_SCK;
			_BaudRateRegister = ~0;

			break;
		};

		switch (parity)
		{
			case 0:	_MR |= US_PAR_NO;	break;	// нет четности
			case 1:	_MR |= US_PAR_ODD;	break;
			case 2:	_MR |= US_PAR_EVEN;	break;
		};

	#elif defined(CPU_XMC48)

		//_dma = cb.dma;
		//_dmaChMask = 1<<cb.dmaCh;
		//_chdma = &(_dma->CH[cb.dmaCh]);
		//_dlr = cb.dlr;
		//_inpr_sr = cb.inpr_sr;

		__CCR = USIC_MODE_UART | ((parity < 3) ? parityMask[parity] : parityMask[0]);

		switch (ct)
		{
			case ASYNC:

				__BRG	= __BRG_ASYN;
				__PCR	= __PCR_ASYN | ((stopBits == 2) ? UART_STPB(1) : 0);;
				__FDR	= BoudToPresc(speed) | USIC_DM(1);

				break;

			case SYNC_M:

				__BRG	= __BRG_SYNC;
				__PCR	= __PCR_SYNC | ((stopBits == 2) ? UART_STPB(1) : 0);
				__FDR	= BoudToPresc(speed) | USIC_DM(1);

				break;

			case SYNC_S:

				__BRG	= __BRG_SYNC;
				__PCR	= __PCR_SYNC | ((stopBits == 2) ? UART_STPB(1) : 0);
				__FDR	= 0;

				break;

		};

	#elif defined(CORETYPE_LPC8XX)

		switch (ct)
		{
			case ASYNC:

				_CFG = (1UL<<2);
				_BRG = BoudToPresc(speed);

				break;

			case SYNC_M:

				_CFG = (1UL<<2)|(1UL<<11)|(1UL<<14);
				_BRG = (MCK+speed/2) / speed - 1;

				break;

			case SYNC_S:

				_CFG = (1UL<<2)|(1UL<<11);
				_BRG = ~0;

				break;

		};

		if (stopBits == 2) { _CFG |= 1UL<<6; };

		switch (parity)
		{
			case 0:		// нет четности
				//_CFG |= 0<<4;
				break;

			case 1:
				_CFG |= 3<<4;
				break;

			case 2:
				_CFG |= 2<<4;
				break;
		};

	#elif defined(__ADSPBF70x__)

		BoudToPresc(speed);

		_ModeRegister = UART_MOD_UART|UART_8BIT|UART_RFIT|UART_EN;

		if (stopBits == 2) { _ModeRegister |= UART_STB; };

		switch (parity)
		{
			case 1:

				_ModeRegister |= UART_PEN;
				break;

			case 2:

				_ModeRegister |= UART_PEN|UART_EPS;
				break;
		};

	#endif

	u32 t = 24000000/speed;

	if (t < 20) t = 20;

	_writeTimeout = US2COM(t);

	InitHW();

	_status485 = READ_END;

	return _connected = true;

#else

	return true;

#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool ComPort::Disconnect()
{
#ifndef WIN32

	if (!_connected) return false;

	DisableReceive();
	DisableTransmit();

	_status485 = READ_END;

	_connected = false;

	Usic_Disconnect();

#endif

	return true;
}

#endif // #else // #ifdef ADSP_BLACKFIN //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

word ComPort::BoudToPresc(dword speed)
{
	if (speed == 0) return 0;

	#ifdef __ADSPBF59x__

		return (word)((SCLK + (speed<<3)) / (speed << 4));

	#elif defined(CPU_SAME53)	

		return 65536ULL*(_GEN_CLK - 8*speed)/_GEN_CLK;

	#elif defined(CPU_SAM4SA)	

		return (MCK/16 + speed/2) / speed;

	#elif defined(CPU_XMC48)

		word presc = ((SYSCLK + speed/2) / speed + 8) / 16;

		if (presc > 1024) presc = 1024;

		return 1024 - presc;

	#elif defined(CORETYPE_LPC8XX)

		return (MCK/16+speed/2) / speed - 1;

	#elif defined(WIN32)
		
		return 0;

	#elif defined(__ADSPBF70x__)

		speed = (SCLK0 + (speed>>1)) / speed;

		if (speed == 0) speed = 1;

		if (speed > 0xFFFF) 
		{
			speed = (speed+8)>>4;

			if (speed > 0xFFFF) speed = 0xFFFF;
		}
		else
		{
			speed |= UART_EDBO;
		};

		return _BaudRateRegister = speed;

	#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void ComPort::TransmitByte(byte v)
{
#ifndef WIN32

	Set_RTS();

	Usic_Lock();

	#ifdef CPU_SAME53	

		_uhw.usart->CTRLB = _CTRLB|USART_TXEN;

		while ((_uhw.usart->INTFLAG & USART_DRE) == 0);

		_uhw.usart->DATA = v;

		while ((_uhw.usart->INTFLAG & USART_TXC) == 0);

		_uhw.usart->CTRLB = _CTRLB;	// Disable transmit and receive

	#elif defined(CPU_SAM4SA)

		T_HW::S_UART &uart = _uhw->uart;

		uart.CR = US_TXDIS|US_RXDIS;// 0xA0;	// Disable transmit and receive

		uart.CR = US_TXEN;	// 0x40;

		uart.THR = v;

		while ((uart.SR & US_TXEMPTY) == 0);

		uart.CR = US_TXDIS|US_RXDIS;// 0xA0;	// Disable transmit and receive

	#elif defined(CPU_XMC48)

		_uhw->CCR = __CCR|USIC_TBIEN;
		_uhw->INPR = USIC_TSINP(Get_INPR_SR());

		_uhw->TBUF[0] = v;

		while ((_uhw->PSR_ASCMode & UART_TSIF) == 0);

		_uhw->CCR = __CCR;	// Disable transmit and receive

	#endif

	Usic_Unlock();

	Clr_RTS();

#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void ComPort::EnableTransmit(void* src, word count)
{
#ifndef WIN32

	if (count == 0) return;

	Set_RTS();

	Usic_Lock();

	#ifdef CPU_SAME53	

		_status = 0;

		if ((_uhw.usart->CTRLA & USART_ENABLE) == 0)
		{
			_uhw.usart->CTRLA = _CTRLA;
			_uhw.usart->CTRLB = _CTRLB;
			_uhw.usart->CTRLC = _CTRLC;
			_uhw.usart->BAUD = _BaudRateRegister;

			_uhw.usart->CTRLA |= USART_ENABLE;

			while(_uhw.usart->SYNCBUSY);
		};

		_uhw.usart->CTRLB = _CTRLB|USART_TXEN;
		_uhw.usart->INTFLAG = ~0;

		_DMA.WritePeripheral(src, &_uhw.usart->DATA, count, DMCH_TRIGACT_BURST|(((DMCH_TRIGSRC_SERCOM0_TX>>8)+_usic_num*2)<<8), DMDSC_BEATSIZE_BYTE); 

		//while (_uhw.usart->SYNCBUSY & USART_CTRLB);

	#elif defined(CPU_SAM4SA)

		T_HW::S_UART &uart = _uhw->uart;
		
		uart.CR = US_TXDIS|US_RXDIS;// 0xA0;	// Disable transmit and receive

		_dma.WritePeripheral(src, count, 0, 0);

		uart.CR = US_TXEN;

	#elif defined(CPU_XMC48)

		_uhw->TBCTR = 0;
		_uhw->TRBSCR = ~0;//TRBSCR_FLUSHTB;

		_DMA->SetDlrLineNum(_DRL);
		//_DMA->Enable_DLR();

		__disable_irq();

		_uhw->PSCR = ~0;
		_uhw->CCR = __CCR|USIC_TBIEN;
		_uhw->TBCTR = TBCTR_SIZE8|TBCTR_LIMIT(3)|TBCTR_STBTEN|TBCTR_STBTM|TBCTR_STBIEN|TBCTR_STBINP(Get_INPR_SR());
		_uhw->INPR = USIC_TBINP(Get_INPR_SR());

		_DMA->WritePeripheralByte(src, &_uhw->IN[0], count);

		if ((_uhw->PSR_ASCMode & UART_TBIF) == 0 && (_uhw->TRBSR & TRBSR_STBI) == 0)
		{
			_uhw->FMR = USIC_CH_FMR_SIO0_Msk << Get_INPR_SR();
		};

		__enable_irq();

	#elif defined(CPU_LPC824)

		_uhw.usart->CFG &= ~1;	// Disable transmit and receive

		_DMATX.WritePeripheralByte(src, &_uhw.usart->TXDATA, count);

		//HW::DMA->ENABLESET0 = _txDmaMask;	

		//HW::DMA->CH[_txDmaCh].CFG = PERIPHREQEN | TRIGBURST_SNGL | CHPRIORITY(3);	

		//DmaTable[_txDmaCh].SEA = (byte*)src + count - 1;	
		//DmaTable[_txDmaCh].DEA = &_usart->TXDATA;			
		//DmaTable[_txDmaCh].NEXT = 0;

		//HW::DMA->CH[_txDmaCh].XFERCFG = CFGVALID | SETINTA | SWTRIG | WIDTH_8 | SRCINC_1 | DSTINC_0 | XFERCOUNT(count);

		//HW::DMA->SETVALID0 = _txDmaMask;

		_uhw.usart->CFG |= 1;

	#elif defined(CPU_LPC812)

		_uhw.usart->CFG &= ~1;	// Disable transmit and receive

		wdata = (byte*)src;
		wlen = count;

		VectorTableExt[_UART_IRQ[_usic_num]] = _IRQ_W_Handler[_usic_num];

		_uhw.usart->INTENSET = 4;

		_uhw.usart->CFG |= 1;

	#elif defined(__ADSPBF59x__)

		*pDMA8_CONFIG = 0;	// Disable transmit and receive
		*pUART0_IER = 0;

		*pDMA8_START_ADDR = src;
		*pDMA8_X_COUNT = count;
		*pDMA8_X_MODIFY = 1;
		*pDMA8_CONFIG = FLOW_STOP|WDSIZE_8|SYNC|DMAEN;

		//_startTransmitTime.Reset();

		*pUART0_IER = ETBEI;

	#elif defined(__ADSPBF70x__)

		_uhw->CTL = _ModeRegister;

		_DMATX.Write8(src, count);

		_uhw->IMSK_SET = UART_ETBEI;

	#endif

	_status485 = WRITEING;

	_rtm.Reset();

#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void ComPort::DisableTransmit()
{
#ifndef WIN32

	Clr_RTS();

	#ifdef CPU_SAME53	

		//while (_uhw.usart->SYNCBUSY & USART_CTRLB);

		if (_status & USART_ERROR)
		{
			_uhw.usart->CTRLA = USART_SWRST;
		}
		else
		{
			_uhw.usart->CTRLB = _CTRLB;	// Disable transmit and receive
		};

		_DMA.Disable(); 

	#elif defined(CPU_SAM4SA)

		_uhw->uart.CR = US_TXDIS;

		_dma.Disable();

	#elif defined(CPU_XMC48)

		_uhw->CCR = __CCR;
		_uhw->TBCTR = 0;
		_uhw->RBCTR = 0;
		_uhw->TRBSCR = ~0;
		_DMA->Disable(); 

	#elif defined(CPU_LPC824)

		_uhw.usart->CFG &= ~1;	// Disable transmit and receive
		_uhw.usart->INTENCLR = 4;

		_DMATX.Disable();

	#elif defined(CPU_LPC812)

		_uhw.usart->CFG &= ~1;	// Disable transmit and receive
		_uhw.usart->INTENCLR = ~0;

	#elif defined(__ADSPBF59x__)

		*pDMA8_CONFIG = 0;	// Disable transmit and receive
		*pUART0_IER = 0;

	#elif defined(__ADSPBF70x__)

		_DMATX.Disable();

		_uhw->IMSK = 0;
		_uhw->STAT = ~0;
		//_uhw->CTL = _ModeRegister;

	#endif

	Usic_Unlock();

#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void ComPort::EnableReceive(void* dst, word count)
{
#ifndef WIN32

	Clr_RTS();

	Usic_Lock();

	#ifdef CPU_SAME53	

		_status = 0;

		if ((_uhw.usart->CTRLA & USART_ENABLE) == 0)
		{
			_uhw.usart->CTRLA = _CTRLA;
			_uhw.usart->CTRLB = _CTRLB;
			_uhw.usart->CTRLC = _CTRLC;
			_uhw.usart->BAUD = _BaudRateRegister;

			_uhw.usart->CTRLA |= USART_ENABLE;

			while(_uhw.usart->SYNCBUSY);
		};

		_DMA.ReadPeripheral(&_uhw.usart->DATA, dst, count, DMCH_TRIGACT_BURST|(((DMCH_TRIGSRC_SERCOM0_RX>>8)+_usic_num*2)<<8), DMDSC_BEATSIZE_BYTE);

		//while (_uhw.usart->SYNCBUSY & USART_CTRLB);

		_uhw.usart->CTRLB = _CTRLB|USART_RXEN;
		_uhw.usart->INTFLAG = ~0;

	#elif defined(CPU_SAM4SA)

		T_HW::S_USART &usart = _uhw->usart;

		usart.CR = US_RSTSTA|US_TXDIS|US_RXDIS;// 0x1A0;	// Disable transmit and receive, reset status

		_dma.ReadPeripheral(dst, count, 0, 0);
		
		_prevDmaCounter = _dma.GetReadBytesLeft();
		
		usart.CR = US_RSTSTA|US_RXEN;

	#elif defined(CPU_XMC48)

		volatile u32 t;

		_DMA->SetDlrLineNum(_DRL);

		_uhw->RBCTR = 0;
		_uhw->TRBSCR = ~0;

		__disable_irq();

		while(_uhw->RBUFSR & (RBUFSR_RDV0|RBUFSR_RDV1))
		{
			t = _uhw->RBUF;
		};

		_uhw->PSCR = ~0;

		_uhw->RBCTR = RBCTR_SIZE8|RBCTR_LIMIT(0)|RBCTR_LOF|RBCTR_SRBIEN|RBCTR_SRBINP(Get_INPR_SR());
		_uhw->CCR = __CCR;//|USIC_RIEN;//|USIC_AIEN;
		_uhw->INPR = ~0;//USIC_RINP(Get_INPR_SR())|USIC_AINP(Get_INPR_SR());

		_DMA->ReadPeripheralByte(&_uhw->OUTR, dst, count);

		__enable_irq();

	#elif defined(CPU_LPC824)

		_uhw.usart->CFG &= ~1;	// Disable transmit and receive

		_DMARX.ReadPeripheralByte(&_uhw.usart->RXDATA, dst, count);

		_uhw.usart->CFG |= 1;

	#elif defined(CPU_LPC812)

		_uhw.usart->CFG &= ~1;	// Disable transmit and receive

		rdata = (byte*)dst;
		rlen = count;

		VectorTableExt[_UART_IRQ[_usic_num]] = _IRQ_R_Handler[_usic_num];

		_uhw.usart->INTENSET = 1;
		_uhw.usart->CFG |= 1;

	#elif defined(__ADSPBF59x__)

		*pDMA7_CONFIG = 0;	// Disable transmit and receive
		*pUART0_IER = 0;

		*pDMA7_START_ADDR = dst;
		*pDMA7_CURR_X_COUNT = *pDMA7_X_COUNT = _prevDmaCounter = count;
		*pDMA7_X_MODIFY = 1;
		*pDMA7_CONFIG = WNR|FLOW_STOP|WDSIZE_8|SYNC|DMAEN;

		//_startReceiveTime.Reset();

		count = *pUART0_RBR;
		count = *pUART0_LSR;
		*pUART0_IER = ERBFI;

	#elif defined(__ADSPBF70x__)

		_uhw->CTL = _ModeRegister;

		_DMARX.Read8(dst, count);

		_prevDmaCounter = _DMARX.GetBytesLeft();

		_uhw->IMSK_SET = UART_ERBFI;

	#endif

#endif

	_rtm.Reset();

	_status485 = WAIT_READ;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void ComPort::DisableReceive()
{
#ifndef WIN32

	Clr_RTS();

	#ifdef CPU_SAME53	

		//while (_uhw.usart->SYNCBUSY & USART_CTRLB);

		if (_status & USART_ERROR)
		{
			_uhw.usart->CTRLA = USART_SWRST;
		}
		else
		{
			_uhw.usart->CTRLB = _CTRLB;
		};

		_DMA.Disable();

	#elif defined(CPU_SAM4SA)

		_uhw->uart.CR = US_RXDIS;

		_dma.Disable();

	#elif defined(CPU_XMC48)

		_uhw->CCR = __CCR;
		_uhw->TBCTR = 0;
		_uhw->RBCTR = 0;
		_uhw->TRBSCR = ~0;
		_DMA->Disable(); 

	#elif defined(CPU_LPC824)

		_uhw.usart->CFG &= ~1;	// Disable transmit and receive
		_uhw.usart->INTENCLR = 1;

		_DMARX.Disable();

	#elif defined(CPU_LPC812)

		_uhw.usart->CFG &= ~1;	// Disable transmit and receive
		_uhw.usart->INTENCLR = ~0;

	#elif defined(__ADSPBF59x__)

		*pDMA7_CONFIG = 0;	// Disable transmit and receive
		*pUART0_IER = 0;

	#elif defined(__ADSPBF70x__)

		_DMARX.Disable();

		_uhw->IMSK = 0;
		_uhw->STAT = ~0;

		//_uhw->CTL = _ModeRegister;

	#endif

	Usic_Unlock();

#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef CPU_LPC812

__irq void ComPort::ReadHandler_0()
{
	_objCom0->IRQ_ReadHandler();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

__irq void ComPort::WriteHandler_0()
{
	_objCom0->IRQ_WriteHandler();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

__irq void ComPort::ReadHandler_1()
{
	_objCom1->IRQ_ReadHandler();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

__irq void ComPort::ReadHandler_2()
{
	_objCom2->IRQ_ReadHandler();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

__irq void ComPort::WriteHandler_1()
{
	_objCom1->IRQ_WriteHandler();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

__irq void ComPort::WriteHandler_2()
{
	_objCom2->IRQ_WriteHandler();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//#pragma push
//#pragma O3
//#pragma Otime

void ComPort::IRQ_ReadHandler()
{
	*(rdata++) = _uhw.usart->RXDATA;
	rlen--;
	
	if (rlen == 0) _uhw.usart->INTENCLR = 1;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void ComPort::IRQ_WriteHandler()
{
	register T_HW::S_USART *usart = _uhw.usart;

	usart->TXDATA = *(wdata++);

	wlen--;
	
	if (wlen == 0) usart->INTENCLR = 4;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//#pragma pop

#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef __ADSPBF59x__

bool ComPort::Update()
{
	static u32 stamp = 0;

	bool r = true;

	if (!_connected) 
	{
		_status485 = READ_END;
	};

	//stamp = GetCycles32();

	switch (_status485)
	{
		case WRITEING:

			if (*pUART0_LSR & TEMT)
			{
				_pWriteBuffer->transmited = true;
				_status485 = READ_END;

				DisableTransmit();
				DisableReceive();

				r = false;
			};

			break;

		case WAIT_READ:

			if ((_prevDmaCounter-*pDMA7_CURR_X_COUNT) == 0)
			{
				if (_rtm.Timeout(_readTimeout))
				{
					DisableReceive();
					_pReadBuffer->len = _pReadBuffer->maxLen - _prevDmaCounter;
					_pReadBuffer->recieved = _pReadBuffer->len > 0;
					_status485 = READ_END;
					r = false;
				};
			}
			else
			{
				_prevDmaCounter = *pDMA7_CURR_X_COUNT;
				_rtm.Reset();
				_readTimeout = _postReadTimeout;
			};

			break;

		case READ_END:

			r = false;

			break;
	};

	return r;
}

#else //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool ComPort::Update()
{
//	static u32 stamp = 0;

	bool r = true;

	if (!_connected) 
	{
		_status485 = READ_END;
	};

//	stamp = GetRTT();

	switch (_status485)
	{
		case WRITEING: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			
			if (IsTransmited())
			{
				if (_rtm.Timeout(_writeTimeout))
				{
					_pWriteBuffer->transmited = true;
					_status485 = READ_END;

					DisableTransmit();

					_status485 = READ_END;

					r = false;
				};
			}
			else
			{
				_rtm.Reset();
			};

			break;

		case WAIT_READ: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		{
			if (IsRecieved())
			{
				_rtm.Reset();
				_readTimeout = _postReadTimeout;
			}
			else
			{
				if (_rtm.Timeout(_readTimeout))
				{
					DisableReceive();
					_pReadBuffer->len = GetRecievedLen();
					_pReadBuffer->recieved = _pReadBuffer->len > 0;
					_status485 = READ_END;
					r = false;
				};
			};

		};

		break;

		case READ_END: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			r = false;

			break;
	};

	return r;
}

#endif 

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool ComPort::Read(ComPort::ReadBuffer *readBuffer, dword preTimeout, dword postTimeout)
{
	if (_status485 != READ_END || readBuffer == 0)
	{
		return false;
	};

	_readTimeout = preTimeout;
	_postReadTimeout = postTimeout;

	_pReadBuffer = readBuffer;
	_pReadBuffer->recieved = false;
	_pReadBuffer->len = 0;

	//_prevDmaCounter = 0;//_pReadBuffer->maxLen;

	EnableReceive(_pReadBuffer->data, _pReadBuffer->maxLen);

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool ComPort::Write(ComPort::WriteBuffer *writeBuffer, dword timeout)
{
	if (_status485 != READ_END || writeBuffer == 0)
	{
		return false;
	};

	_pWriteBuffer = writeBuffer;
	_pWriteBuffer->transmited = false;

	//if (timeout >= US2COM(10)) _writeTimeout = timeout;

	EnableTransmit(_pWriteBuffer->data, _pWriteBuffer->len);

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // COMPORT_IMP_H__11_02_2024__23_09
