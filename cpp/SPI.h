#ifndef SPI_H__21_04_2022__11_18
#define SPI_H__21_04_2022__11_18

#if !defined(CPU_LPC812)

#include "types.h"
#include "usic.h"
#include "list.h"
#include "DMA\DMA.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef DEF_SPIM_IMP_V2
#define S_SPIM	S_SPIM2
#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef __ADSPBF59x__

#define SPI_NUM 2

typedef T_HW::S_SPI *SPIHWT;

#elif defined(__ADSPBF70x__)

#define SPI_NUM 3
#define SPIMODE_MASK (SPI_FMODE|SPI_MIO_DUAL|SPI_MIO_QUAD|SPI_CPOL|SPI_CPHA|SPI_LSBF)

typedef T_HW::S_SPI *SPIHWT;

#endif 

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct DSCSPI
{
	DSCSPI*			next;

	void*			wdata;	
	void*			rdata;
	u32				adr;
	u16				alen;
	u16				wlen;
	u16				rlen;
#ifdef ADSP_BLACKFIN
	u16				mode;
	u16				baud;
#endif
	volatile bool	ready;
	byte			csnum;
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef ADSP_BLACKFIN
class S_SPIM
#else
class S_SPIM : public USIC
#endif
{
protected:

	//const I2C_CONF _conf;

#ifdef CPU_SAME53

	T_HW::S_PORT* const _PIO_SPCK;
	T_HW::S_PORT* const _PIO_MOSI;
	T_HW::S_PORT* const _PIO_MISO;
	T_HW::S_PORT* const _PIO_CS;

	const u32 			_MASK_SPCK;
	const u32 			_MASK_MOSI;
	const u32 			_MASK_MISO;

	const u32 			_PMUX_SPCK;
	const u32 			_PMUX_MOSI;
	const u32 			_PMUX_MISO;

	const u32 			_GEN_SRC;
	const u32 			_GEN_CLK;

	const u32 * const	_MASK_CS;
	const u32			_MASK_CS_LEN;

	const u32 			_DIPO;
	const u32 			_DOPO;

	DMA_CH *	const 	_DMATX;
	DMA_CH *	const 	_DMARX;

#elif defined(CPU_SAM4SA)

	T_HW::S_PORT* const _PIO_CS;

	const u32 * const	_MASK_CS;
	const u32			_MASK_CS_LEN;
	const u32 			_GEN_CLK;

	DMA_CH				_dma;

	u32					_MR;
	u32					_CSR;

#elif defined(CPU_XMC48)

	T_HW::S_PORT* const _PIO_SPCK;
	T_HW::S_PORT* const _PIO_MOSI;
	T_HW::S_PORT* const _PIO_MISO;
	T_HW::S_PORT* const _PIO_CS;

	const byte 			_PIN_SPCK;
	const byte 			_PIN_MOSI;
	const byte 			_PIN_MISO;

	const byte 			_MUX_SPCK;
	const byte 			_MUX_MOSI;

	const byte * const	_PIN_CS;
	const u32			_PIN_CS_LEN;

	DMA_CH *	const 	_DMA;

	const byte	_DRL;

	const u32	_DX0CR;
	const u32	_DX1CR;
	const u32	_GEN_CLK;

	//u32 __SCTR;
	u32 __FDR;
	//u32 __BRG;
	//u32 __TCSR;
	//u32 __CCR;
	//u32 __PCR;

#elif defined(CPU_LPC824)

	DMA_CH				_DMARX;
	DMA_CH				_DMATX;

#endif

#ifdef ADSP_BLACKFIN

	static 	u32 			_busy_mask;
	static 	u32 			_alloc_mask;

	#ifdef __ADSPBF59x__
		static  const byte		_spi_pid[SPI_NUM];
	#endif

	static	SPIHWT	const	_spi_hw[SPI_NUM];

	//T_HW::S_PORT * const	_PORT_CS;
	T_HW::S_PIO * const		_PIO_CS;
	const u16 * const		_MASK_CS;
	const u32 				_GEN_CLK;

	#ifdef __ADSPBF59x__
		const u16				_MASK_SCK_MOSI_MISO;
	#elif defined __ADSPBF70x__
		const u16				_MASK_SCK_MOSI_MISO_D2_D3;
	#endif

	const byte				_num;
	#ifdef __ADSPBF59x__
	const byte				_pid;
	#endif
	
	SPIHWT					_hw;

	const byte				_MASK_CS_LEN;

	#ifdef __ADSPBF59x__
		DMA_CH					_DMA;
	#elif defined __ADSPBF70x__
		DMA_CH					_DMATX;
		DMA_CH					_DMARX;
	#endif

	List<DSCSPI>			_reqList;
	DSCSPI*					_dsc;

	enum STATE { ST_WAIT = 0, ST_WRITE, ST_READ, ST_STOP };

	STATE _state;

	u16		_baud;
	u16		_ctl;
	u16		_MASK_CS_ALL;

	#ifdef __ADSPBF59x__
		u16		_spimode;
	#elif defined __ADSPBF70x__
		u32		_spimode;
	#endif

#else

	List<DSCSPI>	_reqList;
	DSCSPI*			_dsc;

	enum STATE { WAIT = 0, WRITE_ADR, WRITE, STOP };

	STATE _state;

	u32		_baud;
	u32		_MASK_CS_ALL;

#endif

public:

#ifdef __ADSPBF59x__

	S_SPIM(byte num, T_HW::S_PIO* piocs, u16* mcs, byte mcslen, u32 gen_clk, u16 mask_sck_mosi_miso = ~0)
		: _num(num), _pid(_spi_pid[num]), _PIO_CS(piocs), _MASK_CS(mcs), _MASK_CS_LEN(mcslen), _GEN_CLK(gen_clk),
		_MASK_SCK_MOSI_MISO(mask_sck_mosi_miso), _DMA(5+num), _dsc(0), _state(ST_WAIT), _spimode(0) {}

			bool CheckWriteComplete()	{ return _DMA.CheckComplete() && (_hw->Stat & (SPIF|TXS)) == SPIF; }
			bool CheckReadComplete()	{ if (_DMA.CheckComplete()) { _hw->Ctl = 0; _DMA.Disable(); return true;} else return false; }

			void ChipSelect(byte num, u16 spimode, u16 baud)	{ _hw->Baud = baud; _spimode = spimode & (CPOL|CPHA|LSBF); _PIO_CS->CLR(_MASK_CS[num]); }
			void ChipDisable()									{ _PIO_CS->SET(_MASK_CS_ALL); }

			void SetMode(u16 mode) { _spimode = mode & (CPOL|CPHA|LSBF); }

			inline void Disable()	{ _hw->Ctl = 0; _DMA.Disable(); }
			inline void DisableTX() { Disable(); }
			inline void DisableRX() { Disable(); }

			void WriteByteSync(byte v)		{ WriteReadByte(v); }
			void WriteByteAsync(byte v);
			void WaitWriteByte()			{ while((_hw->Stat & (SPIF|TXS)) != SPIF); }

			void ReadByteStart(u16 count) {}
			byte ReadByteAsync() { return WriteReadByte(0); }
			byte ReadByteSync(u16 count) { return WriteReadByte(0); }

#elif defined(__ADSPBF70x__)

	S_SPIM(byte num, T_HW::S_PIO* piocs, u16* mcs, byte mcslen, u32 gen_clk, u16 mask_sck_mosi_miso_d2_d3 = ~0)
		: _num(num), _PIO_CS(piocs), _MASK_CS(mcs), _MASK_CS_LEN(mcslen), _GEN_CLK(gen_clk),
		_MASK_SCK_MOSI_MISO_D2_D3(mask_sck_mosi_miso_d2_d3), _DMATX(4+num*2), _DMARX(5+num*2), _dsc(0), _state(ST_WAIT), _spimode(0) {}

			bool CheckWriteComplete()	{ return /*_DMATX.CheckComplete() &&*/ (_hw->STAT & (SPI_TF|SPI_SPIF)) == (SPI_TF|SPI_SPIF); }
			bool CheckReadComplete()	{ if (_hw->STAT & SPI_RF) { _hw->CTL = 0; _DMARX.Disable(); return true;} else return false; }

			void ChipSelect(byte num, u32 spimode, u16 baud)	{ _hw->CLK = baud; _hw->CTL = _spimode = SPI_EN|SPI_MSTR|(spimode & SPIMODE_MASK); _PIO_CS->CLR(_MASK_CS[num]); }
			void ChipDisable()									{ _PIO_CS->SET(_MASK_CS_ALL); _hw->TXCTL = 0; _hw->RXCTL = 0; _hw->CTL = 0; }

			void SetMode(u16 mode) { _spimode = SPI_EN|SPI_MSTR|(mode & SPIMODE_MASK); }

			void Disable()	 { _hw->CTL = 0; _hw->TXCTL = 0; _hw->RXCTL = 0; _DMATX.Disable(); _DMARX.Disable();}
			void DisableTX() { /*_hw->CTL = 0;*/ _hw->TXCTL = 0; _DMATX.Disable(); }
			void DisableRX() { /*_hw->CTL = 0;*/ _hw->RXCTL = 0; _DMARX.Disable(); }

			void WriteByteSync(byte v)		{ WriteReadByte(v); }
			void WriteByteAsync(byte v);
			void WaitWriteByte()			{ while((_hw->STAT & (SPI_SPIF|SPI_TFS_MASK)) != (SPI_SPIF|SPI_TFS_EMPTY)); }

			void ReadByteStart(u16 count);
			byte ReadByteAsync() { while(_hw->STAT & SPI_RFE); return _hw->RFIFO.W; }
			byte ReadByteSync(u16 count) { ReadByteStart(count); return ReadByteAsync(); }

#elif defined(CPU_SAME53)

	S_SPIM(byte num, T_HW::S_PORT* pspck, T_HW::S_PORT* pmosi, T_HW::S_PORT* pmiso, T_HW::S_PORT* pcs, 
		u32 mspck, u32 mmosi, u32 mmiso, 
		u32 muxspck, u32 muxmosi, u32 muxmiso,
		u32* mcs, u32 mcslen, u32 dipo, u32 dopo,
		u32 gen_src, u32 gen_clk, DMA_CH *dmatx, DMA_CH *dmarx)
		: USIC(num), _PIO_SPCK(pspck), _PIO_MOSI(pmosi), _PIO_MISO(pmiso), _PIO_CS(pcs), 
		_MASK_SPCK(mspck), _MASK_MOSI(mmosi), _MASK_MISO(mmiso),
		_PMUX_SPCK(muxspck), _PMUX_MOSI(muxmosi), _PMUX_MISO(muxmiso), _GEN_SRC(gen_src), _GEN_CLK(gen_clk), _MASK_CS(mcs), _MASK_CS_LEN(mcslen), _DIPO(dipo), _DOPO(dopo), 
		 _DMATX(dmatx), _DMARX(dmarx), _dsc(0), _state(WAIT) {}

			bool CheckWriteComplete() { return _DMATX->CheckComplete() && (_uhw.spi->INTFLAG & SPI_TXC); }
			bool CheckReadComplete() { return _DMATX->CheckComplete() && _DMARX->CheckComplete(); }
			void ChipSelect(byte num)	{ _PIO_CS->CLR(_MASK_CS[num]); }
			void ChipDisable()			{ _PIO_CS->SET(_MASK_CS_ALL); }
			void DisableTX() { _DMATX->Disable(); }
			void DisableRX() { _uhw.spi->CTRLB &= ~SPI_RXEN; _DMATX->Disable(); _DMARX->Disable(); }

			void WriteByteSync(byte v)		{ WriteReadByte(v); }
			void WriteByteAsync(byte v);
			void WaitWriteByte()			{ while((_uhw.spi->INTFLAG & (SPI_TXC|SPI_DRE)) != (SPI_TXC|SPI_DRE)); }

			void ReadByteStart(u16 count) {}
			byte ReadByteAsync() { return WriteReadByte(0); }
			byte ReadByteSync(u16 count) { return WriteReadByte(0); }

#elif defined(CPU_SAM4SA)

	S_SPIM(byte num, T_HW::S_PORT* pcs, u32* mcs, u32 mcslen, u32 genclk) : USIC(num), _PIO_CS(pcs), _MASK_CS(mcs), _MASK_CS_LEN(mcslen), _GEN_CLK(genclk), _dma(&(_usic_hw[num]->spi.PDC)), _dsc(0), _state(WAIT) {}

	bool CheckWriteComplete()	{ return _dma.CheckWriteComplete() && ((_uhw->spi.SR & (SPI_TDRE|SPI_TXEMPTY|SPI_TXBUFE|SPI_ENDTX)) == (SPI_TDRE|SPI_TXEMPTY|SPI_TXBUFE|SPI_ENDTX)); }
	bool CheckReadComplete()	{ return _dma.CheckReadComplete(); }
	void ChipSelect(byte num)	{ _PIO_CS->CLR(_MASK_CS[num]); }
	void ChipDisable()			{ _PIO_CS->SET(_MASK_CS_ALL); }
	void DisableTX()			{ _dma.Disable(); _uhw->spi.CR = SPI_SPIDIS; }
	void DisableRX()			{ _dma.Disable(); _uhw->spi.CR = SPI_SPIDIS; }

	//void WriteByteSync(byte v)		{ WriteReadByte(v); }
	//void WriteByteAsync(byte v);
	//void WaitWriteByte()			{ while((_uhw.spi->INTFLAG & (SPI_TXC|SPI_DRE)) != (SPI_TXC|SPI_DRE)); }

	//void ReadByteStart(u16 count) {}
	//byte ReadByteAsync() { return WriteReadByte(0); }
	//byte ReadByteSync(u16 count) { return WriteReadByte(0); }

#elif defined(CPU_XMC48)

	S_SPIM(byte num, T_HW::S_PORT* pspck, T_HW::S_PORT* pmosi, T_HW::S_PORT* pmiso, T_HW::S_PORT* pcs, 
		byte pinspck, byte pinmosi, byte pinmiso, byte muxspck, byte muxmosi, byte* pincs, u32 pincslen, DMA_CH *dma, byte drl, u32 dx0cr, u32 dx1cr, u32 genclk)
		: USIC(num), _PIO_SPCK(pspck), _PIO_MOSI(pmosi), _PIO_MISO(pmiso), _PIO_CS(pcs), 
		_PIN_SPCK(pinspck), _PIN_MOSI(pinmosi), _PIN_MISO(pinmiso), _MUX_SPCK(muxspck), _MUX_MOSI(muxmosi), _PIN_CS(pincs), _PIN_CS_LEN(pincslen),
		_DMA(dma), _DRL(drl), _DX0CR(dx0cr), _DX1CR(dx1cr), _GEN_CLK(genclk), _dsc(0), _state(WAIT) {}

			void ChipSelect(byte num)	{ _PIO_CS->BCLR(_PIN_CS[num]); }
			bool CheckWriteComplete() { return _DMA->CheckComplete() /*&& (_uhw.spi->INTFLAG & SPI_TXC)*/; }
			bool CheckReadComplete() { return _DMA->CheckComplete(); }
			void ChipDisable()			{ _PIO_CS->SET(_MASK_CS_ALL); }
			void DisableTX() { _DMA->Disable();; }
			void DisableRX() { _DMA->Disable();; }

#elif defined(CPU_LPC824)

	S_SPIM(byte num) : USIC(num), _DMARX(num*2), _DMATX(num*2+1) {}

			void ChipSelect(byte num)	{  }
			bool CheckWriteComplete()	{ return _DMATX.CheckComplete() /*&& (_uhw.spi->INTFLAG & SPI_TXC)*/; }
			bool CheckReadComplete()	{ return _DMARX.CheckComplete(); }
			void ChipDisable()			{  }
			void DisableTX() {  }
			void DisableRX() {  }

#elif defined(WIN32)

	S_SPIM() : USIC(0) {}

	bool CheckWriteComplete() { return true; }
	void DisableTX() {  }
	void DisableRX() {  }

#endif

			bool Connect(u32 baudrate);
			void Disconnect();
			bool AddRequest(DSCSPI *d);
			bool Update();
			
			byte WriteReadByte(byte v);

			void WritePIO(const void *data, u16 count);
			void WriteAsyncDMA(const void *data, u16 count);
			void WriteSyncDMA(const void *data, u16 count);

			void ReadPIO(void *data, u16 count);
			void ReadAsyncDMA(void *data, u16 count);
			void ReadSyncDMA(void *data, u16 count);

			void WriteAsyncDMA(const void *data1, u16 count1, const void *data2, u16 count2);
			void WriteSyncDMA(const void *data1, u16 count1, const void *data2, u16 count2);

	virtual	void InitHW();
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

extern void SPI_Init();
extern bool SPI_AddRequest(DSCSPI *d);
extern bool SPI_Update();
//extern i32	Get_FRAM_SPI_SessionsAdr();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // #if !defined(CPU_LPC812)

#endif // SPI_H__21_04_2022__11_18
