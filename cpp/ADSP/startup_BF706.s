/*
** ADSP-BF706 startup code generated on Jul 18, 2024 at 22:32:50.
*/
/*
** Copyright (C) 2000-2022 Analog Devices Inc., All Rights Reserved.
**
** This file is generated automatically based upon the options selected
** in the System Configuration utility. Changes to the Startup Code configuration
** should be made by modifying the appropriate options rather than editing
** this file. To access the System Configuration utility, double-click the
** system.svc file from a navigation view.
**
** Custom additions can be inserted within the user-modifiable sections. These
** sections are bounded by comments that start with "$VDSG". Only changes
** placed within these sections are preserved when this file is re-generated.
**
** Product      : CrossCore Embedded Studio
** Tool Version : 6.2.3.4
*/

#include <sys/platform.h>
#include <sys/anomaly_macros_rtl.h>
//#include <adi_osal.h>
//#include <sys/fatal_error_code.h>
//#include <sys/exception.h>

//#define LEGACY_SUPERVISOR_MODE
//#define TEST_DCPLB
//#define TEST_ICPLB

#define DISPATCHER_REG_PUSH \
		[--SP] = P0;  \
		[--SP] = P1;  \
		[--SP] = P2;  \
		[--SP] = R0;  \
		[--SP] = R1;  \
		[--SP] = R2;  \
		[--SP] = RETS;\
		[--SP] = ASTAT

/* Standard dispatcher register restore sequence,
** must be identical across all dispatchers
*/
#define DISPATCHER_REG_POP \
		ASTAT = [SP++];\
		RETS  = [SP++];\
		R2    = [SP++];\
		R1    = [SP++];\
		R0    = [SP++];\
		P2    = [SP++];\
		P1    = [SP++];\
		P0    = [SP++] 

#define LOADIMM32REG(R,VAL) R = VAL;

#ifdef LEGACY_SUPERVISOR_MODE
	#define INTERRUPT_BITS (BITM_IMASK_IVG11 | BITM_IMASK_IVG15)	/* Mask of interrupt bits to be enabled by default. Bits 0-4 unmaskable. */
#else
	#define INTERRUPT_BITS (BITM_IMASK_IVG11)						/* Mask of interrupt bits to be enabled by default. Bits 0-4 unmaskable. */
#endif

#define UNASSIGNED_VAL 0x81818181

#define SEC_VECTABLE_LEN PARAM_SEC0_SCOUNT

.SECTION/NO_INIT SEC_VecTable;

	.BYTE4	_SEC_VecTable[SEC_VECTABLE_LEN];
	.GLOBAL _SEC_VecTable;
	.TYPE	_SEC_VecTable,STT_OBJECT;

#if WA_19000054
	.BYTE4	_isr_nesting_count;
	.BYTE4	_prev_bp_enable;
#endif // #if WA_19000054

// The requiredForROMBoot file attribute is included here as this source
// defines code and data that are used before, or as part of, the meminit
// memory initialization support.
.FILE_ATTR requiredForROMBoot;

.SECTION/DOUBLEANY L1_code;
.ALIGN 2;


start:
      // The assembler warning for anomaly 05-00-0312, issued when interrupts
      // are enabled and an an SSYNC, CSYNC or loop register load is used, is
      // not needed till interrupts are enabled. So we suppress it.
      .MESSAGE/SUPPRESS 5515;



      // Disable the Branch Predictor and clear its memory so that it is not
      // making predictions based on out-of-date address/opcode info. This is
      // as recommended for anomaly 19-00-0047. Clearing the BP's memory takes
      // approximately 150 CCLKs, and we need to have the BP disabled during
      // that time.

      CSYNC;                           // Flush pipeline.

      // Configure the Branch Predictor and start operation to flush its
      // internal operation. This must be done while SYSCFG:BPEN is unmasked.

      R0 = BITM_BP_CFG_CLRBP |         // Clear any learned predictions
           (1<<15) |                   // SKUPDLRU (Skip Updates LRU)
           BITM_BP_CFG_JUMPCCEN |      // JUMPCCEN (JUMP Condition Code Enable)
                                       // Note: required for anomaly 19000034.
           BITM_BP_CFG_JUMPEN |        // JUMPEN (JUMP Enable)
#if !WA_19000044
           BITM_BP_CFG_RTSEN |         // RTSEN (RTS Enable)
#endif
           (1<<20) |                   // CALL16EN (Call 16-Bit Enable)
           BITM_BP_CFG_CALL32EN |      // CALL32EN (Call 32-Bit Enable)
           BITM_BP_CFG_CALL64EN |      // CALL64EN (Call 64-Bit Enable)
           (2<<BITP_BP_CFG_STMOUTVAL); // 2 STMOUTVAL (Store Timeout Value)

		[REG_BP_CFG] = R0;

		R0 = SYSCFG;
		BITCLR(R0, BITP_SYSCFG_BPEN);
		SYSCFG = R0;                     // Disable the BP

		// Clearing now in progress. We will have to wait until later to enable
		// the predictor.

		// Disable CPLBs as they might be enable by initialization code
		// or still be enabled after a software reset.

		P1 = REG_L1IM_ICTL;
		R2 = [P1];
		BITCLR(R2,BITP_L1IM_ICTL_ENCPLB);
		SSYNC;                  // It is best practice to issue an
								// SSYNC before changing a memory mode.
		[P1] = R2;
		SSYNC;

		P1 = REG_L1DM_DCTL;
		R2 = [P1];
		BITCLR(R2,BITP_L1DM_DCTL_ENCPLB);
		SSYNC;                  // It is best practice to issue an
								// SSYNC before changing a memory mode.
		[P1] = R2;
		SSYNC; 

		// Set RCU0_SVECT0 to allow a self-initiated core only reset to bypass
		// the boot code and vector straight to the beginning of L1 memory.
		R0 = start;
		[REG_RCU0_SVECT0] = R0;

		// Set registers to unassigned value.
		R0 = UNASSIGNED_VAL;

		// Initialize the stack.
		// Note: this points just past the end of the stack memory.
		// So the first write must be with [--SP].
		.EXTERN ldf_stack_end;
		.TYPE ldf_stack_end,STT_OBJECT;

#ifdef LEGACY_SUPERVISOR_MODE

		SP	= ldf_stack_end;
		USP	= SP;

#else

		.EXTERN ldf_sysstack_end;
		.TYPE ldf_sysstack_end,STT_OBJECT;

		SP	= ldf_sysstack_end;
		USP	= ldf_stack_end;

#endif

		// Push UNASSIGNED_VAL as RETS and old FP onto the stack to terminate
		// the call stack.
		[--SP] = R0;
		[--SP] = R0;

		// Initialize FP to point to the UNASSIGNED_VAL old FP value.
		FP = SP;

		// And make space for incoming "parameters" for functions
		// we call from here.
		//SP += -12;

		// Initialize loop counters to zero, to make sure that
		// hardware loops are disabled (it can be really baffling
		// if the counters and bottom regs are set, and we happen
		// to run into them).
		R7 = 0;
		LC0 = R7;
		LC1 = R7;

		// Clear the DAG Length regs so that it's safe to use I-regs
		// without them wrapping around as required by the Blackfin C ABI.
		L0 = R7;
		L1 = R7;
		L2 = R7;
		L3 = R7;

		// Initialize the Event Vector Table (EVT) entries other than
		// EVT0 (Emulation) and EVT1 (Reset).
		P0		= EVT2;
		R1		= dummy_NMI;
		[P0++]	= R1;
		R1		= dummy_EVX;
		[P0++]	= R1;
		R1		= dummy_IVG;
		[P0++]	= R1;
		R1		= dummy_IVHW;
		[P0++]	= R1;
		R1		= dummy_IVTMR;
		[P0++]	= R1;
		R1		= dummy_IVG;

		P1 = 9;
		LSETUP (.ivt, .ivt) LC0 = P1;

.ivt:	[P0++] = R1;

#ifdef LEGACY_SUPERVISOR_MODE

		// Set IVG15's handler to be the start of the mode-change
		// code. Then, before we return from the Reset back to user
		// mode, we'll raise IVG15. This will mean we stay in supervisor
		// mode, and continue from the mode-change point at the
		// lowest priority.

		P0   = EVT15;
		P1   = supervisor_mode;
		[P0] = P1;

#endif // #ifdef LEGACY_SUPERVISOR_MODE

		// Configure SYSCFG.
		R1 = SYSCFG;

		R0 = (	BITM_SYSCFG_CCEN |		// Enable the cycle counter.
				BITM_SYSCFG_SACC |		// Enable Supervisor Access
				BITM_SYSCFG_SNEN |      // Enable self-nesting interrupts.
				BITM_SYSCFG_BPEN |      // Enable branch prediction.
				BITM_SYSCFG_MPWEN );    // Enable MMR posted writes.

		R1 = R0 | R1;

		SYSCFG = R1;

		R0 = INTERRUPT_BITS (Z);

		STI R0;

#ifdef LEGACY_SUPERVISOR_MODE

		RAISE 15;             // handled by supervisor_mode

		P0		= still_interrupt_in_ipend;
		RETI	= P0;

still_interrupt_in_ipend:

		// Execute RTI instructions until we've `finished` servicing
		// all interrupts of priority higher than IVG15. Normally one
		// would expect to only have the reset interrupt in IPEND
		// being serviced, but occasionally when debugging this may
		// not be the case - if restart is hit when servicing an
		// interrupt.
		//
		// When we clear all bits from IPEND, we'll enter user mode,
		// then we'll automatically jump to supervisor_mode to start
		// servicing IVG15 (which we will 'service' for the whole
		// program, so that the program is in supervisor mode.
		//
		// Need to do this to 'finish' servicing the reset interrupt.

		.MESSAGE/SUPPRESS 1056 FOR 1 LINES;  // Suppress stall information message

		RTI;
#else

		P0		= supervisor_mode;
		RETI	= P0;
		RTI;

#endif

supervisor_mode:

#ifdef LEGACY_SUPERVISOR_MODE

		[--SP] = RETI;        // re-enables the interrupt system
#else

		R0		= UNASSIGNED_VAL;
		[--SP]	= R0;
		[--SP]	= R0;

		FP = SP;

#endif

		SP += -8;

		// Load the data value into R0.
		R0 = BITM_L1DM_DCPLB_DFLT_L1UREAD | BITM_L1DM_DCPLB_DFLT_L1UWRITE | BITM_L1DM_DCPLB_DFLT_L1SWRITE | BITM_L1DM_DCPLB_DFLT_L1EOM;

		// Load the instruction value into R1.
		R1 = BITM_L1IM_ICPLB_DFLT_L1UREAD | BITM_L1IM_ICPLB_DFLT_L1EOM;

		// Write the values to the default CPLB registers.
		[REG_L1DM_DCPLB_DFLT] = R0;
		[REG_L1IM_ICPLB_DFLT] = R1;
		CSYNC;

		P0 = _SEC_VecTable;
		R1 = dummy_sec_vector;
		P1 = SEC_VECTABLE_LEN;
		LSETUP (.secvt, .secvt) LC0 = P1;

		.secvt:	[P0++] = R1;

#if WA_19000054
		R1 = 0;
		[_isr_nesting_count]	= R1;
		[_prev_bp_enable]		= R1;
#endif // #if WA_19000054

		R0 = __sec_int_dispatcher;
		[EVT11] = R0;
		R0 = [IMASK];
		BITSET(R0, 11); 
		[IMASK] = R0;

		.EXTERN _SystemInit;
		.TYPE _SystemInit,STT_FUNC;
		CALL.X _SystemInit;

#ifdef TEST_DCPLB

		P0 = 0x04000000;
		R0 = [P0];

		P0 = 0x04080000-4;
		R0 = [P0];

		P0 = 0x08000000;
		R0 = [P0];

		P0 = 0x08100000-4;
		R0 = [P0];

		P0 = 0x11800000;
		R0 = [P0];

		P0 = 0x11808000-4;
		R0 = [P0];

		P0 = 0x11900000;
		R0 = [P0];

		P0 = 0x11908000-4;
		R0 = [P0];

		P0 = 0x11B00000;
		R0 = [P0];

		P0 = 0x11B02000-4;
		R0 = [P0];

		P0 = 0x1FC00000;
		R0 = [P0];

		P0 = 0x20000000;
		R0 = [P0];

		P0 = 0x38000000;
		R0 = [P0];

		P0 = 0x38000400-4;
		R0 = [P0];

		P0 = 0x40000000;
		R0 = [P0];

		P0 = 0x48000000-4;
		R0 = [P0];

#endif

		// Call constructors for C++ global scope variables.
		.EXTERN ___ctorloop;
		.TYPE ___ctorloop,STT_FUNC
		CALL.X ___ctorloop;

		// Call the application program.
		.EXTERN _main;
		.TYPE _main,STT_FUNC;
		CALL.X _main;

mail_exit_loop:

		EMUEXCPT;
		IDLE;
		JUMP mail_exit_loop; 

__sec_int_dispatcher:

		DISPATCHER_REG_PUSH; 

#if WA_19000054

		R0 = [_isr_nesting_count];
		CC = R0;
		R0 += 1;
		[_isr_nesting_count] = R0;
		// Only save the BPEN bit for the outer ISR.
		IF CC JUMP .wa_19000054_already_nested;
		// Save the BPEN bit.
		R0 = SYSCFG;
		R1 = BITM_SYSCFG_BPEN;
		R1 = R0 & R1;
		[_adi_prev_bp_enable] = R1;
		// Make sure the pipeline is clear before disabling the enable bit.
		BITCLR(R0, BITP_SYSCFG_BPEN);
		CSYNC;
		SYSCFG = R0;
		.wa_19000054_already_nested:

#endif /* WA_19000054 */

		P0 = [CEC_SID];					// read CEC_SID (32-bit MMR)
		//R0 = P0;						// copy SID to 1st handler arg
		[CEC_SID] = P0;					// interrupt acknowledgement
		//[--SP] = RETI;
		[--SP] = P0;

		P1 = _SEC_VecTable;

		P1 = P1 + (P0 << 2);			// &_SEC_VecTable[index]

		P1 = [P1];

		SP += -12;

		CALL (P1);						// call the handler, preserves R0

		SP += 12;
		
		R0 = [SP++];
		[REG_SEC0_END] = R0;			// R0 still contains the SID after the handler call

		//RETI = [SP++];

#if WA_19000054

		R0 = [_adi_isr_nesting_count];
		R0 += -1;
		CC = R0;
		[_adi_isr_nesting_count] = R0;
		IF CC JUMP .wa_19000054_still_nested;
		R1 = [_adi_prev_bp_enable];
		R0 = SYSCFG;
		R0 = R0 | R1;
		SYSCFG = R0;
		.wa_19000054_still_nested:

#endif /* WA_19000054 */ 

		DISPATCHER_REG_POP;

		RTI;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

dummy_sec_vector:

		EMUEXCPT;
		RTS;
		JUMP dummy_sec_vector; 
		
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

dummy_NMI:

		EMUEXCPT;
		RTN;
		JUMP dummy_NMI; 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

dummy_EVX:

		EMUEXCPT;
		RTX;
		JUMP dummy_EVX; 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

dummy_IVHW:

	EMUEXCPT;
	RTI;
	JUMP dummy_IVHW; 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

dummy_IVTMR:

	EMUEXCPT;
	RTI;
	JUMP dummy_IVTMR; 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

dummy_IVG:

	EMUEXCPT;
	RTI;
	JUMP dummy_IVG; 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//.main_end_loop:
//	  JUMP 	.main_end_loop;

.start.end: // End label required by the linker to find the size of the start
            // as necessary for absolute placement.
      .GLOBAL start;
      .type start,STT_FUNC;

      // If File IO support isn't provided, then
      // we provide dummy versions of the device-handling
      // functions, so that the exception handlers don't rely
      // on the file IO library.
      .SECTION/DOUBLEANY program;
      .ALIGN 2;
_dev_open:
_dev_close:
_dev_write:
_dev_read:
_dev_seek:
_dev_dup:
#if WA_05000371
      // Avoid anomaly 05-00-0371 by ensuring 4 instructions
      // before an RTS.
      NOP;
      NOP;
      NOP;
#endif
      R0 = -1;
      RTS;
._dev_open.end:
._dev_close.end:
._dev_write.end:
._dev_read.end:
._dev_seek.end:
._dev_dup.end:

      .GLOBAL _dev_open;
      .TYPE _dev_open,STT_FUNC;
      .GLOBAL _dev_close;
      .TYPE _dev_close,STT_FUNC;
      .GLOBAL _dev_write;
      .TYPE _dev_write,STT_FUNC;
      .GLOBAL _dev_read;
      .TYPE _dev_read,STT_FUNC;
      .GLOBAL _dev_seek;
      .TYPE _dev_seek,STT_FUNC;
      .GLOBAL _dev_dup;
      .TYPE _dev_dup,STT_FUNC;

      // This is a NULL terminated table of functions pointers for C++
      // constructor calls before main(). The LDF should contiguously map this
      // ctor.SECTION first, then others and then ctorl. The ctorl.SECTION
      // contains the NULL terminator.
      // We need to define some data at the start of the.SECTION here so that
      // the start label is placed correctly by the linker. The data could be
      // anything as it is unused. We set it to the address of the ctor end
      // .SECTION label to force that section end label to always be required.
      // This avoids unknowingly not having the ctorl defined NULL terminator
      // linked and the various unexpected results that could cause.
.SECTION/DOUBLEANY ctor;
      .ALIGN 4;
      .GLOBAL ___ctor_table;
      .type ___ctor_table,STT_OBJECT;
      .EXTERN ___ctor_end;
      .type ___ctor_end,STT_OBJECT;
      .BYTE4 ___ctor_table = ___ctor_end;

      // The .gdt sections (global dispatch table) are used by the C++ exception
      // runtime library support to determine which area of code to which a
      // particular address belongs. These sections must be mapped contiguously
      // into memory by the LDF starting with this one and followed by .gdtl.
.SECTION/DOUBLEANY .gdt;
      .ALIGN 4;
      .GLOBAL ___eh_gdt;
      .TYPE ___eh_gdt,STT_OBJECT;
      .EXTERN ___eh_gdt_end;
      .type ___eh_gdt_end,STT_OBJECT;
      .BYTE4 ___eh_gdt = ___eh_gdt_end;

.section ctorl;
.align 4;
___ctor_end:
.global ___ctor_end;
   .byte4=0;    // NULL terminator reauired by __ctorloop
.type ___ctor_end,STT_OBJECT; 

.section .gdtl;
.align 4;
___eh_gdt_end:
.global ___eh_gdt_end;
    .byte4=0;
.type ___eh_gdt_end,STT_OBJECT; 
 