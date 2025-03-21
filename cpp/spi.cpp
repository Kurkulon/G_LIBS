#ifndef CPU_LPC812

#include "spi.h"
#include "core.h"
#include "SEGGER_RTT\SEGGER_RTT.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef ADSP_BLACKFIN

u32 			S_SPIM::_busy_mask = 0;
u32 			S_SPIM::_alloc_mask = 0;

#endif

#ifdef __ADSPBF59x__

const byte		S_SPIM::_spi_pid[SPI_NUM]	= {	PID_DMA5_SPI0_RX_TX,	PID_DMA6_SPI1_RX_TX };
SPIHWT const	S_SPIM::_spi_hw[SPI_NUM]	= { HW::SPI0,				HW::SPI1			};

#elif defined(__ADSPBF70x__)

//const byte		S_SPIM::_spi_pid[SPI_NUM]	= {	PID_DMA5_SPI0_RX_TX,	PID_DMA6_SPI1_RX_TX };
SPIHWT const	S_SPIM::_spi_hw[SPI_NUM]	= { HW::SPI0, HW::SPI1, HW::SPI2			};

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef CPU_XMC48

#define SPI__PCR	(SPI_MSLSEN | SPI_SELINV | SPI_TIWEN | SPI_MCLK | SPI_CTQSEL1(0) | SPI_PCTQ1(0) | SPI_DCTQ1(0))
#define SPI__TCSR	(USIC_TDEN(1) | USIC_HPCMD(0))
#define SPI__SCTR	(USIC_SDIR(1) | USIC_TRM(1) | USIC_FLE(0x3F) | USIC_WLE(7))
#define SPI__BRG	(USIC_SCLKCFG(2U) | USIC_CTQSEL(0) | USIC_DCTQ(1) | USIC_PCTQ(3) | USIC_CLKSEL(0))
#define SPI__CCR	(USIC_MODE_SPI)

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void S_SPIM::InitHW()
{
#ifdef __ADSPBF59x__

	_PIO_CS->DirSet(_MASK_CS_ALL); 
	_PIO_CS->ClrFER(_MASK_CS_ALL); 

	if (_num == 0)
	{
		HW::PORTF->FER |= (PF13|PF14|PF15) & _MASK_SCK_MOSI_MISO;
		HW::PORTF->MUX &= ~((PF13|PF14|PF15) &_MASK_SCK_MOSI_MISO);
	}
	else
	{
		HW::PORTG->FER |= (PG8|PG9|PG10) & _MASK_SCK_MOSI_MISO;
		HW::PORTG->MUX &= ~((PG8|PG9|PG10) & _MASK_SCK_MOSI_MISO);
	};

	_PIO_CS->SET(_MASK_CS_ALL); 

	_hw->Baud = _baud;
	_hw->Ctl = _ctl;
	_hw->Stat = ~0;

#elif defined(__ADSPBF70x__)

	_PIO_CS->DirSet(_MASK_CS_ALL); 
	_PIO_CS->ClrFER(_MASK_CS_ALL); 

	if (_num == 0)
	{
		HW::PIOB->SetFER((PB0|PB1|PB2|PB3|PB7) & _MASK_SCK_MOSI_MISO_D2_D3);

		if (_MASK_SCK_MOSI_MISO_D2_D3 & PB0) HW::PIOB->SetMUX(0, 2);
		if (_MASK_SCK_MOSI_MISO_D2_D3 & PB1) HW::PIOB->SetMUX(1, 2);
		if (_MASK_SCK_MOSI_MISO_D2_D3 & PB2) HW::PIOB->SetMUX(2, 2);
		if (_MASK_SCK_MOSI_MISO_D2_D3 & PB3) HW::PIOB->SetMUX(3, 2);
		if (_MASK_SCK_MOSI_MISO_D2_D3 & PB7) HW::PIOB->SetMUX(7, 2);

	}
	else if (_num == 1)
	{
		HW::PIOA->SetFER((PA0|PA1|PA2) & _MASK_SCK_MOSI_MISO_D2_D3);

		if (_MASK_SCK_MOSI_MISO_D2_D3 & PA0) HW::PIOA->SetMUX(0, 0);
		if (_MASK_SCK_MOSI_MISO_D2_D3 & PA1) HW::PIOA->SetMUX(1, 0);
		if (_MASK_SCK_MOSI_MISO_D2_D3 & PA2) HW::PIOA->SetMUX(2, 0);
	}
	else
	{
		HW::PIOB->SetFER((PB10|PB11|PB12|PB13|PB14) & _MASK_SCK_MOSI_MISO_D2_D3);

		if (_MASK_SCK_MOSI_MISO_D2_D3 & PB10) HW::PIOB->SetMUX(10, 0);
		if (_MASK_SCK_MOSI_MISO_D2_D3 & PB11) HW::PIOB->SetMUX(11, 0);
		if (_MASK_SCK_MOSI_MISO_D2_D3 & PB12) HW::PIOB->SetMUX(12, 0);
		if (_MASK_SCK_MOSI_MISO_D2_D3 & PB13) HW::PIOB->SetMUX(13, 0);
		if (_MASK_SCK_MOSI_MISO_D2_D3 & PB14) HW::PIOB->SetMUX(14, 0);
	};

	_PIO_CS->SET(_MASK_CS_ALL); 

	_hw->CLK = _baud;
	_hw->CTL = _ctl;
	_hw->STAT = ~0;


#elif defined(CPU_SAME53)

	HW::GCLK->PCHCTRL[_ugclk] = GCLK_GEN(_GEN_SRC)|GCLK_CHEN;	// 25 MHz

	HW::MCLK->ClockEnable(_upid);
	
	T_HW::S_SPI* spi = _uhw.spi;
	
	_PIO_SPCK->SetWRCONFIG(_MASK_SPCK, _PMUX_SPCK|PORT_WRPINCFG|PORT_PMUXEN|PORT_WRPMUX);
	_PIO_MOSI->SetWRCONFIG(_MASK_MOSI, _PMUX_MOSI|PORT_WRPINCFG|PORT_PMUXEN|PORT_WRPMUX);
	_PIO_MISO->SetWRCONFIG(_MASK_MISO, _PMUX_MISO|PORT_WRPINCFG|PORT_PMUXEN|PORT_WRPMUX);
	_PIO_CS->DIRSET = _MASK_CS_ALL; 
	_PIO_CS->SetWRCONFIG(_MASK_CS_ALL, PORT_WRPINCFG|PORT_WRPMUX);
	_PIO_CS->SET(_MASK_CS_ALL); 

	spi->CTRLA = SPI_SWRST;

	while(spi->SYNCBUSY);

	spi->CTRLA = SERCOM_MODE_SPI_MASTER;

	spi->CTRLA = SERCOM_MODE_SPI_MASTER|_DIPO|_DOPO;
	spi->CTRLB = 0;
	spi->CTRLC = 1;
	spi->BAUD = _baud;

	spi->DBGCTRL = 1;

	spi->CTRLA |= SPI_ENABLE;

	while(spi->SYNCBUSY);

	spi->STATUS = ~0;

#elif defined(CPU_XMC48)

	HW::Peripheral_Enable(_upid);

	_uhw->KSCFG = USIC_MODEN|USIC_BPMODEN|USIC_BPNOM|USIC_NOMCFG(0);

	__dsb(15);

	_uhw->CCR = 0;

	_uhw->FDR = __FDR;
	_uhw->BRG = SPI__BRG;
    
	_uhw->SCTR = SPI__SCTR;
	_uhw->TCSR = SPI__TCSR;

	_uhw->PCR_SSCMode = SPI__PCR;

	_uhw->PSCR = ~0;

	_uhw->CCR = 0;

	_uhw->DX0CR = _DX0CR;
	_uhw->DX1CR = _DX1CR;

	_uhw->TBCTR = 0; //TBCTR_SIZE8|TBCTR_LIMIT(0);
	_uhw->RBCTR = 0;//RBCTR_SIZE8|RBCTR_LIMIT(0);

	_uhw->CCR = SPI__CCR;

	_PIO_SPCK->ModePin(_PIN_SPCK, A2PP);
	_PIO_MOSI->ModePin(_PIN_MOSI, A2PP);
	_PIO_MISO->ModePin(_PIN_MISO, I0DNP);

	_MASK_CS_ALL = 0;

	for (u32 i = 0; i < _PIN_CS_LEN; i++)
	{
		_PIO_CS->ModePin(_PIN_CS[i], G_PP);
		_MASK_CS_ALL |= 1UL << _PIN_CS[i];
	};

	ChipDisable();

	_DMA->SetDlrLineNum(_DRL);

#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void S_SPIM::Disconnect()
{
	#ifdef CPU_SAME53	
		
		T_HW::S_SPI* spi = _uhw.spi;
		
		_PIO_SPCK->SetWRCONFIG(_MASK_SPCK, PORT_WRPMUX);
		_PIO_MOSI->SetWRCONFIG(_MASK_MOSI, PORT_WRPMUX);
		_PIO_MISO->SetWRCONFIG(_MASK_MISO, PORT_WRPMUX);
		_PIO_CS->SET(_MASK_CS_ALL); 
		_PIO_CS->DIRCLR = _MASK_CS_ALL; 
		_PIO_CS->SetWRCONFIG(_MASK_CS_ALL, PORT_PULLEN|PORT_WRPINCFG|PORT_WRPMUX);

		spi->CTRLA = SPI_SWRST;

		while(spi->SYNCBUSY);

		HW::MCLK->ClockDisable(_upid);

		Usic_Disconnect();

	#elif defined(CPU_XMC48)


	
	#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool S_SPIM::Connect(u32 baudrate)
{
#ifdef ADSP_BLACKFIN

	using namespace HW;

	if ((_alloc_mask & (1UL<<_num)) || _MASK_CS == 0 || _MASK_CS_LEN == 0)
	{
		return false;
	};

	_alloc_mask |= (1UL<<_num);
	
	_hw = _spi_hw[_num];

	if (baudrate == 0) baudrate = 1;

	_MASK_CS_ALL = 0;

	for (u32 i = 0; i < _MASK_CS_LEN; i++) _MASK_CS_ALL |= _MASK_CS[i];

	u16 baud = (_GEN_CLK + baudrate) / (baudrate*2);

	if (baud < 2) baud = 2;

	_baud = baud;

	InitHW();

#elif defined(__CC_ARM)

	using namespace HW;

	SEGGER_RTT_printf(0, RTT_CTRL_TEXT_BRIGHT_GREEN "SPI%u Init ... ", _usic_num);

	#ifdef CPU_SAME53	
	if (!Usic_Connect() || _MASK_CS == 0 || _MASK_CS_LEN == 0)
	#elif defined(CPU_XMC48)
	if (!Usic_Connect() || _PIN_CS == 0 || _PIN_CS_LEN == 0)
	#endif
	{
		SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_RED "!!! ERROR !!!\n");

		return false;
	};

	if (baudrate == 0) baudrate = 1;

	#ifdef CPU_SAME53	

		_MASK_CS_ALL = 0;

		for (u32 i = 0; i < _MASK_CS_LEN; i++)
		{
			_MASK_CS_ALL |= _MASK_CS[i];
		};

		u32 baud = (_GEN_CLK + baudrate/2) / baudrate;

		baud = (baud + 1) / 2;

		if (baud > 0) baud -= 1;

		if (baud > 0xFF) baud = 0xFF;

		_baud = baud;

		InitHW();

	#elif defined(CPU_XMC48)

//		__SCTR = USIC_SDIR(1) | USIC_TRM(1) | USIC_FLE(0x3F) | USIC_WLE(7);
		__FDR = ((1024 - ((_GEN_CLK + baudrate/2) / baudrate + 1) / 2) | USIC_DM(1));
//		__BRG = USIC_SCLKCFG(2)|USIC_CTQSEL(0)|USIC_DCTQ(1)|USIC_PCTQ(3)|USIC_CLKSEL(0));
//		__TCSR = (USIC_TDEN(1)|USIC_HPCMD(0));
//		__DX0CR;
//		__DX1CR;
//		__CCR = (USIC_MODE(1));
//		__PCR = (USIC_MSLSEN | USIC_SELINV |  USIC_TIWEN | USIC_MCLK | USIC_CTQSEL1(0) | USIC_PCTQ1(0) | USIC_DCTQ1(0));

		InitHW();

	#endif

	SEGGER_RTT_WriteString(0, "OK\n");

#endif

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void S_SPIM::WritePIO(const void *data, u16 count)
{
	byte *p = (byte*)data;

#ifdef __ADSPBF59x__

	_hw->Ctl = SPE|MSTR|TDBR_CORE|(_spimode&(CPOL|CPHA|LSBF));	

	while (count != 0)
	{
		_hw->TDBR = *(p++); count--;

		while (_hw->Stat & TXS) HW::ResetWDT();
	};

	while((_hw->Stat & SPIF) == 0);

	_hw->Ctl = 0;

#elif defined(__ADSPBF70x__)

	_hw->CTL	= SPI_EN|SPI_MSTR|(_spimode&SPIMODE_MASK);	
	_hw->TXCTL	= 0;
	_hw->RXCTL	= 0;//RXCTL_REN/*|RXCTL_RTI*/;
	_hw->TWC	= count;
	_hw->TWCR	= 0;
	_hw->TXCTL	= TXCTL_TEN|TXCTL_TTI|TXCTL_TWCEN;

	while (count != 0)
	{
		_hw->TFIFO = *(p++); count--;

		while (_hw->STAT & SPI_TFF) HW::ResetWDT();
	};

	while((_hw->STAT & (SPI_TF|SPI_SPIF)) != (SPI_TF|SPI_SPIF));

	_hw->TXCTL	= 0;
	_hw->RXCTL	= 0;

#elif defined(CPU_SAME53)

	while (count != 0)
	{
		_uhw.spi->DATA = *(p++); count--;

		while((_uhw.spi->INTFLAG & SPI_DRE) == 0);
	};

	while((_uhw.spi->INTFLAG & SPI_TXC) == 0);

#elif defined(CPU_XMC48)
	
#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void S_SPIM::WriteAsyncDMA(const void *data, u16 count)
{
#ifdef __ADSPBF59x__

	_hw->Ctl = GM|MSTR|TDBR_DMA|(_spimode&(CPOL|CPHA|LSBF));

	_DMA.Write8((volatile void*)data, count);

	_hw->Ctl |= SPE;

#elif defined(__ADSPBF70x__)

	_hw->STAT	= ~0;
	_hw->TXCTL	= TXCTL_TEN|TXCTL_TTI|TXCTL_TWCEN|TXCTL_TDR_EMPTY;
	_hw->RXCTL	= 0;
	_hw->CTL	= SPI_EN|SPI_MSTR|(_spimode&SPIMODE_MASK);	
	_hw->TWC	= count;
	_hw->TWCR	= 0;

	_DMATX.Write8((volatile void*)data, count);

	//_hw->TXCTL |= TXCTL_TEN;

#elif defined(CPU_SAME53)	

	_uhw.spi->INTFLAG = ~0;
	_uhw.spi->INTENCLR = ~0;
	_uhw.spi->CTRLB &= ~SPI_RXEN; while(_uhw.spi->SYNCBUSY);

	_DMATX->WritePeripheral(data, &_uhw.spi->DATA, count, DMCH_TRIGACT_BURST|(((DMCH_TRIGSRC_SERCOM0_TX>>8)+_usic_num*2)<<8), DMDSC_BEATSIZE_BYTE);

#elif defined(CPU_XMC48)

	USICHWT	&spi = _uhw;

	spi->TRBSCR = TRBSCR_FLUSHTB;
	spi->TBCTR = TBCTR_SIZE8|TBCTR_LIMIT(0);

	spi->TCSR = SPI__TCSR|USIC_TDSSM(1);

	spi->CCR = SPI__CCR;
	spi->PCR_SSCMode = SPI__PCR|SPI_SELO(1);

	spi->PSCR = ~0;

	while(spi->PSR_SSCMode & SPI_TBIF) spi->PSCR = ~0;

	_DMA->WritePeripheralByte(data, &spi->IN[4], count);

	spi->PSCR = ~0;
	spi->CCR = USIC_MODE_SPI|USIC_TBIEN;
	spi->INPR = USIC_TBINP(Get_INPR_SR())|USIC_RINP(5)|USIC_PINP(5);

	if ((spi->PSR_SSCMode & SPI_TBIF) == 0)
	{
		spi->FMR = USIC_CH_FMR_SIO0_Msk << Get_INPR_SR();
	};


#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void S_SPIM::WriteSyncDMA(const void *data, u16 count)
{
#ifndef WIN32

	WriteAsyncDMA(data, count);

	while (!CheckWriteComplete());

	DisableTX();
	
	//#ifdef __ADSPBF59x__
	//	_hw->Ctl = 0;
	//	_DMA.Disable();
	//#elif defined(__ADSPBF70x__)
	//	DisableTX();
	//#elif defined(CPU_SAME53)
	//	_DMATX->Disable();
	//#elif defined(CPU_XMC48)
	//	_DMA->Disable();
	//#endif

#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void S_SPIM::WriteAsyncDMA(const void *data1, u16 count1, const void *data2, u16 count2)
{
#ifdef __ADSPBF59x__

	_hw->Ctl = GM|MSTR|TDBR_DMA|(_spimode&(CPOL|CPHA|LSBF));

	_DMA.Write8((volatile void*)data1, count1, (volatile void*)data2, count2);

	_hw->Ctl |= SPE;

#elif defined(__ADSPBF70x__)

	_hw->STAT	= ~0;
	_hw->TXCTL	= TXCTL_TEN|TXCTL_TTI|TXCTL_TWCEN|TXCTL_TDR_EMPTY;
	_hw->RXCTL	= 0;
	_hw->CTL	= SPI_EN|SPI_MSTR|(_spimode&SPIMODE_MASK);	
	_hw->TWC	= count1+count2;
	_hw->TWCR	= 0;

	_DMATX.Write8((volatile void*)data1, count1, (volatile void*)data2, count2);

#elif defined(CPU_SAME53)

	_uhw.spi->INTFLAG = ~0;
	_uhw.spi->INTENCLR = ~0;
	_uhw.spi->CTRLB &= ~SPI_RXEN; while(_uhw.spi->SYNCBUSY);

	_DMATX->WritePeripheral(data1, &_uhw.spi->DATA, count1, data2, count2, DMCH_TRIGACT_BURST|(((DMCH_TRIGSRC_SERCOM0_TX>>8)+_usic_num*2)<<8), DMDSC_BEATSIZE_BYTE);

#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void S_SPIM::WriteSyncDMA(const void *data1, u16 count1, const void *data2, u16 count2)
{
	WriteAsyncDMA(data1, count1, data2, count2);

	while (!CheckWriteComplete());

	DisableTX();
	
	//#ifdef __ADSPBF59x__
	//	_hw->Ctl = 0;
	//	_DMA.Disable();
	//#elif defined(__ADSPBF70x__)
	//	DisableTX();
	//#elif defined(CPU_SAME53)
	//	_DMATX->Disable();
	//#elif defined(CPU_XMC48)
	//	_DMA->Disable();
	//#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void S_SPIM::ReadPIO(void *data, u16 count)
{
	volatile register byte t;
	byte *p = (byte*)data;

#ifdef __ADSPBF59x__

	_DMA.Disable();

	_hw->Ctl = SPE|MSTR|SZ|RDBR_CORE|(_spimode&(CPOL|CPHA|LSBF));	

	t = _hw->RDBR; 

	while (count != 0)
	{
		while ((_hw->Stat & RXS) == 0);

		*(p++) = _hw->RDBR; count--;
	};

	_hw->Ctl = 0;

#elif defined(__ADSPBF70x__)

	_DMATX.Disable();
	_DMARX.Disable();

	_hw->TXCTL	= 0;
	_hw->RXCTL	= 0;
	_hw->CTL	= SPI_EN|SPI_MSTR|(_spimode&SPIMODE_MASK);	
	_hw->RWC	= count;
	_hw->RWCR	= 0;

	while((_hw->STAT & SPI_RFE) == 0)
	{
		asm("%0 = W[%1];" : "=D" (t) : "p" (&(_hw->RFIFO)));
	};

	_hw->RXCTL = RXCTL_REN|RXCTL_RTI|RXCTL_RWCEN;

	while (count != 0)
	{
		while ((_hw->STAT & SPI_RFE) != 0);

		*(p++) = _hw->RFIFO.B; count--;
	};

	_hw->TXCTL	= 0;
	_hw->RXCTL	= 0;

#elif defined(CPU_SAME53)

	_uhw.spi->CTRLB |= SPI_RXEN; while(_uhw.spi->SYNCBUSY);

	do t = _uhw.spi->DATA; while(_uhw.spi->INTFLAG & SPI_RXC); 

	while (count != 0)
	{
		_uhw.spi->DATA = 0;

		while((_uhw.spi->INTFLAG & SPI_RXC) == 0);

		*(p++) = _uhw.spi->DATA; count--;
	};

#elif defined(CPU_XMC48)
	
#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void S_SPIM::ReadAsyncDMA(void *data, u16 count)
{
	volatile register byte t;

#ifdef __ADSPBF59x__

	_hw->Ctl = MSTR|SZ|RDBR_DMA|(_spimode&(CPOL|CPHA|LSBF));

	_DMA.Read8(data, count);

	_hw->Ctl |= SPE;

#elif defined(__ADSPBF70x__)

	_hw->STAT	= ~0;
	_hw->TXCTL	= 0;
	_hw->RXCTL	= 0;
	_hw->CTL	= SPI_EN|SPI_MSTR|(_spimode&SPIMODE_MASK);	
	_hw->RWC	= count;
	_hw->RWCR	= 0;

	_DMARX.Read8(data, count);

	_hw->RXCTL	= RXCTL_REN|RXCTL_RTI|RXCTL_RWCEN|RXCTL_RDR_NE;

#elif defined(CPU_SAME53)	

	_uhw.spi->INTFLAG = ~0;
	_uhw.spi->INTENCLR = ~0;
	_uhw.spi->CTRLB |= SPI_RXEN; while(_uhw.spi->SYNCBUSY);

	do t = _uhw.spi->DATA; while(_uhw.spi->INTFLAG & SPI_RXC); 

	_DMARX->ReadPeripheral(&_uhw.spi->DATA, data, count,	DMCH_TRIGACT_BURST|(((DMCH_TRIGSRC_SERCOM0_RX>>8)+_usic_num*2)<<8), DMDSC_BEATSIZE_BYTE);
	_DMATX->WritePeripheral(data, &_uhw.spi->DATA, count, DMCH_TRIGACT_BURST|(((DMCH_TRIGSRC_SERCOM0_TX>>8)+_usic_num*2)<<8), DMDSC_BEATSIZE_BYTE);

#elif defined(CPU_XMC48)

	USICHWT	&SPI = _uhw;

	SPI->RBCTR = 0;
	SPI->TBCTR = 0;

	SPI->CCR = SPI__CCR;
	SPI->PCR_SSCMode = SPI__PCR|SPI_SELO(1);

	while(SPI->PSR_SSCMode & (SPI_TBIF|SPI_RIF))
	{
		t = SPI->RBUF;
		SPI->PSCR = ~0;
	};

	_DMA->ReadPeripheralByte(&SPI->RBUF, data, count);

	SPI->INPR = USIC_RINP(Get_INPR_SR())|USIC_PINP(5)|USIC_TBINP(5);

	SPI->PSCR = ~0;
	
	SPI->CCR = SPI__CCR | USIC_RIEN;
	SPI->TCSR = SPI__TCSR|USIC_TDSSM(0);

	SPI->TBUF[0] = 0;

#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void S_SPIM::ReadSyncDMA(void *data, u16 count)
{
#ifndef WIN32

	ReadAsyncDMA(data, count);

	while (!CheckReadComplete());

	DisableRX();

	//#ifdef __ADSPBF59x__

	//	_hw->Ctl = 0;
	//	_DMA.Disable();

	//#elif defined(__ADSPBF70x__)

	//	DisableRX();

	//#elif defined(CPU_SAME53)	

	//	_uhw.spi->CTRLB &= ~SPI_RXEN;
	//	_DMATX->Disable();
	//	_DMARX->Disable();

	//#elif defined(CPU_XMC48)

	//	_DMA->Disable();

	//#endif

#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

byte S_SPIM::WriteReadByte(byte v)
{
	volatile register byte t;

#ifdef __ADSPBF59x__

	_hw->Ctl = SPE|MSTR|TDBR_CORE|(_spimode&(CPOL|CPHA|LSBF));

	_hw->TDBR = v;

	while((*pSPI0_STAT & (SPIF|RXS)) != (SPIF|RXS)) HW::ResetWDT();

	return _hw->RDBR; 

#elif defined(__ADSPBF70x__)

	_hw->CTL	= SPI_EN|SPI_MSTR|(_spimode&SPIMODE_MASK);	
	_hw->RXCTL	= RXCTL_REN;
	_hw->TXCTL	= TXCTL_TEN|TXCTL_TTI;

	_hw->TFIFO = v;
	
	while((_hw->STAT & (SPI_SPIF|SPI_RFE)) != SPI_SPIF);

	return _hw->RFIFO.B; 

#elif defined(CPU_SAME53)	

	_uhw.spi->CTRLB |= SPI_RXEN; while(_uhw.spi->SYNCBUSY);

	do t = _uhw.spi->DATA; while(_uhw.spi->INTFLAG & SPI_RXC); 

	_uhw.spi->DATA = v;

	while((_uhw.spi->INTFLAG & SPI_RXC) == 0);

	return _uhw.spi->DATA; 

#elif defined(CPU_XMC48)

	return 0;

#else	//elif defined(WIN32)

	return 0;
	
#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef __ADSPBF70x__

void S_SPIM::WriteByteSync(byte v)
{
	//_hw->CTL	= SPI_EN|SPI_MSTR|(_spimode&SPIMODE_MASK);	
	_hw->RXCTL	= 0;
	//_hw->TWC	= 1;	
	_hw->TXCTL	= TXCTL_TEN|TXCTL_TTI/*|TXCTL_TWCEN*/;
	_hw->TFIFO = v;

	while((_hw->STAT & SPI_SPIF) == 0);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void S_SPIM::WriteByteAsync(byte v)
{
	//_hw->CTL	= SPI_EN|SPI_MSTR|(_spimode&SPIMODE_MASK);	
	_hw->RXCTL	= 0;
	_hw->TXCTL	= TXCTL_TEN|TXCTL_TTI;

	while((_hw->STAT & SPI_TFF) != 0);

	_hw->TFIFO = v;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void S_SPIM::ReadByteStart(u16 count)
{
	u32 temp;

	while((_hw->STAT & SPI_RFE) == 0)	{ asm("%0 = W[%1];" : "=D" (temp) : "p" (&(_hw->RFIFO))); };

	//_hw->CTL	= SPI_EN|SPI_MSTR|(_spimode&SPIMODE_MASK);	
	_hw->TXCTL	= 0;
	_hw->RWC	= count;
	_hw->RXCTL	= RXCTL_REN|RXCTL_RTI|RXCTL_RWCEN;
}

#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool S_SPIM::AddRequest(DSCSPI *d)
{
#ifndef WIN32

	if (d == 0) { return false; };

	#ifdef ADSP_BLACKFIN
		if (d->csnum >= _MASK_CS_LEN) return false;
		if (d->baud < 2) d->baud = 2;
	#elif defined(CPU_SAME53)	
		if (d->csnum >= _MASK_CS_LEN) return false;
	#elif defined(CPU_XMC48)
		if (d->csnum >= _PIN_CS_LEN) return false;
	#endif

	d->next = 0;
	d->ready = false;

	_reqList.Add(d);

#endif

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool S_SPIM::Update()
{
	bool result = false;

#ifdef ADSP_BLACKFIN

	switch (_state)
	{
		case ST_WAIT:	//++++++++++++++++++++++++++++++++++++++++++++++
		{
			_dsc = _reqList.Get();

			if (_dsc != 0)
			{
				Disable(); //_hw->Ctl = 0;

				ChipSelect(_dsc->csnum, _dsc->mode, _dsc->baud);  //_PIO_CS->CLR(_MASK_CS[_dsc->csnum]);

				if (_dsc->alen == 0)
				{
					if (_dsc->wdata != 0 && _dsc->wlen > 0)
					{
						WriteAsyncDMA(_dsc->wdata, _dsc->wlen);

						_state = ST_WRITE; 
					}
					else if (_dsc->rdata != 0 && _dsc->rlen > 0)
					{
						ReadAsyncDMA(_dsc->rdata, _dsc->rlen);

						_state = ST_READ; 
					};
				}
				else
				{
					WriteAsyncDMA(&_dsc->adr, _dsc->alen, _dsc->wdata, _dsc->wlen);

					_state = ST_WRITE; 
				};
			};

			break;
		};

		case ST_WRITE:	//++++++++++++++++++++++++++++++++++++++++++++++
		{
			if (CheckWriteComplete())
			{
				DisableTX();	//_DMA.Disable();

				if (_dsc->rdata != 0 && _dsc->rlen > 0)
				{
					ReadAsyncDMA(_dsc->rdata, _dsc->rlen);

					_state = ST_READ; 
				}
				else
				{
					_state = ST_STOP; 
				};
			};

			break;
		};

		case ST_READ:	//++++++++++++++++++++++++++++++++++++++++++++++
		{
			if (CheckReadComplete())
			{
				DisableRX();
				//_hw->Ctl = 0;
				//_DMA.Disable();

				_state = ST_STOP; 
			};

			break;
		};

		case ST_STOP:	//++++++++++++++++++++++++++++++++++++++++++++++
		{
			_dsc->ready = true;

			ChipDisable();

			_dsc = 0;

			_state = ST_WAIT; 

			break;
		};
	};

#elif defined(CPU_SAME53)	

	T_HW::S_SPI* spi = _uhw.spi;

	switch (_state)
	{
		case WAIT:

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

					ChipSelect(_dsc->csnum);  //_PIO_CS->CLR(_MASK_CS[_dsc->csnum]);

					DSCSPI &dsc = *_dsc;

					dsc.ready = false;

					if (dsc.alen == 0)
					{
						if (dsc.wdata != 0 && dsc.wlen > 0)
						{
							WriteAsyncDMA(dsc.wdata, dsc.wlen);
							//_DMATX->WritePeripheral(dsc.wdata, &spi->DATA, dsc.wlen, DMCH_TRIGACT_BURST|(((DMCH_TRIGSRC_SERCOM0_TX>>8)+_usic_num*2)<<8), DMDSC_BEATSIZE_BYTE);

							_state = WRITE; 
						}
						else if (dsc.rdata != 0 && dsc.rlen > 0)
						{
							ReadAsyncDMA(dsc.rdata, dsc.rlen);
							//spi->CTRLB |= SPI_RXEN;
							//_DMARX->ReadPeripheral(&spi->DATA, dsc.rdata, dsc.rlen, DMCH_TRIGACT_BURST|(((DMCH_TRIGSRC_SERCOM0_RX>>8)+_usic_num*2)<<8), DMDSC_BEATSIZE_BYTE);
							//_DMATX->WritePeripheral(dsc.rdata, &spi->DATA, dsc.rlen+1, DMCH_TRIGACT_BURST|(((DMCH_TRIGSRC_SERCOM0_TX>>8)+_usic_num*2)<<8), DMDSC_BEATSIZE_BYTE);

							_state = STOP; 
						};
					}
					else
					{
						spi->INTFLAG = ~0;
						spi->INTENCLR = ~0;
						spi->CTRLB &= ~SPI_RXEN;

						_DMATX->WritePeripheral(&dsc.adr, &spi->DATA, dsc.alen, dsc.wdata, dsc.wlen, DMCH_TRIGACT_BURST|(((DMCH_TRIGSRC_SERCOM0_TX>>8)+_usic_num*2)<<8), DMDSC_BEATSIZE_BYTE);

						_state = WRITE; 
					};
				};
			};

			break;

		case WRITE:
		{
			DSCSPI &dsc = *_dsc;

			if (CheckWriteComplete())
			{
				_DMATX->Disable();

				if (dsc.rdata != 0 && dsc.rlen > 0)
				{
					ReadAsyncDMA(dsc.rdata, dsc.rlen);

					//spi->CTRLB |= SPI_RXEN;

					//_DMARX->ReadPeripheral(&spi->DATA, dsc.rdata, dsc.rlen, DMCH_TRIGACT_BURST|(((DMCH_TRIGSRC_SERCOM0_RX>>8)+_usic_num*2)<<8), DMDSC_BEATSIZE_BYTE);
					//_DMATX->WritePeripheral(dsc.rdata, &spi->DATA, dsc.rlen+1, DMCH_TRIGACT_BURST|(((DMCH_TRIGSRC_SERCOM0_TX>>8)+_usic_num*2)<<8), DMDSC_BEATSIZE_BYTE);
				};

				_state = STOP; 
			};

			break;
		};

		case STOP:
		{
			if (CheckReadComplete())
			{
				_dsc->ready = true;
				
				_dsc = 0;
				
				ChipDisable();//_PIO_CS->SET(_MASK_CS_ALL);

				_DMARX->Disable();
				_DMATX->Disable();

				spi->CTRLB &= ~SPI_RXEN;
				spi->INTFLAG = ~0;
				spi->INTENCLR = ~0;

				_state = WAIT; 

				Usic_Unlock();
			};

			break;
		};
	};

#elif defined(CPU_XMC48)

	USICHWT	&spi = _uhw;

	switch (_state)
	{
		case WAIT:

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

					ChipSelect(_dsc->csnum);  //_PIO_CS->CLR(_MASK_CS[_dsc->csnum]);

					_DMA->SetDlrLineNum(_DRL);

					DSCSPI &dsc = *_dsc;

					dsc.ready = false;

					if (dsc.alen == 0)
					{
						if (dsc.wdata != 0 && dsc.wlen > 0)
						{
							WriteAsyncDMA(dsc.wdata, dsc.wlen);

							_state = WRITE; 
						}
						else 
						{
							if (dsc.rdata != 0 && dsc.rlen > 0) ReadAsyncDMA(dsc.rdata, dsc.rlen);

							_state = STOP; 
						};
					}
					else
					{
						WriteAsyncDMA(&dsc.adr, dsc.alen);

						_state = WRITE_ADR; 
					};
				};
			};

			break;

		case WRITE_ADR:
		{
			DSCSPI &dsc = *_dsc;

			u32 psr = spi->PSR_SSCMode;

			if (/*CheckWriteComplete() && */(psr & SPI_MSLS) == 0)
			{
				_DMA->Disable();

				if (dsc.wdata != 0 && dsc.wlen > 0)
				{
					WriteAsyncDMA(dsc.wdata, dsc.wlen);

					_state = WRITE; 
				}
				else 
				{
					if (dsc.rdata != 0 && dsc.rlen > 0) ReadAsyncDMA(dsc.rdata, dsc.rlen);

					_state = STOP; 
				};
			};

			break;
		};

		case WRITE:
		{
			DSCSPI &dsc = *_dsc;

			//u32 psr = spi->PSR_SSCMode;

			if (/*CheckWriteComplete() && */(spi->PSR_SSCMode & SPI_MSLS) == 0)
			{
				_DMA->Disable();

				if (dsc.rdata != 0 && dsc.rlen > 0)	ReadAsyncDMA(dsc.rdata, dsc.rlen);

				_state = STOP; 
			};

			break;
		};

		case STOP:
		{
			if (CheckReadComplete())
			{
				_dsc->ready = true;
				
				_dsc = 0;
				
				ChipDisable();//_PIO_CS->SET(_MASK_CS_ALL);

				_DMA->Disable();

				spi->TCSR = SPI__TCSR|USIC_TDSSM(1);
				spi->CCR = SPI__CCR;
				spi->PCR_SSCMode = SPI__PCR;

				_state = WAIT; 

				Usic_Unlock();
			};

			break;
		};
	};

#endif

	return result;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // #ifndef CPU_LPC812
