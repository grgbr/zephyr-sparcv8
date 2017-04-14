#include <ksched.h>
#include <kernel_structs.h>

#if defined(CONFIG_THREAD_MONITOR)
/*
 * Add a thread to the kernel's list of active threads.
 */
static inline void thread_monitor_init(struct k_thread *thread)
{
	unsigned int key;

	key = irq_lock();

	thread->next_thread = _kernel.threads;
	_kernel.threads = thread;

	irq_unlock(key);
}

#else  /* !CONFIG_THREAD_MONITOR */
#define thread_monitor_init(_thread) do {} while (0)
#endif /* !CONFIG_THREAD_MONITOR */

struct init_stack_frame {
	/* top of the stack / most recently pushed */

	/* Used by _thread_entry_wrapper. pulls these off the stack and
	 * into argument registers before calling _thread_entry()
	 */
	_thread_entry_t  entry_point;
	void            *arg1;
	void            *arg2;
	void            *arg3;

	/* least recently pushed */
};

extern void _thread_entry_wrapper(_thread_entry_t thread,
				  void *arg1,
				  void *arg2,
				  void *arg3);


void _new_thread(char *stack_memory, size_t stack_size,
		 _thread_entry_t thread_func,
		 void *arg1, void *arg2, void *arg3,
		 int priority, unsigned options)
{
	_ASSERT_VALID_PRIO(priority, thread_func);

	__ASSERT(!((uint32_t)stack_memory & (STACK_ALIGN - 1)),
		 "stack is not aligned properly\n"
		 "%d-byte alignment required\n", STACK_ALIGN);


	struct k_thread         *thread;
	struct init_stack_frame *stack_init;

#ifdef CONFIG_INIT_STACKS
	memset(stack_memory, 0xaa, stack_size);
#endif
	/* Initial stack frame for thread */
	stack_init = (struct init_stack_frame *)
		     STACK_ROUND_DOWN(stack_memory + stack_size -
				      sizeof(struct init_stack_frame));

	/* Setup the initial stack frame */
	stack_init->entry_point = thread_func;
	stack_init->arg1 = arg1;
	stack_init->arg2 = arg2;
	stack_init->arg3 = arg3;

	/* Initialize various struct k_thread members */
	thread = (struct k_thread *)stack_memory;

	_init_thread_base(&thread->base, priority, _THREAD_PRESTART, options);

	/* static threads overwrite it afterwards with real value */
	thread->init_data = NULL;
	thread->fn_abort = NULL;

#ifdef CONFIG_THREAD_CUSTOM_DATA
	/* Initialize custom data field (value is opaque to kernel) */
	thread->custom_data = NULL;
#endif

	/* thread->callee_saved.sp = (uint32_t)stack_init;*/
	thread->callee_saved.sp = (uint32_t)stack_init;
	thread->callee_saved.pc = (uint32_t)_thread_entry_wrapper - 8U;
	thread->callee_saved.psr = _rd_psr() & ~(PSR_ICC_MASK | PSR_PIL_MASK);

	thread_monitor_init(thread);
}
