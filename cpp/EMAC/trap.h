#ifndef MEMORY_AT91SAM7X256_TRAP_H
#define MEMORY_AT91SAM7X256_TRAP_H

#pragma once

#include "MEM\mem.h"
#include "time.h"

extern void TRAP_HandleRxData(Ptr<MB> &mb);
extern void TRAP_HandleRxError();
extern void TRAP_Init();

extern bool TRAP_INFO_SendCaptureIP(u32 old_ip, u16 old_port);
extern bool TRAP_INFO_SendLostIP(u32 new_ip, u16 new_port);

extern bool TRAP_TRACE_SendData(const char *pData, u32 size);
extern bool TRAP_TRACE_PrintString(const char *data, ...);
extern void TRAP_TRACE_PrintChar(char data);
extern void TRAP_TRACE_PrintDec(int number);
extern void TRAP_TRACE_PrintHex(u32 number);

extern bool TRAP_MEMORY_SendInfo();
extern bool TRAP_MEMORY_SendStatus(u32 progress, byte status);
extern bool TRAP_MEMORY_SendSession(u16 session, i64 size, i64 last_adress, RTC_type start_rtc, RTC_type stop_rtc, byte flags);
extern bool TRAP_MEMORY_SendVector(u16 session, u16 device, RTC_type rtc, byte *data, u16 size, byte flags);

extern bool TRAP_CLOCK_SendMain(/*const RTC_type &rtc*/);

extern bool TRAP_VECTOR_SendVector(u16 command, u32 time_ms, int depth, int speed, byte flags, u16 size, byte *data);

extern bool RequestTrap(Ptr<MB> &mb);
extern void UpdateTraps();

extern void SendTrap(Ptr<MB> &mb);
extern void SendFragTrap(Ptr<MB> &mb);
inline bool IsComputerFind() {	extern bool ComputerFind; return ComputerFind; }

#endif


