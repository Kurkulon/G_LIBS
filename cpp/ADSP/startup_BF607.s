#include <sys/platform.h>
#include <sys/anomaly_macros_rtl.h>

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef BOOTLOADER 
#define DISABLE_CORE1
#undef ENABLE_CORE1
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define TEST_DCPLB

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define UNASSIGNED_VAL 0x81818181
#define UNASSIGNED_FILL 0

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if defined(__ADI_HAS_SEC__)
 /* Mask of interrupt bits to be enabled by default. Bits 0-4 unmaskable. */
 #define INTERRUPT_BITS (BITM_IMASK_IVG11 | BITM_IMASK_IVG15)
#else
 #define INTERRUPT_BITS (EVT_IVG15)
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if __ADSPLPBLACKFIN__>=0x220
  #define LOADIMM32REG(R,VAL) R = VAL;
#else
  #define LOADIMM32REG(R,VAL) R ## .L = LO(VAL); R ## .H = HI(VAL);
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define SEC_VECTABLE_LEN PARAM_SEC0_SCOUNT

.SECTION/NO_INIT SEC_VecTable;

	.BYTE4	_SEC_VecTable[SEC_VECTABLE_LEN];
	.GLOBAL _SEC_VecTable;
	.TYPE	_SEC_VecTable,STT_OBJECT;

//.SECTION/NO_INIT core1_SEC_VecTable;

//	.BYTE4	_core1_SEC_VecTable[SEC_VECTABLE_LEN];
//	.GLOBAL _core1_SEC_VecTable;
//	.TYPE	_core1_SEC_VecTable,STT_OBJECT;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// The requiredForROMBoot file attribute is included here as this source
// defines code and data that are used before, or as part of, the meminit
// memory initialization support.
.FILE_ATTR requiredForROMBoot;

.SECTION/DOUBLEANY core0_code;
.ALIGN 2;

start:
      // The assembler warning for anomaly 05-00-0312, issued when interrupts
      // are enabled and an an SSYNC, CSYNC or loop register load is used, is
      // not needed till interrupts are enabled. So we suppress it.
      .MESSAGE/SUPPRESS 5515;


      // Disable CPLBs as they might be enabled by initialization code
      // or still be enabled after a software reset.
		LOADIMM32REG(P1, IMEM_CONTROL)
		R2 = [P1];
		BITCLR(R2,BITP_IMEM_CONTROL_ENCPLB);
		SSYNC;                  // It is best practice to issue an
								// SSYNC before changing a memory mode.
		[P1] = R2;
		SSYNC;

		LOADIMM32REG(P1, DMEM_CONTROL)
		R2 = [P1];
		BITCLR(R2,BITP_DMEM_CONTROL_ENCPLB);
		SSYNC;                  // It is best practice to issue an
								// SSYNC before changing a memory mode.
		[P1] = R2;
		SSYNC; 

      // Set registers to unassigned value.
      LOADIMM32REG(R0, UNASSIGNED_VAL)
#if UNASSIGNED_FILL
      R1 = R0;
      R2 = R0;
      R3 = R0;
      R4 = R0;
      R5 = R0;
      R6 = R0;
      R7 = R0;
      P0 = R0;
      P1 = R0;
      P2 = R0;
      P3 = R0;
      P4 = R0;
      P5 = R0;
#endif

      // Initialize the stack.
      // Note: this points just past the end of the stack memory.
      // So the first write must be with [--SP].
      .EXTERN ldf_core0_stack_end;
      .TYPE ldf_core0_stack_end,STT_OBJECT;
      LOADIMM32REG(SP, ldf_core0_stack_end)
      USP = SP;

      // Push UNASSIGNED_VAL as RETS and old FP onto the stack to terminate
      // the call stack.
      [--SP] = R0;
      [--SP] = R0;

      // Initialize FP to point to the UNASSIGNED_VAL old FP value.
      FP = SP;

      // And make space for incoming "parameters" for functions
      // we call from here.
      SP += -12;

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

      // Zero the return registers.
      RETI = R7;
      RETX = R7;
      RETN = R7;
      RETE = R7;

      // Zero the ITEST_COMMAND and DTEST_COMMAND registers
      // (in case they have unintialized values in them that
      // cause a write somewhere when we enable cache).

      // Set the PARCTL bit for the command registers (I/DTEST use the same
      // bit position) so that any parity errors are ignored.
      // This is a workaround for the yet to characterized anomaly described
      // in TAR-50309.

        BITSET(R7, BITP_ITEST_COMMAND_PARCTL);

        LOADIMM32REG(I0, ITEST_COMMAND)
        LOADIMM32REG(I1, DTEST_COMMAND)
        [I0] = R7;
        [I1] = R7;
        CSYNC;


      // Initialize the Event Vector Table (EVT) entries other than
      // EVT0 (Emulation) and EVT1 (Reset).
        LOADIMM32REG(P0, EVT2)
        LOADIMM32REG(R1, core0_dummy_NMI)
        [P0++]	= R1;
        LOADIMM32REG(R1, core0_dummy_EVX)
        [P0++]	= R1;
        LOADIMM32REG(R1, core0_dummy_IVG)
        [P0++]	= R1;
        LOADIMM32REG(R1, core0_dummy_IVHW)
        [P0++]	= R1;
        LOADIMM32REG(R1, core0_dummy_IVTMR)
        [P0++]	= R1;
        LOADIMM32REG(R1, core0_dummy_IVG)

        P1 = 9;
        LSETUP (.ivt, .ivt) LC0 = P1;

.ivt:	[P0++] = R1;

      // Set IVG15's handler to be the start of the mode-change
      // code. Then, before we return from the Reset back to user
      // mode, we'll raise IVG15. This will mean we stay in supervisor
      // mode, and continue from the mode-change point at the
      // lowest priority.
      LOADIMM32REG(P0, EVT15)
      LOADIMM32REG(P1, supervisor_mode)
      [P0] = P1;


      // Configure SYSCFG.
      R1 = SYSCFG;
      BITSET (R1, BITP_SYSCFG_CCEN);  // Enable the cycle counter.
      BITCLR (R1, BITP_SYSCFG_SNEN);  // Disable self-nesting interrupts.
      SYSCFG = R1;

      // __install_default_handlers is called to allow the opportunity
      // to install event handlers before _main. The default version of this
      // function provided in the libraries just returns the mask passed in.
      R0 = INTERRUPT_BITS (Z);

      // Switch from reset to handling IVG15. This is Done before CPLB
      // initialization so that CPLB events can be handled as soon as
      // they are enabled.

      // We are about to enable interrupts so stop suppressing the assembler
      // warning for 05-00-0312.
      .MESSAGE/RESTORE 5515;

      // Enable interrupts using the mask returned from the call to
      // __install_default_handlers.
      STI R0;
      RAISE 15;             // handled by supervisor_mode

      // Move the processor into user mode.
      LOADIMM32REG(P0, still_interrupt_in_ipend)
      RETI = P0;

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
      // Need to do this to 'finish' servicing the reset interupt.
      .MESSAGE/SUPPRESS 1056 FOR 1 LINES;  // Suppress stall information message
      RTI;

supervisor_mode:
      [--SP] = RETI;        // re-enables the interrupt system

		SP += -8;

		LOADIMM32REG(P0, _SEC_VecTable)
		LOADIMM32REG(R1, core0_dummy_sec_vector)
		P1 = SEC_VECTABLE_LEN;
		LSETUP (.secvt, .secvt) LC0 = P1;

		.secvt:	[P0++] = R1;

		LOADIMM32REG(R0, __sec_int_dispatcher)
        LOADIMM32REG(P0, EVT11)
		[P0] = R0;

        LOADIMM32REG(P0, IMASK)
		R0 = [P0];
		BITSET(R0, 11); 
		[P0] = R0;

		.EXTERN _SystemInit;
		.TYPE _SystemInit,STT_FUNC;
		CALL.X _SystemInit;

#ifdef TEST_DCPLB

		LOADIMM32REG(P0, 0xFF800000)    // MEM_C0_BASE_L1DM_A_SRAM
		R0 = [P0];

		LOADIMM32REG(P0, 0xFF803FFC)    // MEM_C0_BASE_L1DM_A_SRAM
		R0 = [P0];

		LOADIMM32REG(P0, 0xFF900000)    // MEM_C0_BASE_L1DM_B_SRAM
		R0 = [P0];

		LOADIMM32REG(P0, 0xFF903FFC)    // MEM_C0_BASE_L1DM_B_SRAM
		R0 = [P0];

		LOADIMM32REG(P0, 0xC8000000)    // MEM_BASE_L2_ROM
		R0 = [P0];

		LOADIMM32REG(P0, 0xC8080000)    // MEM_BASE_L2_SRAM
		R0 = [P0];

		LOADIMM32REG(P0, 0xC8084000)    
		R0 = [P0];

		LOADIMM32REG(P0, 0xC8088000)    
		R0 = [P0];

		LOADIMM32REG(P0, 0xC808C000)    
		R0 = [P0];

		LOADIMM32REG(P0, 0xC8090000)    
		R0 = [P0];

		LOADIMM32REG(P0, 0xC8094000)    
		R0 = [P0];

		LOADIMM32REG(P0, 0xC8098000)    
		R0 = [P0];

		LOADIMM32REG(P0, 0xC809C000)    
		R0 = [P0];
		
        LOADIMM32REG(P0, 0xC80BFFFC)    // MEM_END_L2_SRAM  
		R0 = [P0];


#endif

		// Call constructors for C++ global scope variables.
		//.EXTERN ___ctorloop;
		//.TYPE ___ctorloop,STT_FUNC
		//CALL.X ___ctorloop;

_core0_ctorloop:

	    P5.L = ___ctor_table;
	    P5.H = ___ctor_table;

        R0 = 0;
	    [P5++] = R0; // Обнуляем для синхронизации ядер

	    R0 = [P5];
	    CC = R0 == 0;
	    if CC jump _core0_ctorloop_end;

	    R7 = -1;
	    R7 <<= 20; // 0xFFF00000
	    R6 = 0 /* -10485760 */;
        R6.H = HI(MEM_C1_BASE_L1IM); // -160 /* -10485760 */;

_core0_ctorloop_begin:

        P1 = [P5++];
        R0 = P1;    //[P5++];
	    R0 = R0 & R7;
	    CC = R0 == R6;
	    if CC jump _core0_ctorloop_skipcall (bp);

	    CALL (P1);

_core0_ctorloop_skipcall:

	    R0 = [P5];
	    CC = R0 == 0;
	    if !CC jump _core0_ctorloop_begin (bp);

_core0_ctorloop_end:

        P5.L = ___ctor_table;
	    P5.H = ___ctor_table;

        R0 = [P5];
		BITSET(R0, 0); 
	    [P5] = R0;      // Core 0 ctorloop complete

#ifdef ENABLE_CORE1

// Enable System Interface Core 1

LOADIMM32REG(P1, REG_RCU0_SIDIS)
        R0 = [P1];
        BITCLR(R0, BITP_RCU_SIDIS_SI1);  
        [P1] = R0;
 
        // Enable Core 1

        LOADIMM32REG(P1, REG_RCU0_CRCTL)
        R0 = [P1];
        BITCLR(R0, BITP_RCU_CRCTL_CR1);    
        [P1] = R0;

        CSYNC;
        SSYNC;

_wait_core1_ctorloop_complete:

        FLUSHINV[P5];
        R0 = [P5];

        CSYNC;
        SSYNC;

        CC = BITTST(R0, 1);
	    if !CC jump _wait_core1_ctorloop_complete;

#endif // ENABLE_CORE1

		// Call the application program.
		.EXTERN _main;
		.TYPE _main,STT_FUNC;
		CALL.X _main;

main_exit_loop:

		EMUEXCPT;
		IDLE;
		JUMP main_exit_loop; 

__sec_int_dispatcher:

		DISPATCHER_REG_PUSH; 

        LOADIMM32REG(P1, CEC_SID)
		P0 = [P1];					    // read CEC_SID (32-bit MMR)
		[P1] = P0;					// interrupt acknowledgement
		[--SP] = P0;

		LOADIMM32REG(P1, _SEC_VecTable)

		P1 = P1 + (P0 << 2);			// &_SEC_VecTable[index]

		P1 = [P1];

		SP += -12;

		CALL (P1);						// call the handler, preserves R0

		SP += 12;
		
		R0 = [SP++];

        LOADIMM32REG(P0, REG_SEC0_END)
		[P0] = R0;			            // R0 still contains the SID after the handler call

		DISPATCHER_REG_POP;

		RTI;

__sec_int_dispatcher.end:

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

core0_dummy_sec_vector:

		EMUEXCPT;
		RTS;
		JUMP core0_dummy_sec_vector; 
		
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

core0_dummy_NMI:

		EMUEXCPT;
		RTN;
		JUMP core0_dummy_NMI; 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

core0_dummy_EVX:

		EMUEXCPT;
		RTX;
		JUMP core0_dummy_EVX; 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

core0_dummy_IVHW:

	EMUEXCPT;
	RTI;
	JUMP core0_dummy_IVHW; 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

core0_dummy_IVTMR:

	EMUEXCPT;
	RTI;
	JUMP core0_dummy_IVTMR; 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

core0_dummy_IVG:

	EMUEXCPT;
	RTI;
	JUMP core0_dummy_IVG; 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.start.end: // End label required by the linker to find the size of the start
            // as necessary for absolute placement.
      .GLOBAL start;
      .type start,STT_FUNC;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef ENABLE_CORE1

.SECTION/DOUBLEANY core1_code;
.ALIGN 2;

core1_start:

      // The assembler warning for anomaly 05-00-0312, issued when interrupts
      // are enabled and an an SSYNC, CSYNC or loop register load is used, is
      // not needed till interrupts are enabled. So we suppress it.
      .MESSAGE/SUPPRESS 5515;


      // Disable CPLBs as they might be enabled by initialization code
      // or still be enabled after a software reset.
		LOADIMM32REG(P1, IMEM_CONTROL)
		R2 = [P1];
		BITCLR(R2,BITP_IMEM_CONTROL_ENCPLB);
		SSYNC;                  // It is best practice to issue an
								// SSYNC before changing a memory mode.
		[P1] = R2;
		SSYNC;

		LOADIMM32REG(P1, DMEM_CONTROL)
		R2 = [P1];
		BITCLR(R2,BITP_DMEM_CONTROL_ENCPLB);
		SSYNC;                  // It is best practice to issue an
								// SSYNC before changing a memory mode.
		[P1] = R2;
		SSYNC; 

      // Set registers to unassigned value.
      LOADIMM32REG(R0, UNASSIGNED_VAL)
#if UNASSIGNED_FILL
      R1 = R0;
      R2 = R0;
      R3 = R0;
      R4 = R0;
      R5 = R0;
      R6 = R0;
      R7 = R0;
      P0 = R0;
      P1 = R0;
      P2 = R0;
      P3 = R0;
      P4 = R0;
      P5 = R0;
#endif

      // Initialize the stack.
      // Note: this points just past the end of the stack memory.
      // So the first write must be with [--SP].
      .EXTERN ldf_core1_stack_end;
      .TYPE ldf_core1_stack_end,STT_OBJECT;
      LOADIMM32REG(SP, ldf_core1_stack_end)
      USP = SP;

      // Push UNASSIGNED_VAL as RETS and old FP onto the stack to terminate
      // the call stack.
      [--SP] = R0;
      [--SP] = R0;

      // Initialize FP to point to the UNASSIGNED_VAL old FP value.
      FP = SP;

      // And make space for incoming "parameters" for functions
      // we call from here.
      SP += -12;

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

      // Zero the return registers.
      RETI = R7;
      RETX = R7;
      RETN = R7;
      RETE = R7;

      // Zero the ITEST_COMMAND and DTEST_COMMAND registers
      // (in case they have unintialized values in them that
      // cause a write somewhere when we enable cache).

      // Set the PARCTL bit for the command registers (I/DTEST use the same
      // bit position) so that any parity errors are ignored.
      // This is a workaround for the yet to characterized anomaly described
      // in TAR-50309.

        BITSET(R7, BITP_ITEST_COMMAND_PARCTL);

        LOADIMM32REG(I0, ITEST_COMMAND)
        LOADIMM32REG(I1, DTEST_COMMAND)
        [I0] = R7;
        [I1] = R7;
        CSYNC;

        // Initialize the Event Vector Table (EVT) entries other than
        // EVT0 (Emulation) and EVT1 (Reset).
        LOADIMM32REG(P0, EVT2)
        LOADIMM32REG(R1, core1_dummy_NMI)
        [P0++]	= R1;
        LOADIMM32REG(R1, core1_dummy_EVX)
        [P0++]	= R1;
        LOADIMM32REG(R1, core1_dummy_IVG)
        [P0++]	= R1;
        LOADIMM32REG(R1, core1_dummy_IVHW)
        [P0++]	= R1;
        LOADIMM32REG(R1, core1_dummy_IVTMR)
        [P0++]	= R1;
        LOADIMM32REG(R1, core1_dummy_IVG)

		P1 = 9;
		LSETUP (.core1_ivt, .core1_ivt) LC0 = P1;

.core1_ivt:	[P0++] = R1;

      // Set IVG15's handler to be the start of the mode-change
      // code. Then, before we return from the Reset back to user
      // mode, we'll raise IVG15. This will mean we stay in supervisor
      // mode, and continue from the mode-change point at the
      // lowest priority.
      LOADIMM32REG(P0, EVT15)
      LOADIMM32REG(P1, core1_supervisor_mode)
      [P0] = P1;

      // Configure SYSCFG.
      R1 = SYSCFG;
      BITSET (R1, BITP_SYSCFG_CCEN);  // Enable the cycle counter.
      BITCLR (R1, BITP_SYSCFG_SNEN);  // Disable self-nesting interrupts.
      SYSCFG = R1;

      // __install_default_handlers is called to allow the opportunity
      // to install event handlers before _main. The default version of this
      // function provided in the libraries just returns the mask passed in.
      R0 = INTERRUPT_BITS (Z);

      // Switch from reset to handling IVG15. This is Done before CPLB
      // initialization so that CPLB events can be handled as soon as
      // they are enabled.

      // We are about to enable interrupts so stop suppressing the assembler
      // warning for 05-00-0312.
      .MESSAGE/RESTORE 5515;

      // Enable interrupts using the mask returned from the call to
      // __install_default_handlers.
      STI R0;
      RAISE 15;             // handled by supervisor_mode

      // Move the processor into user mode.
      LOADIMM32REG(P0, core1_still_interrupt_in_ipend)
      RETI = P0;

core1_still_interrupt_in_ipend:

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
      // Need to do this to 'finish' servicing the reset interupt.
      .MESSAGE/SUPPRESS 1056 FOR 1 LINES;  // Suppress stall information message
      RTI;

core1_supervisor_mode:

      [--SP] = RETI;        // re-enables the interrupt system

		SP += -8;

		//LOADIMM32REG(P0, SEC_VecTable)
		//LOADIMM32REG(R1, core1_dummy_sec_vector)
		//P1 = SEC_VECTABLE_LEN;
		//LSETUP (.core1_secvt, .core1_secvt) LC0 = P1;

		//.core1_secvt:	[P0++] = R1;

		LOADIMM32REG(R0, __core1_sec_int_dispatcher)
        LOADIMM32REG(P0, EVT11)
		[P0] = R0;

        LOADIMM32REG(P0, IMASK)
		R0 = [P0];
		BITSET(R0, 11); 
		[P0] = R0;

		.EXTERN _Core1_SystemInit;
		.TYPE _Core1_SystemInit,STT_FUNC;
		CALL.X _Core1_SystemInit;

#ifdef TEST_DCPLB_CORE1

		LOADIMM32REG(P0, 0x04000000)
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
		//.EXTERN ___ctorloop;
		//.TYPE ___ctorloop,STT_FUNC
		//CALL.X ___ctorloop;

_core1_ctorloop:

	    P5.L = ___ctor_table+4;
	    P5.H = ___ctor_table+4;

	    R0 = [P5];
	    CC = R0 == 0;
	    if CC jump _core1_ctorloop_end;

	    R7 = -1;
	    R7 <<= 20; // 0xFFF00000
	    R6 = 0 /* -10485760 */;
        R6.H = HI(MEM_C1_BASE_L1IM); // -160 /* -10485760 */;

_core1_ctorloop_begin:

        P1 = [P5++];
        R0 = P1;    //[P5++];
	    R0 = R0 & R7;
	    CC = R0 == R6;
	    if !CC jump _core1_ctorloop_skipcall (bp);

	    CALL (P1);

_core1_ctorloop_skipcall:

	    R0 = [P5];
	    CC = R0 == 0;
	    if !CC jump _core1_ctorloop_begin (bp);

_core1_ctorloop_end:

        P5.L = ___ctor_table;
	    P5.H = ___ctor_table;

        R0 = [P5];
		BITSET(R0, 1); 
	    [P5] = R0;      // Core 0 ctorloop complete

        CSYNC;
        SSYNC;

_wait_core0_ctorloop_complete:

        FLUSHINV[P5];

        CSYNC;
        SSYNC;
        R0 = [P5];

        CSYNC;
        SSYNC;

        CC = BITTST(R0, 0);
	    if !CC jump _wait_core0_ctorloop_complete;


		// Call the application program.
		.EXTERN _core1_main;
		.TYPE _core1_main,STT_FUNC;
		CALL.X _core1_main;

core1_main_exit_loop:

		EMUEXCPT;
		IDLE;
		JUMP core1_main_exit_loop; 

__core1_sec_int_dispatcher:

		DISPATCHER_REG_PUSH; 

        LOADIMM32REG(P1, CEC_SID)
		P0 = [P1];					    // read CEC_SID (32-bit MMR)
		[P1] = P0;					// interrupt acknowledgement
		[--SP] = P0;

		LOADIMM32REG(P1, SEC_VecTable)

		P1 = P1 + (P0 << 2);			// &_SEC_VecTable[index]

		P1 = [P1];

		SP += -12;

		CALL (P1);						// call the handler, preserves R0

		SP += 12;
		
		R0 = [SP++];

        LOADIMM32REG(P0, REG_SEC0_END)
		[P0] = R0;			            // R0 still contains the SID after the handler call

		DISPATCHER_REG_POP;

		RTI;

__core1_sec_int_dispatcher.end:

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

core1_dummy_sec_vector:

		EMUEXCPT;
		RTS;
		JUMP core1_dummy_sec_vector; 
		
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

core1_dummy_NMI:

		EMUEXCPT;
		RTN;
		JUMP core1_dummy_NMI; 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

core1_dummy_EVX:

		EMUEXCPT;
		RTX;
		JUMP core1_dummy_EVX; 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

core1_dummy_IVHW:

	EMUEXCPT;
	RTI;
	JUMP core1_dummy_IVHW; 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

core1_dummy_IVTMR:

	EMUEXCPT;
	RTI;
	JUMP core1_dummy_IVTMR; 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

core1_dummy_IVG:

	EMUEXCPT;
	RTI;
	JUMP core1_dummy_IVG; 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.core1_start.end: // End label required by the linker to find the size of the start
            // as necessary for absolute placement.
      .GLOBAL core1_start;
      .type core1_start,STT_FUNC;

#endif // ENABLE_CORE1


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
      .TYPE ___eh_gdt, STT_OBJECT;
      .EXTERN ___eh_gdt_end;
      .type ___eh_gdt_end, STT_OBJECT;
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
    .byte4 = 0;
.type ___eh_gdt_end, STT_OBJECT;
