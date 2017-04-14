#include <toolchain.h>
#include <sections.h>
#include <arch/sparc/arch.h>
#include <offsets_short.h>

	/* Traps enabled */
	.equiv PSR_ET,          1 << 5
	/* Supervisor mode (previous) */
	.equiv PSR_PS,          1 << 6
	/* Supervisor mode (current) */
	.equiv PSR_S,           1 << 7
	/* Integer Condition Codes */
	.equiv PSR_ICC_C_SHIFT, 20
	.equiv PSR_ICC_V_SHIFT, 21
	.equiv PSR_ICC_Z_SHIFT, 22
	.equiv PSR_ICC_N_SHIFT, 23
	/* Version id (read only) */
	.equiv PSR_VERS_BITS,   4
	.equiv PSR_VERS_SHIFT,  24
	.equiv PSR_VERS_MASK,   ((1 << PSR_VERS_BITS) - 1) << PSR_VERS_SHIFT
	/* Implementation id (read only) */
	.equiv PSR_IMPL_BITS,   4
	.equiv PSR_IMPL_SHIFT,  28
	.equiv PSR_IMPL_MASK,   ((1 << PSR_IMPL_BITS) - 1) << PSR_IMPL_SHIFT

	.equiv NWINDOWS,        8


	.macro _COFFIN_TRAP
		b   _coffin
		nop
		nop
		nop
	.endm

	/*
	 * Explicitly generate software trap, forcing processor to enter error
	 * state if already processing an exception.
	 * Used to implement trap handlers for unimplemented or reserved for
	 * future use entries.
	 */
	.macro _ERROR_TRAP
		ta  0
		nop
		nop
		nop
	.endm

	.macro _IRQ_TRAP irq:req
		mov     \irq, %l0
		sethi   %hi(_sw_isr_table + \
		            ((\irq - 1) * _isr_table_entry_SIZEOF)), %g5
		b       _handle_irq
		or      %g5, %lo(_sw_isr_table + \
		                 ((\irq - 1) * _isr_table_entry_SIZEOF)), %g5
	.endm

GTEXT(_Swap)
GTEXT(__start)
GTEXT(_thread_entry_wrapper)

/******************************************************************************
 * Trap / exception handling
 *
 * Traps decrement the current window pointer to the next register window and
 * causes the hardware to write the trapped program counters into l1 (PC) and l2
 * (nPC). Window decrement is performed without test for overflow.
 * They are free to use the other 5 local registers in the new window.
 * They may need to backup PSR in another local register if altering its
 * content to be able to restore its original value before resuming trapped
 * instruction execution.
 ******************************************************************************/

/*
 * Trap / exception handlers table
 */
SECTION_FUNC(trap, _vector)
	b       _reset /*    0: reset exception */
	nop
	nop
	nop

	_COFFIN_TRAP /*    1: instruction access exception */
	_COFFIN_TRAP /*    2: illegal instruction exception */
	_COFFIN_TRAP /*    3: privileged instruction exception */
	_COFFIN_TRAP /*    4: floating point unit disabled exception */

	/*                 5: window overflow exception */
	save
	std     %l0, [%sp + 0]   /* backup registers onto stack */
	b       _regwin_ovfl
	std     %l2, [%sp + 8]


	/*                 6: window underflow exception */
	rd      %wim, %l3
	sll	%l3, 1, %l4
	b       _regwin_undfl
	srl     %l3, (NWINDOWS - 1), %l3


	_COFFIN_TRAP /*    7: memory address not aligned exception */
	_COFFIN_TRAP /*    8: floating point exception */
	_COFFIN_TRAP /*    9: data access exception */
	_COFFIN_TRAP /* 0x0a: tag overflow exception */
	_COFFIN_TRAP /* 0x0b: watchpoint detected exception */
	_ERROR_TRAP  /* 0x0c: reserved for future use */
	_ERROR_TRAP  /* 0x0d: reserved for future use */
	_ERROR_TRAP  /* 0x0e: reserved for future use */
	_ERROR_TRAP  /* 0x0f: reserved for future use */
	_ERROR_TRAP  /* 0x10: reserved for future use */
	_IRQ_TRAP 1  /* 0x11: interrupt level 1 */
	_IRQ_TRAP 2  /* 0x12: interrupt level 2 */
	_IRQ_TRAP 3  /* 0x13: interrupt level 3 */
	_IRQ_TRAP 4  /* 0x14: interrupt level 4 */
	_IRQ_TRAP 5  /* 0x15: interrupt level 5 */
	_IRQ_TRAP 6  /* 0x16: interrupt level 6 */
	_IRQ_TRAP 7  /* 0x17: interrupt level 7 */
	_IRQ_TRAP 8  /* 0x18: interrupt level 8 */
	_IRQ_TRAP 9  /* 0x19: interrupt level 9 */
	_IRQ_TRAP 10 /* 0x1a: interrupt level 10 */
	_IRQ_TRAP 11 /* 0x1b: interrupt level 11 */
	_IRQ_TRAP 12 /* 0x1c: interrupt level 12 */
	_IRQ_TRAP 13 /* 0x1d: interrupt level 13 */
	_IRQ_TRAP 14 /* 0x1e: interrupt level 14 */
	_IRQ_TRAP 15 /* 0x1f: interrupt level 15 */
	_ERROR_TRAP  /* 0x20: register hardware error exception */
	_ERROR_TRAP  /* 0x21: instruction access error (not supported) */
	_ERROR_TRAP  /* 0x22: reserved for future use */
	_ERROR_TRAP  /* 0x23: reserved for future use */
	_ERROR_TRAP  /* 0x24: coprocessor disabled exception (no coprocessor) */
	_ERROR_TRAP  /* 0x25: unimplemented flush (not supported) */
	_ERROR_TRAP  /* 0x26: reserved for future use */
	_ERROR_TRAP  /* 0x27: reserved for future use */
	_ERROR_TRAP  /* 0x28: coprocessor exception (no coprocessor) */
	_ERROR_TRAP  /* 0x29: data access error (not supported) */
	_COFFIN_TRAP /* 0x2a: division by zero exception */
	_COFFIN_TRAP /* 0x2b: data store / write buffer error exception */
	_ERROR_TRAP  /* 0x2c: data access MMU miss (no MMU) */
	_ERROR_TRAP  /* 0x2d: reserved for future use */
	_ERROR_TRAP  /* 0x2e: reserved for future use */
	_ERROR_TRAP  /* 0x2f: reserved for future use */
	_ERROR_TRAP  /* 0x30: reserved for future use */
	_ERROR_TRAP  /* 0x31: reserved for future use */
	_ERROR_TRAP  /* 0x32: reserved for future use */
	_ERROR_TRAP  /* 0x33: reserved for future use */
	_ERROR_TRAP  /* 0x34: reserved for future use */
	_ERROR_TRAP  /* 0x35: reserved for future use */
	_ERROR_TRAP  /* 0x36: reserved for future use */
	_ERROR_TRAP  /* 0x37: reserved for future use */
	_ERROR_TRAP  /* 0x38: reserved for future use */
	_ERROR_TRAP  /* 0x39: reserved for future use */
	_ERROR_TRAP  /* 0x3a: reserved for future use */
	_ERROR_TRAP  /* 0x3b: reserved for future use */
	_ERROR_TRAP  /* 0x3c: instruction access MMU miss (no MMU) */
	_ERROR_TRAP  /* 0x3d - 0x5f: reserved for future use ... */
	_ERROR_TRAP
	_ERROR_TRAP  /* ... 0x3f ... */
	             /* ... 0x40 ... */
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	             /* ... 0x50 ... */
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	             /* 0x60 - 0x7f: implementation dependent traps (none) */
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	             /*
	              * 0x80 - 0xff: software traps (not supported since user
	              * mode not implemented).
	              */
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;
	_ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP; _ERROR_TRAP;


/******************************************************************************
 * Register window overflow exception handler.
 * l0: PSR
 * l1: PC
 * l2: nPC
 * l3: original WIM, i.e. before exception
 * l7: backup of g1 register
 ******************************************************************************/
_regwin_ovfl:
	/*
	 * compute new window mask
	 */
	rd      %wim, %l0
	srl	%l0, 1, %l1
	sll	%l0, (NWINDOWS - 1) , %l0
	wr      %l0, %l1, %wim        /* set new window mask */

	/*
	 * proceed to saving registers window onto stack
	 */
	std     %l6, [%sp + 24]
	std     %i0, [%sp + 32]
	std     %i2, [%sp + 40]
	std     %i4, [%sp + 48]
	std     %i6, [%sp + 56]
	restore                  /* get back to trap window */

	jmp     %l1              /* replay the trapped save / restore */
	rett    %l2


_regwin_undfl:
	wr      %l3, %l4, %wim        /* set new window mask */
	nop
	nop
	nop

	restore
	restore
	ldd     [%sp + 0], %l0
	ldd     [%sp + 8], %l2
	ldd     [%sp + 16], %l4
	ldd     [%sp + 24], %l6
	ldd     [%sp + 32], %i0
	ldd     [%sp + 40], %i2
	ldd     [%sp + 48], %i4
	ldd     [%sp + 56], %i6
	save
	save

	jmp     %l1              /* replay the trapped save / restore */
	rett    %l2


	.section .trap._handle_irq, "ax"
	.local   _handle_irq
	.type    _handle_irq,@function
	.align   4
_handle_irq:
	! Fetch processor state (PSR) for later backup purpose and before it is
	! modified by compare and branch instructions below.
	rd      %psr, %l3

#if defined(CONFIG_NESTED_INTERRUPTS)
	! Get nested interrupt count
	ld      [%g6 + _kernel_offset_to_nested], %l5
	cmp     %l5, 0

	! Increment nested interrupt count
	inc     %l5

	! Check if we are in a nested interrupt thanks to the above compare
	! instruction
	bnz     .L_on_irq_stack
	mov     %fp, %l4
#else
	! Mark interrupt context as ongoing
	mov     1, %l5
#endif

	! Not nested: load top of interrupt stack
	ld      [%g6 + _kernel_offset_to_irq_stack], %l4

.L_on_irq_stack:
	! Store updated nested interrupt count
	st      %l5, [%g6 + _kernel_offset_to_nested]

	! Backup public / global registers before switching to interrupted
	! context register window
	st      %g1, [%l4 - __NANO_ESF_SIZEOF + __NANO_ESF_g1_OFFSET]  ! global registers...
#if !defined(CONFIG_SPARC_NO_APP_REGS)
	std     %g2, [%l4 - __NANO_ESF_SIZEOF + __NANO_ESF_g2_OFFSET]  ! ...in use by...
	st      %g4, [%l4 - __NANO_ESF_SIZEOF + __NANO_ESF_g4_OFFSET]  ! ...C runtime
#endif
#if defined(CONFIG_NESTED_INTERRUPTS)
	! Save interrupt number to later restore it at controller level (see
	! below).
	st      %l0, [%l4 - __NANO_ESF_SIZEOF + __NANO_ESF_irq_OFFSET]
	! In the meantime, as g1 register is now safe to use, make interrupt
	! number visible to all register windows (because of restore instruction
	! below)...
	mov     %l0, %g1
#endif
	st      %l3, [%l4 - __NANO_ESF_SIZEOF + __NANO_ESF_psr_OFFSET] ! PSR
	st      %l1, [%l4 - __NANO_ESF_SIZEOF + __NANO_ESF_pc_OFFSET]  ! PC
	st      %l2, [%l4 - __NANO_ESF_SIZEOF + __NANO_ESF_npc_OFFSET] ! NPC

	! Now switch to interrupted context register window: We will use it
	! untill interrupt service routine called below has returned. This is
	! required so that another register window is always available to handle
	! exceptions that way happen while in interrupt context...
	! Note that stack top (l4) is transfered to g4 since interrupted
	! context register window currently holds private content.
	restore %l4, 0, %g4
	std     %l0, [%g4 - __NANO_ESF_SIZEOF + __NANO_ESF_l0_OFFSET]
	std     %l2, [%g4 - __NANO_ESF_SIZEOF + __NANO_ESF_l2_OFFSET]
	std     %l4, [%g4 - __NANO_ESF_SIZEOF + __NANO_ESF_l4_OFFSET]
	std     %l6, [%g4 - __NANO_ESF_SIZEOF + __NANO_ESF_l6_OFFSET]
	std     %i0, [%g4 - __NANO_ESF_SIZEOF + __NANO_ESF_i0_OFFSET]
	std     %i2, [%g4 - __NANO_ESF_SIZEOF + __NANO_ESF_i2_OFFSET]
	std     %i4, [%g4 - __NANO_ESF_SIZEOF + __NANO_ESF_i4_OFFSET]
	std     %fp, [%g4 - __NANO_ESF_SIZEOF + __NANO_ESF_fp_OFFSET]
	std     %o0, [%g4 - __NANO_ESF_SIZEOF + __NANO_ESF_o0_OFFSET]
	std     %o2, [%g4 - __NANO_ESF_SIZEOF + __NANO_ESF_o2_OFFSET]
	std     %o4, [%g4 - __NANO_ESF_SIZEOF + __NANO_ESF_o4_OFFSET]
	std     %sp, [%g4 - __NANO_ESF_SIZEOF + __NANO_ESF_sp_OFFSET]

	! setup stack area for C runtime: reserve enough space for
	! void _arch_irq_disable(int irq) and interrupt routine
	! void (*isr)(void *) operations:
	!
	! g7/fp ->                                             high addresses
	!         |------------------------------------------| _
	!         |   one single argument storage (4 bytes)  |  |
	!         |------------------------------------------|  |
	!         | aggregate return value pointer (4 bytes) |  |
	!         |    (SPARC ABI requirement, not used)     |   \ 72 bytes
	!         |------------------------------------------|   /
	!         |  i0-i7 registers (8 registers x 4 bytes) |  |
	!         |------------------------------------------|  |
	!         |  l0-l7 registers (8 registers x 4 bytes) | _|
	!         |==========================================|
	!    sp ->|   _arch_irq_disable() / isr() allocated  |
	!         |                stack space               |
	!         |                                          |
	!                                                      low addresses
	!
	! Update fp as well for debugging purpose (really needed ?!)
	add     %g4, -__NANO_ESF_SIZEOF, %fp
	add     %fp, -72, %sp

#if defined(CONFIG_NESTED_INTERRUPTS)
	! mask interrupt at controller level
	call    _arch_irq_disable
	mov     %g1, %o0

	! now that current interrupt is "blocked", enable traps and interrupts
	! back
	rd      %psr, %l3
	wr      %l3, PSR_ET, %psr
#endif

	! invoke interrupt service routine with argument registered at connect
	! time
	ld      [%g5 + ___isr_table_entry_t_isr_OFFSET], %l0
	call    %l0
	ld      [%g5 + ___isr_table_entry_t_arg_OFFSET], %o0

#if defined(CONFIG_NESTED_INTERRUPTS)
	! disable traps and interrupts again to safely restore interrupted
	! context
	rd      %psr, %l3
	wr      %l3, PSR_ET, %psr

	! unmask interrupt at controller level
	call    _arch_irq_enable
	ld      [%fp + __NANO_ESF_irq_OFFSET], %o0
#endif

	mov     %fp, %g5

	! restore interrupted context registers content
	ldd     [%g5 + __NANO_ESF_l0_OFFSET], %l0
	ldd     [%g5 + __NANO_ESF_l2_OFFSET], %l2
	ldd     [%g5 + __NANO_ESF_l4_OFFSET], %l4
	ldd     [%g5 + __NANO_ESF_l6_OFFSET], %l6
	ldd     [%g5 + __NANO_ESF_i0_OFFSET], %i0
	ldd     [%g5 + __NANO_ESF_i2_OFFSET], %i2
	ldd     [%g5 + __NANO_ESF_i4_OFFSET], %i4
	ldd     [%g5 + __NANO_ESF_fp_OFFSET], %fp
	ldd     [%g5 + __NANO_ESF_o0_OFFSET], %o0
	ldd     [%g5 + __NANO_ESF_o2_OFFSET], %o2
	ldd     [%g5 + __NANO_ESF_o4_OFFSET], %o4
	ldd     [%g5 + __NANO_ESF_sp_OFFSET], %sp

	save ! switch back to exception register window

	! restore global registers
	ld      [%g5 + __NANO_ESF_g1_OFFSET], %g1
#if !defined(CONFIG_SPARC_NO_APP_REGS)
	ldd     [%g5 + __NANO_ESF_g2_OFFSET], %g2
	ld      [%g5 + __NANO_ESF_g4_OFFSET], %g4
#endif

#if defined(CONFIG_NESTED_INTERRUPTS)
	! decrement nested interrupt count
	ld      [%g6 + _kernel_offset_to_nested], %l4
	sub     %l4, 1, %l4
	st      %l4, [%g6 + _kernel_offset_to_nested]
#else
	st      %g0, [%g6 + _kernel_offset_to_nested]
#endif

	! restore PSR content at the time interrupt vector was entered
	ld      [%g5 + __NANO_ESF_psr_OFFSET], %l3
	wr      %l3, %psr

	! return to interrupted context:
	! * restore interrupted context supervisor state and register window
	! * re-enable traps
	! * replay the interrupted instruction
	ld      [%g5 + __NANO_ESF_pc_OFFSET], %l1
	ld      [%g5 + __NANO_ESF_npc_OFFSET], %l2
	jmp     %l1
	rett    %l2


/******************************************************************************
 * Out of reset trap handler.
 * At reset time, no particular processor or memory state may assumed.
 ******************************************************************************/
SECTION_FUNC(trap, _reset)
	/* Reset processor state to supervisor mode with traps disabled. */
	wr      PSR_S | PSR_PS, %psr

	/* Set trap table base address. */
	set     _vector, %l0
	wr      %l0, %tbr


SECTION_FUNC(trap, __start)
	! Clear bss section
	set     __bss_start, %l0
	set     __bss_end, %l1
.L1:	add     %l0, 4, %l0
	cmp     %l0, %l1
	bl      .L1
	clr     [%l0 - 4]

	! TODO: copy data if XIP

	set     _kernel, %g6

	! Setup stack for C runtime (no register windows used).
	wr      0, %wim
	set     _interrupt_stack + CONFIG_ISR_STACK_SIZE, %fp
	add     %fp, -64, %sp
	call    _Cstart
	wr      %g1, PSR_S | PSR_PS | PSR_ET, %psr


	.local _coffin
_coffin:
	b       _coffin
	nop

/******************************************************************************
 * int _Swap(unsigned int key) - cooperative context switch
 *
 * Called with interrupts disabled via irq_lock() with its returned key
 * (PSR PIL field) passed as key argument.
 * Return -EAGAIN, or a value set by a call to _set_thread_return_value()
 *
 * System V ABI, SPARCv8 Processor Supplement (third edition), defines registers
 * set usage convention as follows:
 *
 *   name  volatile  usage
 *     g0  NA        always 0
 *     g1  yes       temporary value
 *     g2  no / yes  global 2 / temporary value (see GCC note)
 *     g3  no / yes  global 3 / temporary value (see GCC note)
 *     g4  no / yes  global 4 / temporary value (see GCC note)
 *     g5  NA        reserved for OS (Procedure Linkage Table ?)
 *     g6  NA        reserved for OS (CPU control block ?)
 *     g7  NA        reserved for OS (current thread for TLS support)
 *
 *     i0  no        in parameter 0 / out return value ?
 *     i1  no        in parameter 1 / out return value ?
 *     i2  no        in parameter 2
 *     i3  no        in parameter 3
 *     i4  no        in parameter 4
 *     i5  no        in parameter 5
 *  i6/fp  no        frame pointer when compiled with multiple register windows
 *                   scratch register otherwise (i.e., single)
 *  i7/fp  no        return address - 8 when compiled with single register
 *                   window frame pointer otherwise (i.e., multiple)
 *
 *  l0-l7  no        registers local to function
 *
 *     o0  yes       out parameter 0 / in return value ?
 *     o1  yes       out parameter 1 / in return value ?
 *     o2  yes       out parameter 2
 *     o3  yes       out parameter 3
 *     o4  yes       out parameter 4
 *     o5  yes       out parameter 5
 *  o6/sp  no        stack pointer
 *     o7  yes       temporary value / address of call instruction
 *
 * Temporary / volatile registers are assumed to be clobbered across function
 * calls, whereas non volatile ones are assumed to be preserved.
 * From a cooperative context switching perspective, this means that callers of
 * _Swap() will save (on stack) all volatile registers content if necessary
 * before jumping to _Swap().
 * Called functions should preserve its calling functions's local, in, sp, g2-g4
 * (if GCC -mapp-regs option disabled), g6 and g7 registers.
 *
 * About GCC registers usage:
 *   * With -mapp-regs option enabled (the default), GCC treats g2-g4 (in
 *     addition to g1) as volatile / temporary registers.
 *   * Also note that when compiled using the flat / single register window
 *     model, i7 is used as the frame pointer. This model is compatible with
 *     the normal register window model. Code from either may be intermixed.,
 *     The local registers and the input registers (0–5) are still treated as
 *     "call saved" registers and will be saved on the stack as necessary.
 *
 * Hence, when compiled with flat / single register window model, _Swap() has to
 * save the following registers for the switched out thread: l0-l7, i0-i7, g2-g4
 * (if GCC -mapp-regs option disabled), g5-g7, sp, pc (npc will be inferred from
 * pc), and psr registers.
 ******************************************************************************/
SECTION_FUNC(TEXT, _Swap)
	! load current thread address into g7
	ld      [%g6 + _kernel_offset_to_current], %g7

	! backup current thread's register state
	std     %l0, [%g7 + _thread_offset_to_l0]  ! backup local regs
	std     %l2, [%g7 + _thread_offset_to_l2]
	std     %l4, [%g7 + _thread_offset_to_l4]
	std     %l6, [%g7 + _thread_offset_to_l6]
	std     %i0, [%g7 + _thread_offset_to_i0]  ! backup in regs
	std     %i2, [%g7 + _thread_offset_to_i2]
	std     %i4, [%g7 + _thread_offset_to_i4]
	std     %i6, [%g7 + _thread_offset_to_i6]
#if defined(CONFIG_SPARC_NO_APP_REGS)
	std     %g2, [%g7 + _thread_offset_to_g2]  ! backup global regs
	st      %g4, [%g7 + _thread_offset_to_g4]
#endif
	st      %sp, [%g7 + _thread_offset_to_sp]  ! backup stack pointer
	st      %o7, [%g7 + _thread_offset_to_pc]  ! backup program counter
	rd      %psr, %g1                          ! fetch processor state (PSR)
	andn    %g1, PSR_PIL_MASK, %g1             ! clear PSR's PIL field
	or      %g1, %o0, %g1                      ! PIL field <= key argument
	st      %g1, [%g7 + _thread_offset_to_psr] ! backup PSR

	! store default _Swap() return value
	sethi   %hi(_k_neg_eagain), %g1
	ld      [%g1 + %lo(_k_neg_eagain)], %g1
	st      %g1, [%g7 + _thread_offset_to_swap_retval]

	! load address of next thread to be switched in into g7
	ld      [%g6 + _kernel_offset_to_ready_q_cache], %g7

	! store address of next thread to be switched in into current thread
	! pointer
	st      %g7, [%g6 + _kernel_offset_to_current]

	! restore register state of switched in thread
	ldd     [%g7 + _thread_offset_to_l0], %l0  ! restore local regs
	ldd     [%g7 + _thread_offset_to_l2], %l2
	ldd     [%g7 + _thread_offset_to_l4], %l4
	ldd     [%g7 + _thread_offset_to_l6], %l6
	ldd     [%g7 + _thread_offset_to_i0], %i0  ! restore in regs
	ldd     [%g7 + _thread_offset_to_i2], %i2
	ldd     [%g7 + _thread_offset_to_i4], %i4
	ldd     [%g7 + _thread_offset_to_i6], %i6
#if defined(CONFIG_SPARC_NO_APP_REGS)
	ldd     [%g7 + _thread_offset_to_g2], %g2  ! restore global regs
	ld      [%g7 + _thread_offset_to_g4], %g4
#endif
	ld      [%g7 + _thread_offset_to_sp], %sp  ! restore stack pointer
	ld      [%g7 + _thread_offset_to_pc], %o7  ! fetch address to return to
	ld      [%g7 + _thread_offset_to_psr], %g1 ! fetch processor state (PSR)

	! Restore switched in thread's return value into "return value" register
	! o0
	ld      [%g7 + _thread_offset_to_swap_retval], %o0

	! Now restore processor state (as well as the switched int thread's
	! interrupt state)
	wr      %g1, %psr
	nop

	! return to switched in thread's next instruction (fetched above into
	! o7) + 8
	retl
	nop

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! void _thread_entry_wrapper(void)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
SECTION_FUNC(TEXT, _thread_entry_wrapper)
	ldd     [%sp], %o0
	ldd     [%sp + 8], %o2

	/* pop all the stuff that we just loaded into registers */
	! TODO: round up stack pointer STACK_ROUND_UP(sizeof(struct init_stack_frame))
	add     %sp, 16, %sp

	call    _thread_entry
	mov     %g0, %o7
