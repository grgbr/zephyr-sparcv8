#include <irq.h>

void k_cpu_idle(void)
{
	irq_unlock(0);
}
