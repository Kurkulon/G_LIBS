#ifndef COMPORT_H__11_02_2024__23_08
#define COMPORT_H__11_02_2024__23_08

#pragma once

#include "types.h"
#include "core.h"
#include "time.h"
#include "usic.h"
#include "DMA\DMA.h"

//#ifdef CPU_XMC48
//	#include "COM_DEF.h"
//#endif

#define US2COM(v) US2CTM(v)
#define MS2COM(v) MS2CTM(v)

#ifdef __ADSPBF59x__
class ComPort
#else
class ComPort : public USIC
#endif
{
  public:

	enum CONNECT_TYPE { ASYNC = 0, SYNC_M, SYNC_S };

	struct ReadBuffer
	{
		bool	recieved;
		word	len;
		word	maxLen;
		void*	data;
	};

	struct WriteBuffer
	{
		bool	transmited;
		word	len;
		void*	data;
	};

	#ifdef CPU_XMC48

	struct LLI
	{
		volatile void*	SAR;
		volatile void*	DAR;
		LLI*	LLP;
		u32		CTLL;
		u32		CTLH;
		u32		DSTAT;
	};

	#endif

  protected:

#ifdef ADSP_BLACKFIN
	enum STATUS485 { WRITEING = 0, WAIT_READ, READ_END };
#else
	enum STATUS485 { WRITEING = 0, WAIT_READ = 1, READING = 2, READ_END = 3 };
#endif

	#ifdef ADSP_BLACKFIN

		dword			_ModeRegister;

		word			_prevDmaCounter;

#ifdef __ADSPBF59x__
		word			_BaudRateRegister;
		void		SetBoudRate(word presc);
#elif defined(__ADSPBF70x__)
		dword			_BaudRateRegister;
#endif

	//	dword			_maskRTS;

		//CTM32			_startTransmitTime;
		//CTM32			_startReceiveTime;
		//dword			_preReadTimeout;
		//dword			_postReadTimeout;


		//bool IsTransmited() {return *pUART0_LSR & TEMT; }
		//bool IsRecieved()	{  }

		#ifdef __ADSPBF70x__

			T_HW::S_PORT* const _PIO_RTS;
			const u32	_MASK_RTS;

			const byte	_PIN_RTS;

			DMA_CH		_DMATX;
			DMA_CH		_DMARX;

			bool IsTransmited() { bool c = _DMATX.CheckComplete(); return c && (_uhw->STAT & UART_TEMT); }
			bool IsRecieved()	{ u32 t = _uhw->STAT; bool c = _prevDmaCounter != _DMARX.GetBytesLeft() || (t & (UART_OE|UART_PE|UART_FE|UART_BI)); _uhw->STAT = t; _prevDmaCounter = _DMARX.GetBytesLeft(); return c; }
			u32	GetDmaCounter() { return _DMARX.GetBytesLeft(); }											 
			u16	GetRecievedLen() { return _pReadBuffer->maxLen - GetDmaCounter(); }							 

		#endif

	#elif defined(CPU_SAME53)

		T_HW::S_PORT* const _PIO_SCK;
		T_HW::S_PORT* const _PIO_TX;
		T_HW::S_PORT* const _PIO_RX;
		T_HW::S_PORT* const _PIO_RTS;

		const byte	_PIN_SCK;
		const byte	_PIN_TX;
		const byte	_PIN_RX;
		//const byte	_PIN_RTS;

		const u32	_MASK_RTS;

		const u32 	_PMUX_SCK;
		const u32 	_PMUX_TX;
		const u32 	_PMUX_RX;

		const u32 	_GEN_SRC;
		const u32 	_GEN_CLK;

		const u32	_TXPO;
		const u32	_RXPO;

		DMA_CH		_DMA;

		u32 _CTRLA;
		u32 _CTRLB;
		u32 _CTRLC;
		u16	_BaudRateRegister;

		u32 _status;

		bool IsTransmited() { return (_uhw.usart->INTFLAG & (USART_TXC|USART_DRE)) == (USART_TXC|USART_DRE); }
		bool IsRecieved()	{ _DMA.Update(); u32 s = _uhw.usart->INTFLAG & (USART_ERROR|USART_RXS); if (s) { _status |= s; _uhw.usart->INTFLAG = s; return true; } else return false; }
		u32	GetDmaCounter() { return _DMA.GetBytesLeft(); }
		u16	GetRecievedLen() { return _pReadBuffer->maxLen - GetDmaCounter(); }

	#elif defined(CPU_SAM4SA)

		T_HW::S_PORT* const _PIO_RTS;

		const u32	_MASK_RTS;

		DMA_CH 		_dma;

		CONNECT_TYPE _cnType;

		u32 _MR;
		u16	_BaudRateRegister;
		u16 _prevDmaCounter;

		u32 _status;

		bool IsTransmited()	{ return (_uhw->usart.CSR & (US_TXEMPTY|US_ENDTX)) == (US_TXEMPTY|US_ENDTX); }
		bool IsRecieved()	{ u16 t = _dma.GetReadBytesLeft(); if (_prevDmaCounter != t) { _prevDmaCounter = t; return true; } else return false; }
		//u32	GetDmaCounter() { return _DMA->GetBytesLeft(); }
		u16	GetRecievedLen() { return _pReadBuffer->maxLen - _dma.GetReadBytesLeft(); }

	#elif defined(CPU_XMC48)

		T_HW::S_PORT* const _PIO_SCK;
		T_HW::S_PORT* const _PIO_TX;
		T_HW::S_PORT* const _PIO_RX;
		T_HW::S_PORT* const _PIO_RTS;

		const byte	_PIN_SCK;
		const byte	_PIN_TX;
		const byte	_PIN_RX;
		const byte	_PIN_RTS;

		const byte	_MUX_SCK;
		const byte	_MUX_TX;

		const u32	_MASK_RTS;

		DMA_CH *	const _DMA;

		const byte	_DRL;

		const u32	_DX0CR;
		const u32	_DX1CR;

		//const u32 __SCTR;
		u32 __FDR;
		u32 __BRG;
		//const u32 __TCSR;
		u32 __CCR;
		u32 __PCR;


		bool IsTransmited() { return (_uhw->PSR_ASCMode & UART_BUSY) == 0 && _DMA->CheckComplete(); }
		bool IsRecieved() { u32 s = _uhw->PSR_ASCMode & (UART_BUSY|UART_RSIF); _uhw->PSCR = UART_RSIF; return s; }
//		u16	GetDmaCounter() { return BLOCK_TS(_chdma->CTLH); }
		u32	GetDmaCounter() { return _DMA->GetDstCounter(); }
//		u16	GetRecievedLen() { return _pReadBuffer->maxLen - _prevDmaCounter; }
		u16	GetRecievedLen() { return GetDmaCounter(); }

	#elif defined (CORETYPE_LPC8XX)

		const byte	_PIN_SCK;
		const byte	_PIN_TX;
		const byte	_PIN_RX;
		const byte	_PIN_RTS;

		const u32	_MASK_RTS;

		#ifdef CPU_LPC824

			DMA_CH		_DMARX;
			DMA_CH		_DMATX;

		#endif

		u32			_CFG;  
		u32			_BRG; 
		//u32			_OSR; 

		#ifdef CPU_LPC824

			bool IsTransmited() { return (_uhw.usart->STAT & 8) && _DMATX.CheckComplete(); }
			bool IsRecieved() {  u32 s = _uhw.usart->STAT & (1<<12); _uhw.usart->STAT = (1<<12); return s; }
			u16	GetRecievedLen() { return _pReadBuffer->maxLen - _DMARX.GetBytesLeft(); }

		#elif defined(CPU_LPC812)

			typedef __irq void (*T_IRQ_Handler)();

			static const byte _UART_IRQ[3];
			static const T_IRQ_Handler _IRQ_W_Handler[3];
			static const T_IRQ_Handler _IRQ_R_Handler[3];

			bool IsTransmited()		{ return (_uhw.usart->STAT & 8) && (wlen == 0); }
			bool IsRecieved()		{ u32 s = _uhw.usart->STAT & (1<<12); _uhw.usart->STAT = (1<<12); return s; }
			u16	GetRecievedLen()	{ return _pReadBuffer->maxLen - rlen; }

			static __irq void ReadHandler_0();
			static __irq void ReadHandler_1();
			static __irq void ReadHandler_2();

			static __irq void WriteHandler_0();
			static __irq void WriteHandler_1();
			static __irq void WriteHandler_2();

			static ComPort *_objCom0;
			static ComPort *_objCom1;
			static ComPort *_objCom2;

			void IRQ_WriteHandler();
			void IRQ_ReadHandler();

			byte	*wdata;
			byte	*rdata;
			u32		wlen;
			u32		rlen;

		#endif

	#elif defined(WIN32)

		bool IsTransmited() { return true; }
		bool IsRecieved() { return true; }
		u32	GetDmaCounter() { return 0; }
		u16	GetRecievedLen() { return 0; }

	#endif

	bool			_connected;
	byte			_status485;

	//u32				_prevDmaCounter;

	ReadBuffer		*_pReadBuffer;
	WriteBuffer		*_pWriteBuffer;




	//dword			_ModeRegister;
	//dword			_pinRTS;
	//dword			_startTransmitTime;

//	dword			_startReceiveTime;
//	dword			_preReadTimeout;
	dword			_postReadTimeout;
	dword			_readTimeout;
	dword			_writeTimeout;

	void 		EnableTransmit(void* src, word count);
	void 		DisableTransmit();
	void 		EnableReceive(void* dst, word count);
	void 		DisableReceive();

	//static		bool _InitCom(byte i, ComPort* cp);
	//static		bool _DeinitCom(byte i, ComPort* cp);

	//static ComPort *_objCom1;
	//static ComPort *_objCom2;
	//static ComPort *_objCom3;

	word 		BoudToPresc(dword speed);

	CTM32			_rtm;

#if defined(CPU_SAME53) || defined(CPU_SAM4SA) || defined(CPU_XMC48) || defined(__ADSPBF70x__)

	void		Set_RTS() { if (_PIO_RTS != 0) _PIO_RTS->SET(_MASK_RTS); }
	void		Clr_RTS() { if (_PIO_RTS != 0) _PIO_RTS->CLR(_MASK_RTS); }

#elif defined(CORETYPE_LPC8XX)

	void		Set_RTS() { HW::GPIO->SET(_MASK_RTS); }
	void		Clr_RTS() { HW::GPIO->CLR(_MASK_RTS); }

#endif

  public:
	  
#ifdef __ADSPBF59x__

	ComPort() : _connected(false), _status485(READ_END) {}

#elif defined(__ADSPBF70x__)

	  ComPort(byte num, T_HW::S_PORT *prts, byte pinrts) : USIC(num), _PIO_RTS(prts), _PIN_RTS(pinrts), _MASK_RTS(1UL<<pinrts), _DMATX(UART0_TX_DMA+num*2), _DMARX(UART0_RX_DMA+num*2), _connected(false), _status485(READ_END) {}

#elif defined(CPU_SAME53)

	ComPort(byte num, T_HW::S_PORT* psck, T_HW::S_PORT* ptx, T_HW::S_PORT* prx, T_HW::S_PORT* prts, byte pinsck, byte pintx, byte pinrx, byte pinrts, u32 muxsck, u32 muxtx, u32 muxrx, u32 txpo, u32 rxpo, u32 gen_src, u32 gen_clk, byte dmach)
		: USIC(num), _PIO_SCK(psck), _PIO_TX(ptx), _PIO_RX(prx), _PIO_RTS(prts), _PIN_SCK(pinsck), _PIN_TX(pintx), _PIN_RX(pinrx), _MASK_RTS(1UL<<pinrts),
		_PMUX_SCK(muxsck), _PMUX_TX(muxtx), _PMUX_RX(muxrx), _GEN_SRC(gen_src), _GEN_CLK(gen_clk), _TXPO(txpo), _RXPO(rxpo), _DMA(dmach), _status485(READ_END) {}

#elif defined(CPU_SAM4SA)

	  ComPort(byte num, T_HW::S_PORT* prts, byte pinrts) : USIC(num), _PIO_RTS(prts), _MASK_RTS(1UL<<pinrts), _status485(READ_END), _dma(&(_usic_hw[num]->uart.PDC)) {}

#elif defined(CPU_XMC48)

	ComPort(byte num, T_HW::S_PORT* psck, T_HW::S_PORT* ptx, T_HW::S_PORT* prx, T_HW::S_PORT* prts, byte pinsck, byte pintx, byte pinrx, byte pinrts, byte muxsck, byte muxtx, u32 dx0cr, u32 dx1cr, DMA_CH *dma, byte drl)
		: USIC(num), _PIO_SCK(psck), _PIO_TX(ptx), _PIO_RX(prx), _PIO_RTS(prts), _PIN_SCK(pinsck), _PIN_TX(pintx), _PIN_RX(pinrx), _PIN_RTS(pinrts), _MUX_SCK(muxsck), _MUX_TX(muxtx), _MASK_RTS(1UL<<pinrts),
		_DMA(dma), _DRL(drl), _DX0CR(dx0cr), _DX1CR(dx1cr), _connected(false), _status485(READ_END) {}

#elif defined (CPU_LPC824)

	ComPort(byte num, byte pinsck, byte pintx, byte pinrx, byte pinrts)
		: USIC(num), _PIN_SCK(pinsck), _PIN_TX(pintx), _PIN_RX(pinrx), _PIN_RTS(pinrts), _MASK_RTS(1UL<<pinrts), _DMARX(num*2), _DMATX(num*2+1), _status485(READ_END) {}

#elif defined (CPU_LPC812)

	ComPort(byte num, byte pinsck, byte pintx, byte pinrx, byte pinrts)
		: USIC(num), _PIN_SCK(pinsck), _PIN_TX(pintx), _PIN_RX(pinrx), _PIN_RTS(pinrts), _MASK_RTS(1UL<<pinrts), _status485(READ_END) {}

#else

	ComPort() : USIC(0), _connected(false), _status485(READ_END) {}

#endif

	bool	Connect(CONNECT_TYPE ct, dword speed, byte parity, byte stopBits = 1);
	bool	Connect(dword speed, byte parity) { return Connect(ASYNC, speed, parity, 1); }

#ifdef __ADSPBF59x__

	//bool		Connect(CONNECT_TYPE ct, dword speed, byte parity, byte stopBits);

#else
	virtual		void	InitHW();

#endif
				//bool	ConnectAsyn(byte port, dword speed, byte parity, byte stopBits);
				//bool	ConnectSync(byte port, dword speed, byte parity, byte stopBits);
				bool	Disconnect();
				bool	Update();

				bool	Read(ComPort::ReadBuffer *readBuffer, dword preTimeout, dword postTimeout);
				bool	Write(ComPort::WriteBuffer *writeBuffer, dword timeout = 0);

				void	TransmitByte(byte v);

	//static __irq void _IntHandlerCom1();
	//static __irq void _IntHandlerCom2();
	//static __irq void _IntHandlerCom3();

};

#endif // COMPORT_H__11_02_2024__23_08
