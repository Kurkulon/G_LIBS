#ifndef CRC_CCITT_DMA_H__24_11_2023__15_49
#define CRC_CCITT_DMA_H__24_11_2023__15_49

#include "types.h"

//extern u16	CRC_CCITT_PIO(const void *data, u32 len, u16 init = ~0);
extern u16	CRC_CCITT_DMA(const void *data, u32 len, u16 init = ~0);
extern bool	CRC_CCITT_DMA_Async(const void* data, u32 len, u16 init = ~0);
extern bool	CRC_CCITT_DMA_CheckComplete(u16* crc);
extern bool CRC_CCITT_PIO(const void *data, u32 len, u16 *crc, u16 init = ~0);

#endif // CRC_CCITT_DMA_H__24_11_2023__15_49
