#ifndef _kernel_arch_func__h_
#define _kernel_arch_func__h_

#ifdef __cplusplus
extern "C" {
#endif

extern char _interrupt_stack[CONFIG_ISR_STACK_SIZE];

static ALWAYS_INLINE int _is_in_isr(void)
{
	/* Alternate impl: sp on interrupt stack ? PSR_ET bit null ? */
	return _kernel.nested != 0;
}

static ALWAYS_INLINE void nanoArchInit(void)
{
	_kernel.irq_stack = _interrupt_stack + CONFIG_ISR_STACK_SIZE;
}


static ALWAYS_INLINE void
_set_thread_return_value(struct k_thread *thread, unsigned int value)
{
	/*
	 * TODO:
	 *     is swap_return_value really necessary (see struct _thread_arch) ?
	 */
	thread->arch.swap_retval = value;
}

/* TODO: fill me ??! */
static inline void _IntLibInit(void)
{
}

#ifdef __cplusplus
}
#endif

#endif
