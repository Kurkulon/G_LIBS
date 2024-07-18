#ifndef CORE_H__11_02_2024__23_07
#define CORE_H__11_02_2024__23_07

#pragma once

#ifdef CPU_SAME53

#define	CORETYPE_SAME53
#define CORTEX_M4

#ifndef __TARGET_ARCH_ARM
#define __TARGET_ARCH_ARM		0
#endif

#ifndef __TARGET_ARCH_THUMB
#define __TARGET_ARCH_THUMB		4
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

#include "ARM\XMC4800.h"

#elif defined(CPU_LPC824)

#define	CORETYPE_LPC8XX
#define	CORETYPE_LPC82X
#define CORTEX_M0

#include "ARM\lpc82x.h"

#elif defined(CPU_LPC812)

#define	CORETYPE_LPC8XX
#define	CORETYPE_LPC81X
#define CORTEX_M0

#include "ARM\lpc81x.h"

#elif defined(CPU_BF592)

#define	CORETYPE_BF592
#define ADSP_BLACKFIN

#ifndef __ADSPBF59x__
#define __ADSPBF59x__
#endif

#include "ADSP\bf592.h"

#elif defined(CPU_BF706)

#define	CORETYPE_BF706
#define ADSP_BLACKFIN

#ifndef __ADSPBF70x__
#define __ADSPBF70x__
#endif

#include "ADSP\bf706.h"

#endif

#endif // CORE_H__11_02_2024__23_07
