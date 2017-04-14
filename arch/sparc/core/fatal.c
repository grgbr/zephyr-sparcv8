#include <kernel.h>
#include <kernel_structs.h>
#include <misc/printk.h>

/*
 * Define a default ESF for use with _NanoFatalErrorHandler() in the event
 * the caller does not have a NANO_ESF to pass
 */
const NANO_ESF _default_esf = {
	.l0 = 0xdeadbaad,
	.l1 = 0xdeadbaad,
	.l2 = 0xdeadbaad,
	.l3 = 0xdeadbaad,
	.l4 = 0xdeadbaad,
	.l5 = 0xdeadbaad,
	.l6 = 0xdeadbaad,
	.l7 = 0xdeadbaad,

	.i0 = 0xdeadbaad,
	.i1 = 0xdeadbaad,
	.i2 = 0xdeadbaad,
	.i3 = 0xdeadbaad,
	.i4 = 0xdeadbaad,
	.i5 = 0xdeadbaad,
	.fp = 0xdeadbaad,
	.i7 = 0xdeadbaad,

#if defined(CONFIG_SPARC_NO_APP_REGS)
	.g2 = 0xdeadbaad,
	.g3 = 0xdeadbaad,
	.g4 = 0xdeadbaad,
#endif

	.sp = 0xdeadbaad,

	.pc = 0xdeadbaad,

	.psr = 0xdeadbaad
};

FUNC_NORETURN void _SysFatalErrorHandler(unsigned int    reason,
					 const NANO_ESF *esf)
{
        ARG_UNUSED(reason);
        ARG_UNUSED(esf);

#if !defined(CONFIG_SIMPLE_FATAL_ERROR_HANDLER)
        if (k_is_in_isr() || _is_thread_essential()) {
                printk("Fatal fault in %s! Spinning...\n",
                       k_is_in_isr() ? "ISR" : "essential thread");
                /* spin forever */
                for (;;)
                        __asm__ volatile("nop");
        }
        printk("Fatal fault in thread! Aborting.\n");
        k_thread_abort(_current);
#else
        for (;;)
                k_cpu_idle();

#endif

        CODE_UNREACHABLE;
}

/**
 *
 * @brief Kernel fatal error handler
 *
 * This routine is called when a fatal error condition is detected by either
 * hardware or software.
 *
 * The caller is expected to always provide a usable ESF.  In the event that the
 * fatal error does not have a hardware generated ESF, the caller should either
 * create its own or use a pointer to the global default ESF <_default_esf>.
 *
 * @param reason the reason that the handler was called
 * @param pEsf pointer to the exception stack frame
 *
 * @return This function does not return.
 */
FUNC_NORETURN void _NanoFatalErrorHandler(unsigned int reason,
					  const NANO_ESF *pEsf)
{
	/* TODO: fill me */

#ifdef CONFIG_PRINTK
	/* Display diagnostic information about the error */
	printk("***** Fatal error %d! *****\n", reason);
#endif


	/*
	 * Error was fatal to a kernel task or a fiber; invoke the system
	 * fatal error handling policy defined for the platform.
	 */
	_SysFatalErrorHandler(reason, pEsf);
}

#if 0

#if CONFIG_EXCEPTION_DEBUG

static FUNC_NORETURN void generic_exc_handle(unsigned int vector,
					     const NANO_ESF *pEsf)
{
	printk("***** CPU exception %d\n", vector);
	if ((1 << vector) & _EXC_ERROR_CODE_FAULTS) {
		printk("***** Exception code: 0x%x\n", pEsf->errorCode);
	}
	_NanoFatalErrorHandler(_NANO_ERR_CPU_EXCEPTION, pEsf);
}

#define _EXC_FUNC(vector) \
FUNC_NORETURN void handle_exc_##vector(const NANO_ESF *pEsf) \
{ \
	generic_exc_handle(vector, pEsf); \
}

#define _EXC_FUNC_CODE(vector) \
	_EXC_FUNC(vector) \
	_EXCEPTION_CONNECT_CODE(handle_exc_##vector, vector)

#define _EXC_FUNC_NOCODE(vector) \
	_EXC_FUNC(vector) \
	_EXCEPTION_CONNECT_NOCODE(handle_exc_##vector, vector)

/* Necessary indirection to ensure 'vector' is expanded before we expand
 * the handle_exc_##vector
 */
#define EXC_FUNC_NOCODE(vector) \
	_EXC_FUNC_NOCODE(vector)

#define EXC_FUNC_CODE(vector) \
	_EXC_FUNC_CODE(vector)

EXC_FUNC_NOCODE(IV_DIVIDE_ERROR);
EXC_FUNC_NOCODE(IV_NON_MASKABLE_INTERRUPT);
EXC_FUNC_NOCODE(IV_OVERFLOW);
EXC_FUNC_NOCODE(IV_BOUND_RANGE);
EXC_FUNC_NOCODE(IV_INVALID_OPCODE);
EXC_FUNC_NOCODE(IV_DEVICE_NOT_AVAILABLE);
EXC_FUNC_CODE(IV_DOUBLE_FAULT);
EXC_FUNC_CODE(IV_INVALID_TSS);
EXC_FUNC_CODE(IV_SEGMENT_NOT_PRESENT);
EXC_FUNC_CODE(IV_STACK_FAULT);
EXC_FUNC_CODE(IV_GENERAL_PROTECTION);
EXC_FUNC_CODE(IV_PAGE_FAULT);
EXC_FUNC_NOCODE(IV_X87_FPU_FP_ERROR);
EXC_FUNC_CODE(IV_ALIGNMENT_CHECK);
EXC_FUNC_NOCODE(IV_MACHINE_CHECK);

#endif /* CONFIG_EXCEPTION_DEBUG */

#endif
