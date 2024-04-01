// SPDX-License-Identifier: GPL-2.0-or-later WITH eCos-exception-2.0
  
/*
* This file contains an ECC algorithm from Toshiba that allows for detection
* and correction of 1-bit errors in a 256 byte block of data.
*
* [ Extracted from the initial code found in some early Linux versions.
*   The current Linux code is bigger while being faster, but this is of
*   no real benefit when the bottleneck largely remains the JTAG link.  ]
*
* Copyright (C) 2000-2004 Steven J. Hill (sjhill at realitydiluted.com)
*                         Toshiba America Electronics Components, Inc.
*
* Copyright (C) 2006 Thomas Gleixner <tglx at linutronix.de>
*/

#include "types.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define ECC_STRUCT

union RX
{
	int d;

	struct
	{
		int B0 : 1;
		int B1 : 1;
		int B2 : 1;
		int B3 : 1;
		int B4 : 1;
		int B5 : 1;
		int B6 : 1;
		int B7 : 1;
	} b;
};

union UECC
{
	int d;
	byte b8[3];

	struct
	{
		int B0 : 1;
		int B1 : 1;
		int B2 : 1;
		int B3 : 1;
		int B4 : 1;
		int B5 : 1;
		int B6 : 1;
		int B7 : 1;
		int B8 : 1;
		int B9 : 1;
		int B10 : 1;
		int B11 : 1;
		int B12 : 1;
		int B13 : 1;
		int B14 : 1;
		int B15 : 1;
		int B16 : 1;
		int B17 : 1;
		int B18 : 1;
		int B19 : 1;
		int B20 : 1;
		int B21 : 1;
		int B22 : 1;
		int B23 : 1;
	} b;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
/*
* Pre-calculated 256-way 1 byte column parity
*/

static const byte nand_ecc_precalc_table[] = 
{
    0x00, 0x55, 0x56, 0x03, 0x59, 0x0c, 0x0f, 0x5a, 0x5a, 0x0f, 0x0c, 0x59, 0x03, 0x56, 0x55, 0x00,
    0x65, 0x30, 0x33, 0x66, 0x3c, 0x69, 0x6a, 0x3f, 0x3f, 0x6a, 0x69, 0x3c, 0x66, 0x33, 0x30, 0x65,
    0x66, 0x33, 0x30, 0x65, 0x3f, 0x6a, 0x69, 0x3c, 0x3c, 0x69, 0x6a, 0x3f, 0x65, 0x30, 0x33, 0x66,
    0x03, 0x56, 0x55, 0x00, 0x5a, 0x0f, 0x0c, 0x59, 0x59, 0x0c, 0x0f, 0x5a, 0x00, 0x55, 0x56, 0x03,
    0x69, 0x3c, 0x3f, 0x6a, 0x30, 0x65, 0x66, 0x33, 0x33, 0x66, 0x65, 0x30, 0x6a, 0x3f, 0x3c, 0x69,
    0x0c, 0x59, 0x5a, 0x0f, 0x55, 0x00, 0x03, 0x56, 0x56, 0x03, 0x00, 0x55, 0x0f, 0x5a, 0x59, 0x0c,
    0x0f, 0x5a, 0x59, 0x0c, 0x56, 0x03, 0x00, 0x55, 0x55, 0x00, 0x03, 0x56, 0x0c, 0x59, 0x5a, 0x0f,
    0x6a, 0x3f, 0x3c, 0x69, 0x33, 0x66, 0x65, 0x30, 0x30, 0x65, 0x66, 0x33, 0x69, 0x3c, 0x3f, 0x6a,
    0x6a, 0x3f, 0x3c, 0x69, 0x33, 0x66, 0x65, 0x30, 0x30, 0x65, 0x66, 0x33, 0x69, 0x3c, 0x3f, 0x6a,
    0x0f, 0x5a, 0x59, 0x0c, 0x56, 0x03, 0x00, 0x55, 0x55, 0x00, 0x03, 0x56, 0x0c, 0x59, 0x5a, 0x0f,
    0x0c, 0x59, 0x5a, 0x0f, 0x55, 0x00, 0x03, 0x56, 0x56, 0x03, 0x00, 0x55, 0x0f, 0x5a, 0x59, 0x0c,
    0x69, 0x3c, 0x3f, 0x6a, 0x30, 0x65, 0x66, 0x33, 0x33, 0x66, 0x65, 0x30, 0x6a, 0x3f, 0x3c, 0x69,
    0x03, 0x56, 0x55, 0x00, 0x5a, 0x0f, 0x0c, 0x59, 0x59, 0x0c, 0x0f, 0x5a, 0x00, 0x55, 0x56, 0x03,
    0x66, 0x33, 0x30, 0x65, 0x3f, 0x6a, 0x69, 0x3c, 0x3c, 0x69, 0x6a, 0x3f, 0x65, 0x30, 0x33, 0x66,
    0x65, 0x30, 0x33, 0x66, 0x3c, 0x69, 0x6a, 0x3f, 0x3f, 0x6a, 0x69, 0x3c, 0x66, 0x33, 0x30, 0x65,
    0x00, 0x55, 0x56, 0x03, 0x59, 0x0c, 0x0f, 0x5a, 0x5a, 0x0f, 0x0c, 0x59, 0x03, 0x56, 0x55, 0x00
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//#ifdef ECC_STRUCT
//
//void Nand_Calculate_ECC(const byte *dat, byte *ecc_code)
//{
//    int idx;
//	RX reg1, reg2, reg3, tmp1, tmp2;
//
//    /* Initialize variables */
//    reg1.d = reg2.d = reg3.d = 0;
//  
//    /* Build up column parity */
//    for (int i = 0; i < 256; i++)
//    {
//        /* Get CP0 - CP5 from table */
//        idx = nand_ecc_precalc_table[*dat++];
//        reg1.d ^= (idx & 0x3f);
//  
//        /* All bit XOR = 1 ? */
//        if (idx & 0x40)
//        {
//            reg3.d ^= i;
//            reg2.d ^= ~i;
//        };
//    };
//  
//    /* Create non-inverted ECC code from line parity */
//	tmp1.b.B7 = reg3.b.B7; /* B7 -> B7 */
//    tmp1.b.B6 = reg2.b.B7; /* B7 -> B6 */
//	tmp1.b.B5 = reg3.b.B6; /* B6 -> B5 */
//	tmp1.b.B4 = reg2.b.B6; /* B6 -> B4 */
//	tmp1.b.B3 = reg3.b.B5; /* B5 -> B3 */
//	tmp1.b.B2 = reg2.b.B5; /* B5 -> B2 */
//	tmp1.b.B1 = reg3.b.B4; /* B4 -> B1 */
//    tmp1.b.B0 = reg2.b.B4; /* B4 -> B0 */
//  
//    tmp2.b.B7 = reg3.b.B3; /* B3 -> B7 */
//    tmp2.b.B6 = reg2.b.B3; /* B3 -> B6 */
//    tmp2.b.B5 = reg3.b.B2; /* B2 -> B5 */
//    tmp2.b.B4 = reg2.b.B2; /* B2 -> B4 */
//    tmp2.b.B3 = reg3.b.B1; /* B1 -> B3 */
//    tmp2.b.B2 = reg2.b.B1; /* B1 -> B2 */
//    tmp2.b.B1 = reg3.b.B0; /* B0 -> B1 */
//    tmp2.b.B0 = reg2.b.B0; /* B7 -> B0 */
//  
//    /* Calculate final ECC code */
//#ifdef NAND_ECC_SMC
//    ecc_code[0] = ~tmp2.d;
//    ecc_code[1] = ~tmp1.d;
//#else
//    ecc_code[0] = ~tmp1.d;
//    ecc_code[1] = ~tmp2.d;
//#endif
//    ecc_code[2] = ((~reg1.d) << 2) | 0x03;
//}
//
//#else
//
///*
//* nand_calculate_ecc - Calculate 3-byte ECC for 256-byte block
//*/
//void Nand_Calculate_ECC(const byte *dat, byte *ecc_code)
//{
//    int idx, reg1, reg2, reg3, tmp1, tmp2;
//    int i;
//  
//    /* Initialize variables */
//    reg1 = reg2 = reg3 = 0;
//  
//    /* Build up column parity */
//    for (i = 0; i < 256; i++)
//    {
//        /* Get CP0 - CP5 from table */
//        idx = nand_ecc_precalc_table[*dat++];
//        reg1 ^= (idx & 0x3f);
//  
//        /* All bit XOR = 1 ? */
//        if (idx & 0x40)
//        {
//            reg3 ^= i;
//            reg2 ^= ~i;
//        }
//    }
//  
//    /* Create non-inverted ECC code from line parity */
//    tmp1  = (reg3 & 0x80) >> 0; /* B7 -> B7 */
//    tmp1 |= (reg2 & 0x80) >> 1; /* B7 -> B6 */
//    tmp1 |= (reg3 & 0x40) >> 1; /* B6 -> B5 */
//    tmp1 |= (reg2 & 0x40) >> 2; /* B6 -> B4 */
//    tmp1 |= (reg3 & 0x20) >> 2; /* B5 -> B3 */
//    tmp1 |= (reg2 & 0x20) >> 3; /* B5 -> B2 */
//    tmp1 |= (reg3 & 0x10) >> 3; /* B4 -> B1 */
//    tmp1 |= (reg2 & 0x10) >> 4; /* B4 -> B0 */
//  
//    tmp2  = (reg3 & 0x08) << 4; /* B3 -> B7 */
//    tmp2 |= (reg2 & 0x08) << 3; /* B3 -> B6 */
//    tmp2 |= (reg3 & 0x04) << 3; /* B2 -> B5 */
//    tmp2 |= (reg2 & 0x04) << 2; /* B2 -> B4 */
//    tmp2 |= (reg3 & 0x02) << 2; /* B1 -> B3 */
//    tmp2 |= (reg2 & 0x02) << 1; /* B1 -> B2 */
//    tmp2 |= (reg3 & 0x01) << 1; /* B0 -> B1 */
//    tmp2 |= (reg2 & 0x01) << 0; /* B7 -> B0 */
//  
//    /* Calculate final ECC code */
//#ifdef NAND_ECC_SMC
//    ecc_code[0] = ~tmp2;
//    ecc_code[1] = ~tmp1;
//#else
//    ecc_code[0] = ~tmp1;
//    ecc_code[1] = ~tmp2;
//#endif
//    ecc_code[2] = ((~reg1) << 2) | 0x03;
//  
//}
//
//#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//#ifdef ECC_STRUCT
//
///*
//* nand_calculate_ecc - Calculate 3-byte ECC for 256-byte block
//*/
//void Nand_Calculate_ECC(const byte *dat, u16 len, byte *ecc_code)
//{
//    int idx;
//	RX reg1, reg2, reg3, tmp1, tmp2;
//  
//	while (len > 0)
//	{
//		/* Initialize variables */
//		reg1.d = reg2.d = reg3.d = 0;
//
//		u16 count = (len > 256) ? 256 : len;
//	  
//		/* Build up column parity */
//		for (u16 i = 0; i < count; i++)
//		{
//			/* Get CP0 - CP5 from table */
//			idx = nand_ecc_precalc_table[*dat++];
//			reg1.d ^= (idx & 0x3f);
//	  
//			/* All bit XOR = 1 ? */
//			if (idx & 0x40)
//			{
//				reg3.d ^= (byte) i;
//				reg2.d ^= ~((byte) i);
//			}
//		};
//	  
//		/* Create non-inverted ECC code from line parity */
//		tmp1.b.B7 = reg3.b.B7; /* B7 -> B7 */
//		tmp1.b.B6 = reg2.b.B7; /* B7 -> B6 */
//		tmp1.b.B5 = reg3.b.B6; /* B6 -> B5 */
//		tmp1.b.B4 = reg2.b.B6; /* B6 -> B4 */
//		tmp1.b.B3 = reg3.b.B5; /* B5 -> B3 */
//		tmp1.b.B2 = reg2.b.B5; /* B5 -> B2 */
//		tmp1.b.B1 = reg3.b.B4; /* B4 -> B1 */
//		tmp1.b.B0 = reg2.b.B4; /* B4 -> B0 */
//	  
//		tmp2.b.B7 = reg3.b.B3; /* B3 -> B7 */
//		tmp2.b.B6 = reg2.b.B3; /* B3 -> B6 */
//		tmp2.b.B5 = reg3.b.B2; /* B2 -> B5 */
//		tmp2.b.B4 = reg2.b.B2; /* B2 -> B4 */
//		tmp2.b.B3 = reg3.b.B1; /* B1 -> B3 */
//		tmp2.b.B2 = reg2.b.B1; /* B1 -> B2 */
//		tmp2.b.B1 = reg3.b.B0; /* B0 -> B1 */
//		tmp2.b.B0 = reg2.b.B0; /* B7 -> B0 */
//  
//		/* Calculate final ECC code */
//	#ifdef NAND_ECC_SMC
//		ecc_code[0] = ~tmp2.d;
//		ecc_code[1] = ~tmp1.d;
//	#else
//		ecc_code[0] = ~tmp1.d;
//		ecc_code[1] = ~tmp2.d;
//	#endif
//		ecc_code[2] = ((~reg1.d) << 2) | 0x03;
//
//		len -= count;
//		ecc_code += 3;
//	};
//}
//
//#else
//
///*
//* nand_calculate_ecc - Calculate 3-byte ECC for 256-byte block
//*/
//void Nand_Calculate_ECC(const byte *dat, u16 len, byte *ecc_code)
//{
//    byte idx, reg1, reg2, reg3, tmp1, tmp2;
//    int i;
//  
//	while (len > 0)
//	{
//		/* Initialize variables */
//		reg1 = reg2 = reg3 = 0;
//
//		u16 count = (len > 256) ? 256 : len;
//	  
//		/* Build up column parity */
//		for (i = 0; i < count; i++)
//		{
//			/* Get CP0 - CP5 from table */
//			idx = nand_ecc_precalc_table[*dat++];
//			reg1 ^= (idx & 0x3f);
//	  
//			/* All bit XOR = 1 ? */
//			if (idx & 0x40)
//			{
//				reg3 ^= (byte) i;
//				reg2 ^= ~((byte) i);
//			}
//		}
//	  
//		/* Create non-inverted ECC code from line parity */
//		tmp1  = (reg3 & 0x80) >> 0; /* B7 -> B7 */
//		tmp1 |= (reg2 & 0x80) >> 1; /* B7 -> B6 */
//		tmp1 |= (reg3 & 0x40) >> 1; /* B6 -> B5 */
//		tmp1 |= (reg2 & 0x40) >> 2; /* B6 -> B4 */
//		tmp1 |= (reg3 & 0x20) >> 2; /* B5 -> B3 */
//		tmp1 |= (reg2 & 0x20) >> 3; /* B5 -> B2 */
//		tmp1 |= (reg3 & 0x10) >> 3; /* B4 -> B1 */
//		tmp1 |= (reg2 & 0x10) >> 4; /* B4 -> B0 */
//	  
//		tmp2  = (reg3 & 0x08) << 4; /* B3 -> B7 */
//		tmp2 |= (reg2 & 0x08) << 3; /* B3 -> B6 */
//		tmp2 |= (reg3 & 0x04) << 3; /* B2 -> B5 */
//		tmp2 |= (reg2 & 0x04) << 2; /* B2 -> B4 */
//		tmp2 |= (reg3 & 0x02) << 2; /* B1 -> B3 */
//		tmp2 |= (reg2 & 0x02) << 1; /* B1 -> B2 */
//		tmp2 |= (reg3 & 0x01) << 1; /* B0 -> B1 */
//		tmp2 |= (reg2 & 0x01) << 0; /* B7 -> B0 */
//  
//		/* Calculate final ECC code */
//	#ifdef NAND_ECC_SMC
//		ecc_code[0] = ~tmp2;
//		ecc_code[1] = ~tmp1;
//	#else
//		ecc_code[0] = ~tmp1;
//		ecc_code[1] = ~tmp2;
//	#endif
//		ecc_code[2] = ((~reg1) << 2) | 0x03;
//
//		len -= count;
//		ecc_code += 3;
//	};
//}
//
//#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static inline int countbits(u32 b)
{
    int res = 0;
  
    for (; b; b >>= 1) res += b & 0x01;
    return res;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//#ifdef ECC_STRUCT
//
//bool Nand_ECC_Correct_Data(byte* dat, byte* read_ecc, byte* calc_ecc)
//{
//    UECC s;
//
//	s.d = 0;
//
//#ifdef NAND_ECC_SMC
//	s0.b8[0] = calc_ecc[0] ^ read_ecc[0];
//	s1.b8[1] = calc_ecc[1] ^ read_ecc[1];
//	s2.b8[2] = calc_ecc[2] ^ read_ecc[2];
//#else
//	s.b8[1] = calc_ecc[0] ^ read_ecc[0];
//	s.b8[0] = calc_ecc[1] ^ read_ecc[1];
//	s.b8[2] = calc_ecc[2] ^ read_ecc[2];
//#endif
//
//	if (s.d == 0) return true;
//
//	/* Check for a single bit error */
//	if (((s.d ^ (s.d >> 1)) & 0x545555) == 0x545555)
//	{
//		RX byteoffs, bitnum;
//
//		byteoffs.d = 0; bitnum.d = 0;
//
//		byteoffs.b.B7 = s.b.B15;	//(s1 << 0) & 0x80;
//		byteoffs.b.B6 = s.b.B13;	//(s1 << 1) & 0x40;
//		byteoffs.b.B5 = s.b.B11;	//(s1 << 2) & 0x20;
//		byteoffs.b.B4 = s.b.B9;		//(s1 << 3) & 0x10;
//
//		byteoffs.b.B3 = s.b.B7;	//(s0 >> 4) & 0x08;
//		byteoffs.b.B2 = s.b.B5;	//(s0 >> 3) & 0x04;
//		byteoffs.b.B1 = s.b.B3;	//(s0 >> 2) & 0x02;
//		byteoffs.b.B0 = s.b.B1;	//(s0 >> 1) & 0x01;
//
//		bitnum.b.B2 = s.b.B23;//(s2 >> 5) & 0x04;
//		bitnum.b.B1 = s.b.B21;//(s2 >> 4) & 0x02;
//		bitnum.b.B0 = s.b.B19;//(s2 >> 3) & 0x01;
//
//		dat[byteoffs.d] ^= (1 << bitnum.d);
//
//		return true;
//	};
//
//	return (countbits(s.d) == 1);
//}
//
//#else
//
//bool Nand_ECC_Correct_Data(byte* dat, byte* read_ecc, byte* calc_ecc)
//{
//    byte s0, s1, s2;
//
//#ifdef NAND_ECC_SMC
//	s0 = calc_ecc[0] ^ read_ecc[0];
//	s1 = calc_ecc[1] ^ read_ecc[1];
//	s2 = calc_ecc[2] ^ read_ecc[2];
//#else
//	s1 = calc_ecc[0] ^ read_ecc[0];
//	s0 = calc_ecc[1] ^ read_ecc[1];
//	s2 = calc_ecc[2] ^ read_ecc[2];
//#endif
//
//	if ((s0 | s1 | s2) == 0) return 0;
//
//	/* Check for a single bit error */
//	if (((s0 ^ (s0 >> 1)) & 0x55) == 0x55 &&
//		((s1 ^ (s1 >> 1)) & 0x55) == 0x55 &&
//		((s2 ^ (s2 >> 1)) & 0x54) == 0x54) {
//
//		u32 byteoffs, bitnum;
//
//		byteoffs = (s1 << 0) & 0x80;
//		byteoffs |= (s1 << 1) & 0x40;
//		byteoffs |= (s1 << 2) & 0x20;
//		byteoffs |= (s1 << 3) & 0x10;
//
//		byteoffs |= (s0 >> 4) & 0x08;
//		byteoffs |= (s0 >> 3) & 0x04;
//		byteoffs |= (s0 >> 2) & 0x02;
//		byteoffs |= (s0 >> 1) & 0x01;
//
//		bitnum = (s2 >> 5) & 0x04;
//		bitnum |= (s2 >> 4) & 0x02;
//		bitnum |= (s2 >> 3) & 0x01;
//
//		dat[byteoffs] ^= (1 << bitnum);
//
//		return true;
//	}
//
//	return (countbits(s0 | ((u32)s1 << 8) | ((u32)s2 << 16)) == 1);
//}
//
//#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Nand_ECC_Calc_V2(const byte *dat, u16 len, byte *ecc_code)
{
    int idx;
	int reg1, reg2, reg3;//, tmp1, tmp2;
  
	while (len > 0)
	{
		/* Initialize variables */
		reg1 = reg2 = reg3 = 0;

		u16 count = (len > 256) ? 256 : len;
	  
		/* Build up column parity */
		for (u16 i = 0; i < count; i++)
		{
			/* Get CP0 - CP5 from table */
			idx = nand_ecc_precalc_table[*dat++];
			reg1 ^= (idx & 0x3f);
	  
			/* All bit XOR = 1 ? */
			if (idx & 0x40) reg3 ^= i, reg2 ^= ~i;
		};
	  
		/* Calculate final ECC code */
		ecc_code[0] = ~reg3;
		ecc_code[1] = ~reg2;
		ecc_code[2] = ((~reg1) << 2) | 0x03;

		len -= count;
		ecc_code += 3;
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Nand_ECC_Corr_V2(byte* dat, u16 len, const byte* read_ecc, u32 *pErrCount, u32 *pCorrErrCount, u32 *pParityErrCount)
{
    int idx;
	int reg1, reg2, reg3;
		
	UECC s;//, tmp;

	u32 corrErr = 0, err = 0, parErr = 0;

	while (len > 0)
	{
		/* Initialize variables */
		reg1 = reg2 = reg3 = 0;
		//tmp.d = 0;

		byte *src = dat;

		u16 count = (len > 256) ? 256 : len;
	  
		/* Build up column parity */
		for (u16 i = 0; i < count; i++)
		{
			/* Get CP0 - CP5 from table */
			idx = nand_ecc_precalc_table[*dat++];
			reg1 ^= (idx & 0x3f);
	  
			/* All bit XOR = 1 ? */
			if (idx & 0x40) reg3 ^= i, reg2 ^= ~i;
		};
	  
		s.d = 0;

		//tmp.b8[0] = ~reg3;
		//tmp.b8[1] = ~reg2;
		//tmp.b8[2] = ((~reg1) << 2) | 0x03;


		s.b8[0] = (~reg3) 					^ read_ecc[0];
		s.b8[1] = (~reg2) 					^ read_ecc[1];
		s.b8[2] = (((~reg1) << 2) | 0x03)	^ read_ecc[2];

		read_ecc += 3;

		//s.d ^= tmp.d;

		if (s.d == 0)
		{
				
		}
		else if ((byte)(s.b8[0] ^ s.b8[1]) == 0xFF && ((s.b8[2] ^ (s.b8[2] >> 1)) & 0x54) == 0x54)
		{
			RX bitnum;

			bitnum.d = 0;

			bitnum.b.B2 = s.b.B23;//(s2 >> 5) & 0x04;
			bitnum.b.B1 = s.b.B21;//(s2 >> 4) & 0x02;
			bitnum.b.B0 = s.b.B19;//(s2 >> 3) & 0x01;

			src[s.b8[0]] ^= (1 << bitnum.d);

			corrErr++;
		}
		else if (countbits(s.d) == 1)
		{
			parErr++;	
		}
		else
		{
			err++;
		};

		len -= count;
	};

	if (pErrCount		!= 0) *pErrCount		= err;
	if (pCorrErrCount	!= 0) *pCorrErrCount	= corrErr;
	if (pParityErrCount != 0) *pParityErrCount	= parErr;

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
