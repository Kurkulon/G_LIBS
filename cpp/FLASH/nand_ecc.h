#ifndef NAND_ECC_H__01_04_2024__12_36
#define NAND_ECC_H__01_04_2024__12_36

#include "types.h"

//extern void Nand_Calculate_ECC(const byte *dat, byte *ecc_code);
//extern int Nand_ECC_Correct_Data(byte* dat, byte* read_ecc, byte* calc_ecc);
//extern void Nand_Calculate_ECC(const byte *dat, u16 len, byte *ecc_code);

extern void Nand_ECC_Calc_V2(const byte *dat, u16 len, byte *ecc_code);
extern void Nand_ECC_Corr_V2(byte* dat, u16 len, const byte* read_ecc, u32 *pErrCount, u32 *pCorrErrCount, u32 *pParityErrCount);

#endif //NAND_ECC_H__01_04_2024__12_36
