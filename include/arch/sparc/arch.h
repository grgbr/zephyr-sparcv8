#ifndef _SPARC_ARCH_H_
#define _SPARC_ARCH_H_

#define PSR_PIL_MASK (0xf << 8)
#define PSR_ICC_MASK (0xf << 20)

#if !defined(_ASMLANGUAGE)

#include <stdint.h>
#include <irq.h>
#include <sw_isr_table.h>
#include <arch/sparc/exc.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Sparc 32-bits architecture stacks must be 8 bytes aligned
 * to support double words register load/store.
 */
#define STACK_ALIGN         8
#define STACK_ROUND_UP(x)   ROUND_UP(x, STACK_ALIGN)
#define STACK_ROUND_DOWN(x) ROUND_DOWN(x, STACK_ALIGN)

/* APIs need to support non-byte addressable architectures */
#define OCTET_TO_SIZEOFUNIT(X) (X)
#define SIZEOFUNIT_TO_OCTET(X) (X)

extern void _arch_irq_enable(unsigned int irq);
extern void _arch_irq_disable(unsigned int irq);
extern int  _arch_irq_is_enabled(unsigned int irq);
extern void _irq_spurious(void *unused);

/* TODO:
 *     implement priority / nested support
 *     direct support ?
 *     dedicated stack
 */
#define _ARCH_IRQ_CONNECT(irq_p, priority_p, isr_p, isr_param_p, flags_p) \
({                                                                        \
	_ISR_DECLARE(irq_p, 0, isr_p, isr_param_p);                       \
	irq_p;                                                            \
})

static ALWAYS_INLINE unsigned int _arch_irq_lock(void)
{
	unsigned int psr, tmp;

	__asm__ volatile ("rd %%psr, %0;"
			  "or %0, %2, %1;"
			  "wr %1, %%psr"
			  : "=r" (psr), "=r" (tmp)
			  : "I" (PSR_PIL_MASK)
			  : "memory");

	return psr & PSR_PIL_MASK;
}

static inline uint32_t _rd_psr(void)
{
	uint32_t psr;

	__asm__ volatile ("rd %%psr, %0" : "=r" (psr) : : "memory");

	return psr;
}

static ALWAYS_INLINE void _arch_irq_unlock(unsigned int pil)
{
	unsigned int tmp;

	__asm__ volatile ("rd %%psr, %0;"
			  "andn %0, %2, %0;"
			  "wr %0, %1, %%psr"
			  : "=&r" (tmp)
			  : "r" (pil), "I" (PSR_PIL_MASK)
			  : "memory");
}

extern uint32_t _timer_cycle_get_32();

static ALWAYS_INLINE uint32_t _arch_k_cycle_get_32(void)
{
	return _timer_cycle_get_32();
}

#include <arch/sparc/asm_inline.h>

#ifdef __cplusplus
}
#endif

#endif /* !defined(_ASMLANGUAGE) */

#endif
