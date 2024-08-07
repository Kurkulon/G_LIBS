#include "core.h"

#if !defined(CORETYPE_LPC8XX) && !defined(_ADI_COMPILER)

//#define _CRT_SECURE_NO_WARNINGS

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "time.h"
#include "trap.h"
#include "FLASH\NandFlash.h"
#include "emac.h"

#include "trap_def.h"
#include "xtrap.h"

#include "extern_def.h"

//#pragma diag_suppress 2548,546,550,177

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef WIN32
	#pragma O3
	#pragma Otime
#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef WIN32
#define VECTOR_IP_MTU			ETH_IP_MTU
#else
#define VECTOR_IP_MTU			0x4000
#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

enum trap_status
{
	TRAP_WAIT = 0,
	TRAP_SEND_SESSION,
	TRAP_SEND_VECTOR,
	TRAP_PAUSE_VECTOR
};

//static trap_status trapStatus = TRAP_WAIT;


static const bool __trace = true;

//char TrapTxDataBuffer[TRAP_TX_DATA_BUFFER_SIZE];

u32 TrapRxCounter;
u32 TrapTxCounter;
u32 TrapRxLost;

static bool startSendVector = false;
static u16  startSession = 0;
static u64	startAdr = 0;

//static bool startSendSession = false;
static bool stop = false;
static bool pause = false;

static TM32 tmReboot;
static bool cmdReboot = false;


/******************************************************/
//static void TRAP_MakePacketHeaders(char *data, bool need_ask, bool is_ask, char device);
static void MakePacketHeaders(TrapHdr *p, bool need_ask, bool is_ask, char device);

static void StartSendVector(u16 session, u64 adr);


/*********************** Info *************************/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool TRAP_INFO_SendError(u32 error)
{
	Ptr<MB> mb(AllocMemBuffer(sizeof(EthTrap)));

	if (!mb.Valid()) return false;

	EthTrap &et = *((EthTrap*)mb->GetDataPtr());

	TrapError &trap = (TrapError&)et.trap;

	MakePacketHeaders(&trap.hdr, TRAP_PACKET_NO_NEED_ASK, TRAP_PACKET_NO_ASK, TRAP_INFO_DEVICE);

	trap.hdr.cmd = TRAP_INFO_COMMAND_ERROR;
	trap.error = error;

	mb->len = sizeof(EthUdp) + sizeof(trap);
	
	SendTrap(mb);

	if (__trace) { TRAP_TRACE_PrintString(__func__); };

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool TRAP_INFO_SendCaptureIP(u32 old_ip, u16 old_port)
{
	Ptr<MB> mb(AllocMemBuffer(sizeof(EthTrap)));

	if (!mb.Valid()) return false;

	EthTrap &et = *((EthTrap*)mb->GetDataPtr());

	TrapIP &trap = (TrapIP&)et.trap;

	MakePacketHeaders(&trap.hdr, TRAP_PACKET_NO_NEED_ASK, TRAP_PACKET_NO_ASK, TRAP_INFO_DEVICE);
	trap.hdr.cmd = TRAP_INFO_CAPTURE_IP;
	trap.ip = old_ip;
	trap.port = old_port;

	mb->len = sizeof(EthUdp) + sizeof(trap);

	SendTrap(mb);

	if (__trace) { TRAP_TRACE_PrintString(__func__); };

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool TRAP_INFO_SendLostIP(u32 new_ip, u16 new_port)
{
	Ptr<MB> mb(AllocMemBuffer(sizeof(EthTrap)));

	if (!mb.Valid()) return false;

	EthTrap &et = *((EthTrap*)mb->GetDataPtr());

	TrapIP &trap = (TrapIP&)et.trap;

	MakePacketHeaders(&trap.hdr, TRAP_PACKET_NO_NEED_ASK, TRAP_PACKET_NO_ASK, TRAP_INFO_DEVICE);
	trap.hdr.cmd = TRAP_INFO_LOST_IP;
	trap.ip = new_ip;
	trap.port = new_port;

	mb->len = sizeof(EthUdp) + sizeof(trap);

	SendTrap(mb);

	if (__trace) { TRAP_TRACE_PrintString(__func__); };

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool TRAP_INFO_SendInfo()
{
	Ptr<MB> mb(AllocMemBuffer(sizeof(EthTrap)));

	if (!mb.Valid()) return false;

	EthTrap &et = *((EthTrap*)mb->GetDataPtr());

	TrapInfo &trap = (TrapInfo&)et.trap;

	MakePacketHeaders(&trap.hdr, TRAP_PACKET_NO_NEED_ASK, TRAP_PACKET_NO_ASK, TRAP_INFO_DEVICE);

	trap.hdr.cmd = TRAP_INFO_COMMAND_INFO;

	trap.version = GetVersionDevice();
	trap.number = GetNumDevice(); 
	trap.memory_mask = NandFlash_Chip_Mask_Get();
	trap.memory_size = NandFlash_Full_Size_Get();
	trap.devices_mask = TRAP_DEVICES_MASK;
	trap.device_type = 1;// ������ 
	trap.device_telemetry = 2; // USART
	
	mb->len = sizeof(EthUdp) + sizeof(trap);

	SendTrap(mb);

	if (__trace) { TRAP_TRACE_PrintString(__func__); };

	return true;
}

/******************** CLOCK ******************************/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool TRAP_CLOCK_SendMain(/*const RTC_type &rtc*/)
{
	Ptr<MB> mb(AllocMemBuffer(sizeof(EthTrap)));

	if (!mb.Valid()) return false;

	EthTrap &et = *((EthTrap*)mb->GetDataPtr());

	TrapClock &trap = (TrapClock&)et.trap;

	MakePacketHeaders(&trap.hdr, TRAP_PACKET_NO_NEED_ASK, TRAP_PACKET_NO_ASK, TRAP_CLOCK_DEVICE);

	trap.hdr.cmd = TRAP_CLOCK_COMMAND_MAIN;
	
	GetTime(&trap.rtc);	

	mb->len = sizeof(EthUdp) + sizeof(trap);

	SendTrap(mb);

//	if (__trace) { TRAP_TRACE_PrintString(__func__); };

	return true;
}

/******************** TRACE ******************************/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool TRAP_TRACE_SendData(const char *pData, u32 size)
{
	Ptr<MB> mb(AllocMemBuffer(sizeof(EthTrpHdr)+size));

	if (!mb.Valid()) return false;

	EthTrap &et = *((EthTrap*)mb->GetDataPtr());

	Trap &trap = (Trap&)et.trap;

	MakePacketHeaders(&trap.hdr, TRAP_PACKET_NO_NEED_ASK, TRAP_PACKET_NO_ASK, TRAP_TRACE_DEVICE);

	trap.hdr.cmd = TRAP_TRACE_COMMAND_MAIN;

	u32 maxlen = mb->MaxLen() - sizeof(EthTrpHdr);

	if (size > maxlen)
	{
		size = maxlen;
	};

	COPY(pData, trap.data, size);

	mb->len = sizeof(EthUdp) + sizeof(TrapHdr) + size;

	SendTrap(mb);

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool TRAP_TRACE_PrintString(const char *data, ...)
{
	Ptr<MB> mb(AllocMemBuffer(ETH_IP_MTU));

	if (!mb.Valid()) return false;

	EthTrap &et = *((EthTrap*)mb->GetDataPtr());

	Trap &trap = (Trap&)et.trap;

	MakePacketHeaders(&trap.hdr, TRAP_PACKET_NO_NEED_ASK, TRAP_PACKET_NO_ASK, TRAP_TRACE_DEVICE);

	trap.hdr.cmd = TRAP_TRACE_COMMAND_MAIN;

	va_list arglist;

    va_start(arglist, data);
    
	int i = vsnprintf((char*)trap.data, mb->MaxLen() - sizeof(EthTrpHdr) - 2, data, arglist);

	if (i < 0) i = 0;
	
	byte *dst = trap.data + i;

	*dst++ = '\r';
	*dst++ = '\n';

	i += 2;

	mb->len = sizeof(EthUdp) + sizeof(TrapHdr) + i;

	SendTrap(mb);

	va_end(arglist);

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void TRAP_TRACE_PrintChar(char data)
{
	TRAP_TRACE_SendData(&data, sizeof(data));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void TRAP_TRACE_PrintDec(int number)
{
	char buf[11];
	byte i;
	u32 n = abs(number);
	for(i = 0; i < 11; i++)	
	{
	        buf[10 - i] = n - 10 * (n / 10) + 0x30;
		n /= 10;
	}
    	i = 0;
	while ((buf[i]=='0') && (i < 10)) i++;
	if(number < 0) 
	{
		i--;
		buf[i] = '-';
	}
	TRAP_TRACE_SendData(&buf[i], 11 - i);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void TRAP_TRACE_PrintHex(u32 number)
{
	char buf[10] = { '0' , 'x' };
	byte i;
	for(i = 0; i < 8; i++)	
	{
	        buf[7 - i + 2] = number - 16*(number/16);
		if(buf[7 - i + 2] >= 10) buf[7 - i + 2] += 0x41 - 10; else buf[7 - i + 2] += 0x30;
		number /= 16;
	}
	TRAP_TRACE_SendData(buf, sizeof(buf));
}

/******************** MEMORY ******************************/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool TRAP_MEMORY_SendInfo()
{
	Ptr<MB> mb(AllocMemBuffer(sizeof(EthTrap)));

	if (!mb.Valid()) return false;

	EthTrap &et = *((EthTrap*)mb->GetDataPtr());

	TrapMemInfo &trap = (TrapMemInfo&)et.trap;

	MakePacketHeaders(&trap.hdr, TRAP_PACKET_NO_NEED_ASK, TRAP_PACKET_NO_ASK, TRAP_MEMORY_DEVICE);

	trap.hdr.cmd = TRAP_MEMORY_COMMAND_INFO;

	trap.mask = NandFlash_Chip_Mask_Get();
	trap.size = NandFlash_Full_Size_Get();
	trap.size_used = NandFlash_Used_Size_Get();

	mb->len = sizeof(EthUdp) + sizeof(trap);

	SendTrap(mb);

	if (__trace) { TRAP_TRACE_PrintString(__func__); };

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool TRAP_MEMORY_SendStatus(u32 progress, byte status)
{
	Ptr<MB> mb(AllocMemBuffer(sizeof(EthTrap)));

	if (!mb.Valid()) return false;

	EthTrap &et = *((EthTrap*)mb->GetDataPtr());

	TrapMemStatus &trap = (TrapMemStatus&)et.trap;

	MakePacketHeaders(&trap.hdr, TRAP_PACKET_NO_NEED_ASK, TRAP_PACKET_NO_ASK, TRAP_MEMORY_DEVICE);

	trap.hdr.cmd = TRAP_MEMORY_COMMAND_STATUS;

	trap.progress = progress;
	trap.status = status;

	mb->len = sizeof(EthUdp) + sizeof(trap);

	SendTrap(mb);

	//if (__trace) { TRAP_TRACE_PrintString(__func__); };

	return true;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool TRAP_MEMORY_SendSession(u16 session, i64 size, i64 last_adress, RTC_type start_rtc, RTC_type stop_rtc, byte flags)
{
	Ptr<MB> mb(AllocMemBuffer(sizeof(EthTrap)));

	if (!mb.Valid()) return false;

	EthTrap &et = *((EthTrap*)mb->GetDataPtr());

	TrapSession &trap = (TrapSession&)et.trap;

	MakePacketHeaders(&trap.hdr, TRAP_PACKET_NO_NEED_ASK, TRAP_PACKET_NO_ASK, TRAP_MEMORY_DEVICE);

	trap.hdr.cmd = TRAP_MEMORY_COMMAND_SESSION;

	trap.si.session = session;
	trap.si.size = size;
	trap.si.last_adress = last_adress;
	trap.si.start_rtc = start_rtc;
	trap.si.stop_rtc = stop_rtc;
	trap.si.flags = flags;

	mb->len = sizeof(EthUdp) + sizeof(trap);

	SendTrap(mb);

	if (__trace) { TRAP_TRACE_PrintString("TRAP_MEMORY_SendSession ses=%5hu, size=%02X %08X, adr=%02X %08X", session, (u32)(size>>32), (u32)size, (u32)(last_adress>>32), (u32)(last_adress)); };

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//static bool TRAP_MEMORY_SendLastSession(const SessionInfo *si)
//{
//	return TRAP_MEMORY_SendSession(si->session, si->size, si->last_adress, si->start_rtc, si->stop_rtc, si->flags);
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//static bool TRAP_MEMORY_SendNullSession()
//{
//	SessionInfo si;
//
//	si.size = 0;
//	si.last_adress = ~0;
//	si.session = ~0;
//
//	return TRAP_MEMORY_SendSession(si.session, si.size, si.last_adress, si.start_rtc, si.stop_rtc, si.flags);
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//bool TRAP_MEMORY_SendVector(u16 session, u16 device, RTC_type rtc, byte *data, u16 size, byte flags) 
//{
//	 // ��� �������� ������ �� ������ ������� ��� ����� ������, �� ����� ��� ������������ ������� ��������� ���������� ��� ������, � �� ����� � ����������, ������ ���� �������� ���
//	if(TRAP_TX_HEADERS_LEN + sizeof(TRAP_command_type) + sizeof(TRAP_MEMORY_vector_type) + size > sizeof(TrapTxDataBuffer)) return false;
//	TRAP_MakePacketHeaders((char *)TrapTxDataBuffer, TRAP_PACKET_NO_NEED_ASK, TRAP_PACKET_NO_ASK, TRAP_MEMORY_DEVICE);
//	TRAP_command_type c;
//	c.command = TRAP_MEMORY_COMMAND_VECTOR;
//	COPY((char *)(&c.command), (char *)(TrapTxDataBuffer) + TRAP_TX_HEADERS_LEN, sizeof(TRAP_command_type));
//	TRAP_MEMORY_vector_type v;	
//	v.session = session;
//	v.device = device;
//	v.rtc = rtc;
//	v.flags = flags;
//	memmove((char *)(TrapTxDataBuffer) + TRAP_TX_HEADERS_LEN + sizeof(TRAP_command_type), (char *)(&v.session), sizeof(TRAP_MEMORY_vector_type));
//	memmove((char *)(TrapTxDataBuffer) + TRAP_TX_HEADERS_LEN + sizeof(TRAP_command_type) + sizeof(TRAP_MEMORY_vector_type), (char *)data, size);
//	EMAC_SendData((char *)TrapTxDataBuffer, TRAP_TX_HEADERS_LEN + sizeof(TRAP_command_type) + sizeof(TRAP_MEMORY_vector_type) + size);
//
//	if (__trace) { TRAP_TRACE_PrintString(__func__); };
//
//	return true;
//}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void TRAP_BATTERY_SendStatusMessage(u16 cmd)
{
	//TRAP_MakePacketHeaders((char *)TrapTxDataBuffer, TRAP_PACKET_NO_NEED_ASK, TRAP_PACKET_NO_ASK, TRAP_BATTERY_DEVICE);
	//TRAP_command_type c;
	//c.command = cmd;
	//COPY((char *)(&c.command), (char *)(TrapTxDataBuffer) + TRAP_TX_HEADERS_LEN, sizeof(TRAP_command_type));
	//TRAP_BATTERY_status_type s;
	//FRAM_Power_Battery_Voltages_Get(&s.battery_setup_voltage, &s.battery_min_voltage, &s.battery_max_voltage);
	//FRAM_Power_Line_Voltages_Get(&s.line_setup_voltage, &s.line_min_voltage, &s.line_max_voltage);
	//FRAM_Power_Battery_Coeffs_Get(&s.battery_coeff_k, &s.battery_coeff_b);
	//FRAM_Power_Line_Coeffs_Get(&s.line_coeff_k, &s.line_coeff_b);
	//COPY((char *)(&s), (char *)(TrapTxDataBuffer) + TRAP_TX_HEADERS_LEN + sizeof(TRAP_command_type), sizeof(s));
	//EMAC_SendData((char *)TrapTxDataBuffer, TRAP_TX_HEADERS_LEN + sizeof(TRAP_command_type) + sizeof(s));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/******************** SENSORS ******************************/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool TRAP_SendAsknowlege(byte device, u32 on_packet)
{
	Ptr<MB> mb(AllocMemBuffer(sizeof(EthTrap)));

	if (!mb.Valid()) return false;

	EthTrap &et = *((EthTrap*)mb->GetDataPtr());

	TrapAsk &trap = (TrapAsk&)et.trap;

	MakePacketHeaders(&trap.hdr, TRAP_PACKET_NO_NEED_ASK, TRAP_PACKET_IS_ASK, device);

	trap.hdr.cmd = TRAP_COMMAND_ASKNOWLEGE;
	trap.on_packet = on_packet;

	mb->len = sizeof(EthUdp) + sizeof(TrapAsk);
	
	SendTrap(mb);

	if (__trace) { TRAP_TRACE_PrintString("%s('%c', 0x%08X)", __func__, device, on_packet); };

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void TRAP_HandleRxData(Ptr<MB> &mb)
{
	EthTrap *et = ((EthTrap*)mb->GetDataPtr());

	Trap *t = &et->trap;

	//if (__trace) TRAP_TRACE_PrintString("TRAP_Request 0x%04hX", t->hdr.cmd);

	u32 size = et->eudp.udp.len - sizeof(et->eudp.udp);

	if(size < TRAP_RX_HEADERS_LEN) { TRAP_INFO_SendError(TRAP_PACKET_ERROR_UNKNOW); return; }

//	TRAP_RX_PACKET_type *packet = (TRAP_RX_PACKET_type *)data;

	if((TrapRxCounter == 0) || (t->hdr.counter <= 1))
	{
		TrapRxLost = 0;
	}
	else
	{
		TrapRxLost += t->hdr.counter - TrapRxCounter - 1;	
	};

	TrapRxCounter = t->hdr.counter;

	bool need_ask = (((t->hdr.status)>>3)&0x1);

	bool is_ask = (((t->hdr.status)>>2)&0x1);

	byte version = t->hdr.version;

	if(version > TRAP_PACKET_VERSION)
	{
		TRAP_INFO_SendError(TRAP_PACKET_ERROR_VERSION);
		return;
	};

	if(is_ask == TRAP_PACKET_NO_ASK)
	{
//		TRAP_command_type *cmd = (TRAP_command_type *)(data + TRAP_RX_HEADERS_LEN);	

		if(size < TRAP_RX_HEADERS_LEN + sizeof(TRAP_command_type))
		{ 
			TRAP_INFO_SendError(TRAP_PACKET_ERROR_UNKNOW); 
			return; 
		};

		switch (t->hdr.device)
		{
			case TRAP_INFO_DEVICE: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

				//TrapInfoSet &ts = (TrapInfoSet&)*t;

				switch (t->hdr.cmd)
				{
					case TRAP_INFO_COMMAND_GET_INFO:

						if(need_ask == TRAP_PACKET_NEED_ASK) TRAP_SendAsknowlege(TRAP_INFO_DEVICE, TrapRxCounter);					
						TRAP_INFO_SendInfo();
						break;

					case TRAP_INFO_COMMAND_SET_NUMBER:

						if(need_ask == TRAP_PACKET_NEED_ASK) TRAP_SendAsknowlege(TRAP_INFO_DEVICE, TrapRxCounter);
						
						{ TrapInfoSet &ts = (TrapInfoSet&)*t; SetNumDevice(ts.number); };
						
						SaveMainParams();

						break;

					case TRAP_INFO_COMMAND_SET_TYPE:

						if(need_ask == TRAP_PACKET_NEED_ASK) TRAP_SendAsknowlege(TRAP_INFO_DEVICE, TrapRxCounter);
//						FRAM_Main_Device_Type_Set(ts.type);
						break;

					case TRAP_INFO_COMMAND_SET_TELEMETRY:

						if(need_ask == TRAP_PACKET_NEED_ASK) TRAP_SendAsknowlege(TRAP_INFO_DEVICE, TrapRxCounter);
//						FRAM_Main_Device_Telemetry_Set(ts.telemetry);
						break;

					default: 

						TRAP_INFO_SendError(TRAP_PACKET_ERROR_UNKNOW);
						break;
				};

				break;

			case TRAP_CLOCK_DEVICE: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

				switch (t->hdr.cmd)
				{
					case TRAP_CLOCK_COMMAND_GET:

						if(need_ask == TRAP_PACKET_NEED_ASK) TRAP_SendAsknowlege(TRAP_CLOCK_DEVICE, TrapRxCounter);					
						TRAP_CLOCK_SendMain(/*RTC_Get()*/);
						break;

					case TRAP_CLOCK_COMMAND_SET:

						//TrapClock &tc = (TrapClock&)*t;

						if(need_ask == TRAP_PACKET_NEED_ASK) TRAP_SendAsknowlege(TRAP_CLOCK_DEVICE, TrapRxCounter);	

						{ TrapClock &tc = (TrapClock&)*t; SetClock(tc.rtc); };

						if (__trace) { TRAP_TRACE_PrintString(" TRAP_CLOCK_COMMAND_SET \r\n"); };

						break;

					default: 

						TRAP_INFO_SendError(TRAP_PACKET_ERROR_UNKNOW);
						break;
				};

				break;

			case TRAP_MEMORY_DEVICE: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

				switch (t->hdr.cmd)
				{
					case TRAP_MEMORY_COMMAND_GET_INFO:

						if(need_ask == TRAP_PACKET_NEED_ASK) TRAP_SendAsknowlege(TRAP_MEMORY_DEVICE, TrapRxCounter);					
						TRAP_MEMORY_SendInfo();
						break;

					case TRAP_MEMORY_COMMAND_READ_SESSION_START:

						if(need_ask == TRAP_PACKET_NEED_ASK) TRAP_SendAsknowlege(TRAP_MEMORY_DEVICE, TrapRxCounter);

						NandFlash_StartSendSession();

						//TRAP_MEMORY_SendLastSession(GetLastSessionInfo());
						//TRAP_MEMORY_SendStatus(-1, NANDFL_STAT_READ_SESSION_READY);

//						TRAP_MEMORY_SendNullSession();

//						Mode_Ethernet_Flash_Read_Session_Start();
						break;

					case TRAP_MEMORY_COMMAND_READ_VECTOR_START:

						if(need_ask == TRAP_PACKET_NEED_ASK) TRAP_SendAsknowlege(TRAP_MEMORY_DEVICE, TrapRxCounter);

						{ TrapReadVector &tr = (TrapReadVector&)*t; StartSendVector(tr.session, tr.last_adress); };

//						Mode_Ethernet_Flash_Read_Vector_Start(tr.session, tr.last_adress);

						break;

					case TRAP_MEMORY_COMMAND_STOP:

						if(need_ask == TRAP_PACKET_NEED_ASK) TRAP_SendAsknowlege(TRAP_MEMORY_DEVICE, TrapRxCounter);					

						stop = true;

						NandFlash_SendStatus(~0, NANDFL_STAT_STOP);

//						Mode_Ethernet_Flash_Stop();
						break;

					case TRAP_MEMORY_COMMAND_PAUSE:

						if(need_ask == TRAP_PACKET_NEED_ASK) TRAP_SendAsknowlege(TRAP_MEMORY_DEVICE, TrapRxCounter);	

						pause = true;

						NandFlash_SendStatus(~0, NANDFL_STAT_PAUSE);

						break;

					case TRAP_MEMORY_COMMAND_RESUME:

						if(need_ask == TRAP_PACKET_NEED_ASK) TRAP_SendAsknowlege(TRAP_MEMORY_DEVICE, TrapRxCounter);

						pause = false;

						NandFlash_SendStatus(~0, NANDFL_STAT_RESUME);

						break;

					case TRAP_MEMORY_COMMAND_ERASE:

						if(need_ask == TRAP_PACKET_NEED_ASK) TRAP_SendAsknowlege(TRAP_MEMORY_DEVICE, TrapRxCounter);	

						NandFlash_FullErase();

//						Mode_Ethernet_Flash_Erase();
						break;

					case TRAP_MEMORY_COMMAND_UNERASE:

						if(need_ask == TRAP_PACKET_NEED_ASK) TRAP_SendAsknowlege(TRAP_MEMORY_DEVICE, TrapRxCounter);

						NandFlash_UnErase();

						break;

					default: 

						TRAP_INFO_SendError(TRAP_PACKET_ERROR_UNKNOW);
						break;
				};

				break;

			case TRAP_BOOTLOADER_DEVICE: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

				switch (t->hdr.cmd)
				{
					case TRAP_BOOTLOADER_COMMAND_START:

						if(need_ask == TRAP_PACKET_NEED_ASK) TRAP_SendAsknowlege(TRAP_BOOTLOADER_DEVICE, TrapRxCounter);

						cmdReboot = true;
						tmReboot.Reset();

						break;

					default: 

						TRAP_INFO_SendError(TRAP_PACKET_ERROR_UNKNOW);

						break;
				};

				break;



			default: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

				TRAP_INFO_SendError(TRAP_PACKET_ERROR_UNKNOW);

				break;

		}
	}
	else 
	{
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void EMAC_HandleRxError()
{
	TRAP_INFO_SendError(TRAP_PACKET_ERROR_CHECKSUM);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void MakePacketHeaders(TrapHdr *p, bool need_ask, bool is_ask, char device)
{
//	TRAP_TX_PACKET_type p;
	p->counter = TrapTxCounter++;
	p->errors = (u16)TrapRxLost;
	p->version = TRAP_PACKET_VERSION;
	p->status = ((is_ask&0x1)<<2) | ((need_ask&0x1)<<3);
	p->device = device;
//        COPY((char *)(&p.counter), (char *)data, sizeof(TRAP_TX_PACKET_type));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//static void TRAP_MakePacketHeaders(char *data, bool need_ask, bool is_ask, char device)
//{
//	TRAP_TX_PACKET_type p;
//	p.counter = TrapTxCounter++;
//	p.errors = (u16)TrapRxLost;
//	p.version = TRAP_PACKET_VERSION;
//	p.status = ((is_ask&0x1)<<2) | ((need_ask&0x1)<<3);
//	p.device = device;
//        COPY((char *)(&p.counter), (char *)data, sizeof(TRAP_TX_PACKET_type));
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void StartSendVector(u16 session, u64 adr)
{
	startSendVector = true;
	stop = false;
	pause = false;
	startSession = session;
	startAdr = adr;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//#define UPDATE_SEND_VECTOR_NEW

#ifdef UPDATE_SEND_VECTOR_NEW

static bool UpdateSendVector()
{
	static byte i = 0;
	static NANDFLRB flrb;

	static Ptr<MB> mb;

	static ListPtr<MB> mbList;

//	static VecData::Hdr h;

	static u64 vecCount = 0;
	static u32 fragLen = 0;
	static u32 fragOff = 0;
	static u16 ipID = 0;
	//static u16 crc = 0;

	static TM32 tm;

	static u16 ses = 0;
	static u64 adr = 0;
	static u64 size = 0;
	static u64 flashFullSize = 0x200000000ULL;
	static bool useadr = false;

	static NandFileDsc *si = 0;

	__packed struct TRP { EthUdp eu; TrapVector tv; byte data[VECTOR_IP_MTU - sizeof(UdpHdr) - sizeof(TrapVector)]; };
	__packed struct FR  { EthIp  ei; byte data[VECTOR_IP_MTU]; };

	switch (i)
	{
		case 0:

			if (startSendVector)
			{
				startSendVector = false;
				adr = startAdr;
				ses = startSession;
				useadr = true;

				si = NandFlash_GetSessionInfo(ses, adr);

				if (si != 0)
				{
					size = si->size;
				};

				flashFullSize = NandFlash_Full_Size_Get();

				if (size > flashFullSize)
				{
					size = flashFullSize;
				};

				vecCount = 0;

				NandFlash_SendStatus(0, NANDFL_STAT_READ_VECTOR_IDLE);

				i++;
			}
			else
			{
				return false;
			};

			break;

		case 1:

			if (tm.Check(200))
			{
				NandFlash_SendStatus(vecCount * (1<<22) / (size/1024), NANDFL_STAT_READ_VECTOR_IDLE);
			};

			if (stop)
			{
				stop = false;

				i = 0;
			}
			else if (!pause)
			{
				mb = AllocMemBuffer(sizeof(TRP));

				if (mb.Valid())
				{
					TRP &et = *((TRP*)mb->GetDataPtr());

					flrb.data = et.data;
					flrb.maxLen = sizeof(et.data);
					flrb.vecStart = true;
					flrb.useAdr = useadr;
					flrb.adr = adr;

					NandFlash_RequestRead(&flrb);

					useadr = false;

					i++;
				};
			};

			break;

		case 2:

			if (flrb.ready)
			{
				if (flrb.len == 0 || flrb.hdr.session != ses || vecCount > flashFullSize)
				{
					mb->len = 0;

					NandFlash_SendStatus(~0, NANDFL_STAT_READ_VECTOR_READY);

					stop = false;
					pause = false;

					i = 0;
				}
				else // if (flrb.hdr.crc == 0)
				{
					TRP &et = *((TRP*)mb->GetDataPtr());

					TrapVector &trap = et.tv;

					MakePacketHeaders(&trap.hdr, TRAP_PACKET_NO_NEED_ASK, TRAP_PACKET_NO_ASK, TRAP_MEMORY_DEVICE);

					trap.hdr.cmd = TRAP_MEMORY_COMMAND_VECTOR;
					trap.session = flrb.hdr.session;
					trap.device = flrb.hdr.device;
					trap.rtc = flrb.hdr.rtc;
					trap.flags = flrb.hdr.flags;

					vecCount += flrb.hdr.dataLen;

					ipID = GetIpID(); 

					et.eu.iph.id = ipID;
					et.eu.iph.off = 0;

					mb->len = sizeof(et.eu) + sizeof(et.tv) + flrb.len;

					if (flrb.hdr.dataLen > flrb.maxLen)
					{
						fragOff = mb->len - sizeof(EthIp); //flrb.maxLen;
						fragLen = flrb.hdr.dataLen - flrb.maxLen;

						et.eu.iph.off |= 0x2000;

						et.eu.udp.len = sizeof(UdpHdr) + sizeof(trap) + flrb.hdr.dataLen - 2;

						mbList.Add(mb);

						i = 3;
					}
					else
					{
						mb->len -=  (flrb.crc != 0) ? flrb.len : 2;

						if (flrb.crc != 0) TRAP_TRACE_PrintString("Send vector CRC Error !!!");

						SendFragTrap(mb);

						i = 1;
					};

				};
			};

			break;

		case 3:

			mb = AllocMemBuffer(sizeof(FR));

			if (mb.Valid())
			{
				FR  &ef = *((FR*)mb->GetDataPtr());

				flrb.data = ef.data;
				flrb.maxLen = sizeof(ef.data);
				flrb.vecStart = false;
				flrb.useAdr = false;

				if (flrb.maxLen > fragLen) { flrb.maxLen = fragLen; };

				NandFlash_RequestRead(&flrb);

				i++;
			};

			break;

		case 4:

			if (flrb.ready)
			{
				if (flrb.len == 0)
				{
					i = 1;
				}
				else
				{
					FR  &ef = *((FR*)mb->GetDataPtr());

					ef.ei.iph.id = ipID;
					ef.ei.iph.off = (fragOff/8)&0x1FFF;

					fragLen -= flrb.len;
					fragOff += flrb.len;

					mb->len = sizeof(ef.ei) + flrb.len;

					if (fragLen > 0)
					{ 
						ef.ei.iph.off |= 0x2000;

						mbList.Add(mb);
						
						i = 3;
					}
					else 
					{
						if (flrb.crc != 0)
						{
							while(mb.Valid()) mb = mbList.Get();

							TRAP_TRACE_PrintString("Send vector CRC Error !!!");
						}
						else
						{
							mb->len -= 2;

							mbList.Add(mb);

							while(mb.Valid()) SendFragTrap(mb = mbList.Get());
						};

						i = 1;
					};
				};
			};

			break;
	};

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#else

static bool UpdateSendVector()
{
	static byte i = 0;
	static NANDFLRB flrb;

	static Ptr<MB> mb;
//	static VecData::Hdr h;

	static u64 vecCount = 0;
	static u32 fragLen = 0;
	static u32 fragOff = 0;
	static u16 ipID = 0;
	//static u16 crc = 0;

	static TM32 tm;

	static u16 ses = 0;
	static u64 adr = 0;
	static u64 size = 0;
	static u64 flashFullSize = 0x200000000ULL;
	static bool useadr = false;

	static NandFileDsc *si = 0;

	__packed struct TRP { EthUdp eu; TrapVector tv; byte data[VECTOR_IP_MTU - sizeof(UdpHdr) - sizeof(TrapVector)]; };
	__packed struct FR  { EthIp  ei; byte data[VECTOR_IP_MTU]; };

	switch (i)
	{
		case 0:

			if (startSendVector)
			{
				startSendVector = false;
				adr = startAdr;
				ses = startSession;
				useadr = true;

				si = NandFlash_GetSessionInfo(ses, adr);

				if (si != 0)
				{
					size = si->size;
				};

				flashFullSize = NandFlash_Full_Size_Get();

				if (size > flashFullSize)
				{
					size = flashFullSize;
				};

				vecCount = 0;

				NandFlash_SendStatus(0, NANDFL_STAT_READ_VECTOR_IDLE);

				i++;
			}
			else
			{
				return false;
			};

			break;

		case 1:

			if (tm.Check(200))
			{
				NandFlash_SendStatus(vecCount * (1<<22) / (size/1024), NANDFL_STAT_READ_VECTOR_IDLE);
			};

			if (stop)
			{
				stop = false;

				i = 0;
			}
			else if (!pause)
			{
				mb = AllocMemBuffer(sizeof(TRP));

				if (mb.Valid())
				{
					TRP &et = *((TRP*)mb->GetDataPtr());

					flrb.data = et.data;
					flrb.maxLen = sizeof(et.data);
					flrb.vecStart = true;
					flrb.useAdr = useadr;
					flrb.adr = adr;

					NandFlash_RequestRead(&flrb);

					useadr = false;

					i++;
				};
			};

			break;

		case 2:

			if (flrb.ready)
			{
				if (flrb.len == 0 || flrb.hdr.session != ses || vecCount > flashFullSize)
				{
					mb->len = 0;

					NandFlash_SendStatus(~0, NANDFL_STAT_READ_VECTOR_READY);

					stop = false;
					pause = false;

					i = 0;
				}
				else // if (flrb.hdr.crc == 0)
				{
					TRP &et = *((TRP*)mb->GetDataPtr());

					TrapVector &trap = et.tv;

					MakePacketHeaders(&trap.hdr, TRAP_PACKET_NO_NEED_ASK, TRAP_PACKET_NO_ASK, TRAP_MEMORY_DEVICE);

					trap.hdr.cmd = TRAP_MEMORY_COMMAND_VECTOR;
					trap.session = flrb.hdr.session;
					trap.device = flrb.hdr.device;
					trap.rtc = flrb.hdr.rtc;
					trap.flags = flrb.hdr.flags;

					vecCount += flrb.hdr.dataLen;

					ipID = GetIpID(); 

					et.eu.iph.id = ipID;
					et.eu.iph.off = 0;

					mb->len = sizeof(et.eu) + sizeof(et.tv) + flrb.len;

					if (flrb.hdr.dataLen > flrb.maxLen)
					{
						fragOff = mb->len - sizeof(EthIp); //flrb.maxLen;
						fragLen = flrb.hdr.dataLen - flrb.maxLen;

						et.eu.iph.off |= 0x2000;

						et.eu.udp.len = sizeof(UdpHdr) + sizeof(trap) + flrb.hdr.dataLen - 2;

						i = 3;
					}
					else
					{
						mb->len -=  (flrb.crc != 0) ? flrb.len : 2;

						if (flrb.crc != 0) TRAP_TRACE_PrintString("Send vector CRC Error !!!");

						i = 1;
					};

					SendFragTrap(mb);
				};
			};

			break;

		case 3:

			mb = AllocMemBuffer(sizeof(FR));

			if (mb.Valid())
			{
				FR  &ef = *((FR*)mb->GetDataPtr());

				flrb.data = ef.data;
				flrb.maxLen = sizeof(ef.data);
				flrb.vecStart = false;
				flrb.useAdr = false;

				if (flrb.maxLen > fragLen) { flrb.maxLen = fragLen; };

				NandFlash_RequestRead(&flrb);

				i++;
			};

			break;

		case 4:

			if (flrb.ready)
			{
				if (flrb.len == 0)
				{
					i = 1;
				}
				else
				{
					FR  &ef = *((FR*)mb->GetDataPtr());

					ef.ei.iph.id = ipID;
					ef.ei.iph.off = (fragOff/8)&0x1FFF;

					fragLen -= flrb.len;
					fragOff += flrb.len;

					mb->len = sizeof(ef.ei) + flrb.len;

					if (fragLen > 0)
					{ 
						ef.ei.iph.off |= 0x2000; 
						i = 3;
					}
					else 
					{
						if (flrb.crc != 0)
						{
							mb->len = 0;
							
							TRAP_TRACE_PrintString("Send vector CRC Error !!!");
						}
						else
						{
							mb->len -= 2;
						};

						i = 1;
					};

					SendFragTrap(mb);
				};
			};

			break;
	};

	return true;
}

#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/*
static bool UpdateSendVector_Dlya_Vova()
{
	static byte i = 0;
//	static NANDFLRB flrb;

	static Ptr<MB> mb;
//	static VecData::Hdr h;

	static u32 vecCount = 0;
//	static u32 fragLen = 0;
//	static u32 fragOff = 0;
//	static u16 ipID = 0;
//	static u16 crc = 0;

	static TM32 tm;

	static u16 ses = 0;
	//static u64 adr = 0;
	static u32 size = 0;
	//static bool useadr = false;

	static RTC_type rtc;

	//static NandFileDsc *si = 0;

	__packed struct TRP { EthUdp eu; TrapVector tv; byte data[VECTOR_IP_MTU - sizeof(UdpHdr) - sizeof(TrapVector)]; };
	//__packed struct FR  { EthIp  ei; byte data[ETH_IP_MTU]; };


	switch (i)
	{
		case 0:

			if (startSendVector)
			{
				startSendVector = false;
				//adr = startAdr;
				ses = startSession;
				//useadr = true;

				//si = NandFlash_GetSessionInfo(ses, adr);

				size = 20000000;

				vecCount = 0;

				GetTime(&rtc);

				i++;
			}
			else
			{
				return false;
			};

			break;

		case 1:

			if (tm.Check(200))
			{
				NandFlash_SendStatus((u64)vecCount*(1<<22)/(size>>10), NANDFL_STAT_READ_VECTOR_IDLE);
			};

			if (vecCount >= size)
			{
				NandFlash_SendStatus(~0, NANDFL_STAT_READ_VECTOR_READY);

				stop = false;
				pause = false;

				i = 0;
			}
			else if (stop)
			{
				stop = false;

				i = 0;
			}
			else if (!pause)
			{
				mb = AllocMemBuffer(sizeof(TRP));

				if (mb.Valid())
				{
					TRP &et = *((TRP*)mb->GetDataPtr());

					TrapVector &trap = et.tv;

					MakePacketHeaders(&trap.hdr, TRAP_PACKET_NO_NEED_ASK, TRAP_PACKET_NO_ASK, TRAP_MEMORY_DEVICE);

					trap.hdr.cmd = TRAP_MEMORY_COMMAND_VECTOR;
					trap.session = ses;
					trap.device = 0xAD00;
					trap.rtc = rtc;
					trap.flags = 0;
					
					if (rtc.msec < 999)
					{
						rtc.msec += 1;
					}
					else
					{
						rtc.msec = 0;

						if (rtc.sec < 59)
						{
							rtc.sec += 1;
						}
						else
						{
							rtc.sec = 0;

							if (rtc.min < 59)
							{
								rtc.min += 1;
							}
							else
							{
								rtc.min = 0;

								if (rtc.hour < 23)
								{
									rtc.hour += 1;
								}
								else
								{
									rtc.hour = 0;

									byte day = 30;

									if (rtc.day < day)
									{
										rtc.day += 1;
									}
									else
									{
										rtc.day = 1;

										if (rtc.mon < 12)
										{
											rtc.mon += 1;
										}
										else
										{
											rtc.mon = 1;

											rtc.year += 1;
										};
									};
								};
							};
						};
					};

					vecCount += 2;

					et.eu.iph.off = 0;

					et.data[0] = 0x40;
					et.data[1] = 0xAD;

					mb->len = sizeof(et.eu) + sizeof(et.tv) + 0x1000;

					SendTrap(mb);

					//i++;
				};
			};

			break;
	};

	return true;
}
*/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void TRAP_Init()
{
	TrapRxCounter = 0;
	TrapTxCounter = 0;
	TrapRxLost = 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void TRAP_Idle()
{
	#ifndef WIN32
					if (cmdReboot && tmReboot.Check(100)) HW::SystemReset(); 
	#endif

	UpdateSendVector();
	//UpdateSendVector_Dlya_Vova();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // #ifndef CORETYPE_LPC8XX

