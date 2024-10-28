#ifndef CORE_H__11_02_2024__23_07
#define CORE_H__11_02_2024__23_07

#pragma once

#include "types.h"

#ifdef CPU_SAME53

	#define	CORETYPE_SAME53
	#define CORTEX_M4

	#ifndef __TARGET_ARCH_ARM
	#define __TARGET_ARCH_ARM		0
	#endif

	#ifndef __TARGET_ARCH_THUMB
	#define __TARGET_ARCH_THUMB		4
	#endif

	#ifndef __CC_ARM
	#define __CC_ARM		
	#endif

	#include "ARM\same53.h"

#elif defined(CPU_XMC48)

	#define	CORETYPE_XMC4800
	#define CORTEX_M4

	#ifndef __TARGET_ARCH_ARM
	#define __TARGET_ARCH_ARM		0
	#endif

	#ifndef __TARGET_ARCH_THUMB
	#define __TARGET_ARCH_THUMB		4
	#endif

	#ifndef __CC_ARM
	#define __CC_ARM		
	#endif

	#include "ARM\XMC4800.h"

#elif defined(CPU_LPC824)

	#define	CORETYPE_LPC8XX
	#define	CORETYPE_LPC82X
	#define CORTEX_M0

	#ifndef __CC_ARM
	#define __CC_ARM		
	#endif

	#include "ARM\lpc82x.h"

#elif defined(CPU_LPC812)

	#define	CORETYPE_LPC8XX
	#define	CORETYPE_LPC81X
	#define CORTEX_M0

	#ifndef __CC_ARM
	#define __CC_ARM		
	#endif

	#include "ARM\lpc81x.h"

#elif defined(CPU_BF592) || defined(CPU_BF706)

	#ifndef _ADI_COMPILER
	#define _ADI_COMPILER
	#endif

	#ifndef __ADSPBLACKFIN__
	#define __ADSPBLACKFIN__
	#endif

	#ifndef __cplusplus
	#define __cplusplus
	#endif

	#ifndef _LANGUAGE_C
	#define _LANGUAGE_C
	#endif

	#ifndef __NUM_CORES__
	#define __NUM_CORES__ 1
	#endif

	#ifdef CPU_BF592

		#define	CORETYPE_BF592
		#define ADSP_BLACKFIN

		#ifndef __ADSPBF59x__
		#define __ADSPBF59x__
		#endif

		#ifndef __ADSPLPBLACKFIN__
		#define __ADSPLPBLACKFIN__ 1
		#endif


		#ifndef __SILICON_REVISION__
		#define __SILICON_REVISION__ 0x0002
		#endif

		#include "ADSP\bf592.h"

	#elif defined(CPU_BF706)

		#define	CORETYPE_BF706
		#define ADSP_BLACKFIN

		#ifndef __ADSPBF706__
		#define __ADSPBF706__
		#endif

		#ifndef __ADSPBF70x__
		#define __ADSPBF70x__
		#endif

		#ifndef __ADSPBF7xx__
		#define __ADSPBF7xx__
		#endif

		#ifndef __ADSPLPBLACKFIN__
		#define __ADSPLPBLACKFIN__ 0x220
		#endif

		#ifndef __SILICON_REVISION__
		#define __SILICON_REVISION__ 0x101
		#endif

		#include "ADSP\bf706.h"

	#endif

#elif defined(WIN32)

	namespace HW
	{
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		inline bool RamCheck(const void *ptr) { return true; }

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	} // namespace HW

#endif

#endif // CORE_H__11_02_2024__23_07
