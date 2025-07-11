#ifndef TYPES_H__11_02_2024__23_04
#define TYPES_H__11_02_2024__23_04

#pragma once

#if defined(_DEBUG) && !defined(__DEBUG)
#define __DEBUG
#endif

#if defined(__DEBUG) && !defined(_DEBUG)
#define _DEBUG
#endif

#ifdef _ADI_COMPILER

	#include <ccblkfn.h>

	#ifndef _MSC_VER

		#ifdef __DEBUG
			#define __breakpoint(v) asm("EMUEXCPT;")
		#else
			#define __breakpoint(v)
		#endif

		#define __forceinline inline
		#define __nop __builtin_NOP
		#define __packed /*__attribute__((packed))*/

		#define __disable_irq cli

	#endif

#endif // _ADI_COMPILER

#ifdef _MSC_VER

	#define WINDOWS_IGNORE_PACKING_MISMATCH

	#include <intrin.h>

	#define __writeonly /**/
	#define __packed __declspec(align(1))
	#define __softfp /**/
	#define __irq __declspec(naked)
	#define __align(v) __declspec(align(v))
	#define __attribute__(v)
	#define __func__ __FUNCTION__
	#define restrict /**/
	#define section(v) static const char _section = v;

	__forceinline void __breakpoint(int v) { __debugbreak(); }
	__forceinline void __disable_irq() {}
	__forceinline void __enable_irq() {}
	//inline void __nop() {}

	//#define __CC_ARM
	//#define __TARGET_FPU_VFP

	#if _MSC_VER > 1500
		#pragma comment(lib, "legacy_stdio_definitions.lib")
	#endif

#endif 

#if defined(_DEBUG) || defined(__DEBUG)
#define DEBUG_ASSERT(v) if (!(v)) __breakpoint(0)
#else
#define DEBUG_ASSERT(v)
#endif

typedef unsigned char byte, u8;
typedef unsigned short int word, u16;
typedef unsigned long int dword, u32;
typedef signed char i8;
typedef signed short int i16;
typedef signed long int i32;
typedef signed long long int i64;
typedef unsigned long long int u64;

#define ArraySize(x) (sizeof(x)/sizeof(x[0]))

#if !defined(_MSC_VER) && !defined(NAN)
static const dword __NAN_dword = 0xFFFFFFFF;
#define NAN (*((const float*)(&__NAN_dword)))
#endif



//inline bool fIsValid(float v) { return (((u16*)&v)[1] & 0x7F80) != 0x7F80; }
//inline bool dIsValid(double v) { return (((u32*)&v)[1] & 0x7FF0) != 0x7FF0; }

//__forceinline u32 LIM(u32 v, u32 min, u32 max) { return (v < min) ? min : ((v > max) ? max : v); }
//__forceinline u32 MIN(u32 a, u32 b) { return (a < b) ? a : b; }
//__forceinline u32 MAX(u32 a, u32 b) { return (a >= b) ? a : b; }

#define LIM(v, min, max)	(((v) < (min)) ? (min) : (((v) > (max)) ? (max) : (v)))
#define MIN(a, b)			(((a) < (b)) ? (a) : (b))
#define MAX(a, b)			(((a) >= (b)) ? (a) : (b))

#define GD(adr, t, i) (*(((t*)adr)+i))
#define GB(adr,i) (*(((byte*)adr)+i))

__forceinline void COPY(const void *src, void *dst, u32 size) { const byte *s = (const byte*)src; byte *d = (byte*)dst;  while(size--) *d++ = *s++; }
__forceinline void delay(u32 cycles) { while(cycles--) __nop();}

#if defined(CPU_BF592) || defined(__ADSPBLACKFIN__)
inline i32	ABS(i32 v)			{ return __builtin_abs(v); }
inline i32	Max32(i32 a, i32 b)	{ return __builtin_max(a,b); }
inline i32	Min32(i32 a, i32 b)	{ return __builtin_min(a,b); }
#else
__forceinline i32	ABS(i32 v)			{ return (v<0) ? -v : v; }
__forceinline i32	Max32(i32 a, i32 b)	{ return MAX(a,b); }
__forceinline i32	Min32(i32 a, i32 b)	{ return MIN(a,b); }
#endif

__forceinline float	ABS(float v)	{ *((u32*)&v) &= 0x7FFFFFFF; return v; }

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

__forceinline u32 Push_IRQ()
{
#ifdef _ADI_COMPILER

	return __builtin_cli();

#elif  defined(__CC_ARM)

	register u32 t;

	register u32 primask __asm("primask");

	t = primask;

	__disable_irq();

	return t;

#else 

	return 0;

#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

__forceinline void Pop_IRQ(u32 t)
{
#ifdef _ADI_COMPILER

	__builtin_sti(t);

#elif  defined(__CC_ARM)

	register u32 primask __asm("primask");

	primask = t;

#endif
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef _ADI_COMPILER

__forceinline void Read32(u32 v) {  }
//__forceinline word ReverseWord(word v) { return ((v&0x00FF)<<8 | (v&0xFF00)>>8); }
//__forceinline dword ReverseDword(dword v) { v = (v&0x00FF00FF)<<8 | (v&0xFF00FF00)>>8;	return (v&0x0000FFFF)<<16 | (v&0xFFFF0000)>>16; }
__forceinline word ReverseWord(word v) { return __builtin_byteswap2(v); }
__forceinline dword ReverseDword(dword v) { return __builtin_byteswap4(v); }
__forceinline dword SwapDword(dword v) { return (v&0x00FF00FF)<<8 | (v&0xFF00FF00)>>8; }

__forceinline i32 _InterlockedIncrement(volatile i32 *v) { u32 t = __builtin_cli(); i32 r = *v += 1; __builtin_sti(t); return r; }
__forceinline i32 _InterlockedDecrement(volatile i32 *v) { u32 t = __builtin_cli(); i32 r = *v -= 1; __builtin_sti(t); return r; }

#elif  defined(__CC_ARM)

__forceinline u32 ReadMem32(u32 ptr) { return ptr; }
__forceinline void Read32(u32 v) { u32 t; __asm { add t, v }; }
__forceinline u16 ReverseWord(u16 v) { __asm	{ rev16 v, v };	return v; }
__forceinline u32 ReverseDword(u32 v) { return __rev(v); }
__forceinline dword SwapDword(dword v) { __asm { rev16 v, v }; return v; }

__forceinline i32 _InterlockedIncrement(volatile i32 *v) { u32 t = Push_IRQ(); i32 r = *v += 1; Pop_IRQ(t); return r; }
__forceinline i32 _InterlockedDecrement(volatile i32 *v) { u32 t = Push_IRQ(); i32 r = *v -= 1; Pop_IRQ(t); return r; }

#elif defined(_MSC_VER)

__forceinline void Read32(u32 v) {  }
__forceinline word ReverseWord(word v) { return ((v&0x00FF)<<8 | (v&0xFF00)>>8); }
__forceinline dword ReverseDword(dword v) { v = (v&0x00FF00FF)<<8 | (v&0xFF00FF00)>>8;	return (v&0x0000FFFF)<<16 | (v&0xFFFF0000)>>16; }
__forceinline dword SwapDword(dword v) { return (v&0x00FF00FF)<<8 | (v&0xFF00FF00)>>8; }

#endif


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

union DataCRC
{
	word	w;
	byte	b[2];
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

union U16u 
{
	word w; byte b[2]; 

	U16u(word v) {w = v;}
	operator word() {return w;}
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

union U32u 
{ 
	dword d; word w[2]; byte b[4]; 

	U32u(dword v) {d = v;}
	U32u() {d = 0;}
	operator dword() {return d;}
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

union F32u
{ 
	float f; dword d; word w[2]; byte b[4]; 

	F32u (float v) {f = v;}
	operator float() {return f;}
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

union DataPointer
{
	__packed void	*v;
	__packed byte	*b;
	__packed word	*w;
	__packed dword	*d;
	__packed float	*f;

	DataPointer(void *p) { v = p; } 

	void operator=(void *p) { v = p; } 

#ifdef _ADI_COMPILER
	void WW(word a) { misaligned_store16(v, a); }
	word RW() { return misaligned_load16(v); }
#endif

};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

union ConstDataPointer
{
	__packed const void		*v;
	__packed const byte		*b;
	__packed const word		*w;
	__packed const dword	*d;
	__packed const float	*f;

	ConstDataPointer(const void *p) { v = p; } 

	void operator=(const void *p) { v = p; } 
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define __CONCAT1(s1)						s1
#define __CONCAT2(s1,s2)					s1##s2
#define __CONCAT3(s1,s2,s3)					s1##s2##s3
#define __CONCAT4(s1,s2,s3,s4)				s1##s2##s3##s4
#define __CONCAT5(s1,s2,s3,s4,s5)			s1##s2##s3##s4##s5
#define __CONCAT6(s1,s2,s3,s4,s5,s6)		s1##s2##s3##s4##s5##s6
#define __CONCAT7(s1,s2,s3,s4,s5,s6,s7)		s1##s2##s3##s4##s5##s6##s7
#define __CONCAT8(s1,s2,s3,s4,s5,s6,s7,s8)	s1##s2##s3##s4##s5##s6##s7##s8


#define CONCAT1(s1)							__CONCAT1(s1)
#define CONCAT2(s1,s2)						__CONCAT2(s1,s2)
#define CONCAT3(s1,s2,s3)					__CONCAT3(s1,s2,s3)
#define CONCAT4(s1,s2,s3,s4)				__CONCAT4(s1,s2,s3,s4)
#define CONCAT5(s1,s2,s3,s4,s5)				__CONCAT4(s1,s2,s3,s4,s5)
#define CONCAT6(s1,s2,s3,s4,s5,s6)			__CONCAT6(s1,s2,s3,s4,s5,s6)		
#define CONCAT7(s1,s2,s3,s4,s5,s6,s7)		__CONCAT7(s1,s2,s3,s4,s5,s6,s7)		
#define CONCAT8(s1,s2,s3,s4,s5,s6,s7,s8)	__CONCAT8(s1,s2,s3,s4,s5,s6,s7,s8)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#endif // TYPES_H__11_02_2024__23_04
