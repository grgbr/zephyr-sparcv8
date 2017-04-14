#include <gen_offset.h>
#include <kernel_structs.h>
#include <kernel_offsets.h>

/* Architecture specific thread_arch_t member offsets */
GEN_OFFSET_SYM(_thread_arch_t, swap_retval);

/* Cooperative context switching structure member offsets */
GEN_OFFSET_SYM(_callee_saved_t, l0);
GEN_OFFSET_SYM(_callee_saved_t, l1);
GEN_OFFSET_SYM(_callee_saved_t, l2);
GEN_OFFSET_SYM(_callee_saved_t, l3);
GEN_OFFSET_SYM(_callee_saved_t, l4);
GEN_OFFSET_SYM(_callee_saved_t, l5);
GEN_OFFSET_SYM(_callee_saved_t, l6);
GEN_OFFSET_SYM(_callee_saved_t, l7);
GEN_OFFSET_SYM(_callee_saved_t, i0);
GEN_OFFSET_SYM(_callee_saved_t, i1);
GEN_OFFSET_SYM(_callee_saved_t, i2);
GEN_OFFSET_SYM(_callee_saved_t, i3);
GEN_OFFSET_SYM(_callee_saved_t, i4);
GEN_OFFSET_SYM(_callee_saved_t, i5);
GEN_OFFSET_SYM(_callee_saved_t, i6);
GEN_OFFSET_SYM(_callee_saved_t, i7);
#if defined(CONFIG_SPARC_NO_APP_REGS)
GEN_OFFSET_SYM(_callee_saved_t, g2);
GEN_OFFSET_SYM(_callee_saved_t, g3);
GEN_OFFSET_SYM(_callee_saved_t, g4);
#endif
GEN_OFFSET_SYM(_callee_saved_t, sp);
GEN_OFFSET_SYM(_callee_saved_t, pc);
GEN_OFFSET_SYM(_callee_saved_t, psr);

/* Exceptions stack frame structure member offsets and size */
GEN_OFFSET_SYM(NANO_ESF, l0);
GEN_OFFSET_SYM(NANO_ESF, l1);
GEN_OFFSET_SYM(NANO_ESF, l2);
GEN_OFFSET_SYM(NANO_ESF, l3);
GEN_OFFSET_SYM(NANO_ESF, l4);
GEN_OFFSET_SYM(NANO_ESF, l5);
GEN_OFFSET_SYM(NANO_ESF, l6);
GEN_OFFSET_SYM(NANO_ESF, l7);
GEN_OFFSET_SYM(NANO_ESF, i0);
GEN_OFFSET_SYM(NANO_ESF, i1);
GEN_OFFSET_SYM(NANO_ESF, i2);
GEN_OFFSET_SYM(NANO_ESF, i3);
GEN_OFFSET_SYM(NANO_ESF, i4);
GEN_OFFSET_SYM(NANO_ESF, i5);
GEN_OFFSET_SYM(NANO_ESF, fp);
GEN_OFFSET_SYM(NANO_ESF, i7);
GEN_OFFSET_SYM(NANO_ESF, o0);
GEN_OFFSET_SYM(NANO_ESF, o1);
GEN_OFFSET_SYM(NANO_ESF, o2);
GEN_OFFSET_SYM(NANO_ESF, o3);
GEN_OFFSET_SYM(NANO_ESF, o4);
GEN_OFFSET_SYM(NANO_ESF, o5);
GEN_OFFSET_SYM(NANO_ESF, sp);
GEN_OFFSET_SYM(NANO_ESF, o7);
GEN_OFFSET_SYM(NANO_ESF, g1);
#if !defined(CONFIG_SPARC_NO_APP_REGS)
GEN_OFFSET_SYM(NANO_ESF, g2);
GEN_OFFSET_SYM(NANO_ESF, g3);
GEN_OFFSET_SYM(NANO_ESF, g4);
#endif
#if defined(CONFIG_NESTED_INTERRUPTS)
GEN_OFFSET_SYM(NANO_ESF, irq);
#endif
GEN_OFFSET_SYM(NANO_ESF, psr);
GEN_OFFSET_SYM(NANO_ESF, pc);
GEN_OFFSET_SYM(NANO_ESF, npc);

GEN_ABSOLUTE_SYM(__NANO_ESF_SIZEOF, STACK_ROUND_UP(sizeof(NANO_ESF)));

/* No floating point support for now. */
GEN_ABSOLUTE_SYM(_K_THREAD_NO_FLOAT_SIZEOF,
		 STACK_ROUND_UP(sizeof(struct k_thread)));

/* Interrupt table entry offsets and size */
typedef struct _isr_table_entry _isr_table_entry_t;

GEN_OFFSET_SYM(_isr_table_entry_t, arg);
GEN_OFFSET_SYM(_isr_table_entry_t, isr);
GEN_ABSOLUTE_SYM(_isr_table_entry_SIZEOF, sizeof(struct _isr_table_entry));

GEN_ABS_SYM_END
