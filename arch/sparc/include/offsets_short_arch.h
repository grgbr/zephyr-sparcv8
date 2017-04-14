#ifndef _offsets_short_arch__h_
#define _offsets_short_arch__h_

#include <offsets.h>

/* Fill me ! */

/* kernel */

/* nothing for now */

/* end - kernel */

/* threads */

#define _thread_offset_to_l0 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_l0_OFFSET)
#define _thread_offset_to_l1 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_l1_OFFSET)
#define _thread_offset_to_l2 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_l2_OFFSET)
#define _thread_offset_to_l3 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_l3_OFFSET)
#define _thread_offset_to_l4 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_l4_OFFSET)
#define _thread_offset_to_l5 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_l5_OFFSET)
#define _thread_offset_to_l6 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_l6_OFFSET)
#define _thread_offset_to_l7 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_l7_OFFSET)

#define _thread_offset_to_i0 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_i0_OFFSET)
#define _thread_offset_to_i1 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_i1_OFFSET)
#define _thread_offset_to_i2 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_i2_OFFSET)
#define _thread_offset_to_i3 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_i3_OFFSET)
#define _thread_offset_to_i4 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_i4_OFFSET)
#define _thread_offset_to_i5 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_i5_OFFSET)
#define _thread_offset_to_i6 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_i6_OFFSET)
#define _thread_offset_to_i7 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_i7_OFFSET)

#if defined(CONFIG_SPARC_NO_APP_REGS)
#define _thread_offset_to_g2 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_g2_OFFSET)
#define _thread_offset_to_g3 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_g3_OFFSET)
#define _thread_offset_to_g4 \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_g4_OFFSET)
#endif

#define _thread_offset_to_sp \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_sp_OFFSET)

#define _thread_offset_to_pc \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_pc_OFFSET)

#define _thread_offset_to_psr \
        (___thread_t_callee_saved_OFFSET + ___callee_saved_t_psr_OFFSET)

#define _thread_offset_to_swap_retval \
	(___thread_t_arch_OFFSET + ___thread_arch_t_swap_retval_OFFSET)

/* end - threads */

#endif /* _offsets_short_arch__h_ */
