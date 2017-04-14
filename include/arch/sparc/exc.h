#ifndef _ARCH_SPARC_EXC_H_
#define _ARCH_SPARC_EXC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct __esf {
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
        uint32_t fp;
        uint32_t i7;

        uint32_t o0;
        uint32_t o1;
        uint32_t o2;
        uint32_t o3;
        uint32_t o4;
        uint32_t o5;
        uint32_t sp;
        uint32_t o7;

        uint32_t g1;
#if !defined(CONFIG_SPARC_NO_APP_REGS)
        uint32_t g2;
        uint32_t g3;
        uint32_t g4;
#endif
#if defined(CONFIG_NESTED_INTERRUPTS)
        uint32_t irq;
#endif
        uint32_t psr;
        uint32_t pc;
        uint32_t npc;
} __aligned(8);

typedef struct __esf NANO_ESF;

extern const NANO_ESF _default_esf;

#define _NANO_ERR_INVALID_TASK_EXIT (1)  /* Invalid task exit */

extern FUNC_NORETURN void _NanoFatalErrorHandler(unsigned int, const NANO_ESF*);

#ifdef __cplusplus
}
#endif

#endif
