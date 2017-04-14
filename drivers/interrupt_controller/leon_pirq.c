#include <init.h>
#include <inttypes.h>
#include <device.h>
#include <irq.h>
#include <soc.h>
#include <misc/__assert.h>

#define LEON_PIRQ_MASK_PRIO (LEON_PIRQ_BASE + 0)
#define LEON_PIRQ_PEND      (LEON_PIRQ_BASE + 4)
#define LEON_PIRQ_FORCE     (LEON_PIRQ_BASE + 8)
#define LEON_PIRQ_CLEAR     (LEON_PIRQ_BASE + 0xc)

static void leon_pirq_enable(unsigned int irq)
{
	unsigned int key;
	uint32_t     msk = 1U << irq;

	__ASSERT((irq > 0) && (irq <= LEON_PIRQ_NR), "invalid interrupt line");

	key = irq_lock();

	/* Clear interrupt's pending status. */
	sys_write32(msk, LEON_PIRQ_CLEAR);

	/* Unmask interrupt. */
	sys_write32(sys_read32(LEON_PIRQ_MASK_PRIO) | msk, LEON_PIRQ_MASK_PRIO);

	irq_unlock(key);
}

static void leon_pirq_disable(unsigned int irq)
{
	unsigned int key;

	__ASSERT((irq > 0) && (irq <= LEON_PIRQ_NR), "invalid interrupt line");

	key = irq_lock();

	/* Mask interrupt. */
	sys_write32(sys_read32(LEON_PIRQ_MASK_PRIO) & ~(1U << irq),
		    LEON_PIRQ_MASK_PRIO);

	irq_unlock(key);
}

static int leon_pirq_is_enabled(unsigned int irq)
{
	__ASSERT((irq > 0) && (irq <= LEON_PIRQ_NR), "invalid interrupt line");

	/* Return current interrupt's masked state. */
	return !!(sys_read32(LEON_PIRQ_MASK_PRIO) & (1U << irq));
}

static int leon_pirq_init(struct device *unused)
{
	ARG_UNUSED(unused);

	/* Mask all interrupts. */
	sys_write32(0, LEON_PIRQ_MASK_PRIO);

	/* Disable all forced interrupts. */
	sys_write32(0, LEON_PIRQ_FORCE);

	/* Clear all pending interrupts. */
	sys_write32(((1U << LEON_PIRQ_NR) - 1) << 1, LEON_PIRQ_CLEAR);

	return 0;
}
SYS_INIT(leon_pirq_init, PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

void _arch_irq_enable(unsigned int irq)
{
        leon_pirq_enable(irq);
}

void _arch_irq_disable(unsigned int irq)
{
        leon_pirq_disable(irq);
}

int _arch_irq_is_enabled(unsigned int irq)
{
	return leon_pirq_is_enabled(irq);
}
