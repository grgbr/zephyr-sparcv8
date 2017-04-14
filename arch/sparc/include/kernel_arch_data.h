#ifndef _kernel_arch_data_h_
#define _kernel_arch_data_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <nano_internal.h>

struct _callee_saved {
        uint32_t l0;
        uint32_t l1;
        uint32_t l2;
        uint32_t l3;
        uint32_t l4;
        uint32_t l5;
        uint32_t l6;
        uint32_t l7;

        uint32_t i0;
        uint32_t i1;
        uint32_t i2;
        uint32_t i3;
        uint32_t i4;
        uint32_t i5;
        uint32_t i6;
        uint32_t i7;

#if defined(CONFIG_SPARC_NO_APP_REGS)
        uint32_t g2;
        uint32_t g3;
        uint32_t g4;
#endif

        uint32_t sp;

        uint32_t pc;

        uint32_t psr;
};

typedef struct _callee_saved _callee_saved_t;

struct _caller_saved {
	/* Fill me */
};

struct _thread_arch {
	/*
	 * Fill me
	 * return value of _Swap() ?
	 * i0 in stack frame cannot be written to reliably ?
	 */
	uint32_t swap_retval;
};

typedef struct _thread_arch _thread_arch_t;

struct _kernel_arch {
	/* Fill me */
};

#ifdef __cplusplus
}
#endif

#endif /* _kernel_arch_data_h_ */
