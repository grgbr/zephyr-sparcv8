#include <misc/printk.h>

void _irq_spurious(void *unused)
{
	ARG_UNUSED(unused);

	/* Complete me ! */
	 printk("Spurious interrupt detected!\n");
}
