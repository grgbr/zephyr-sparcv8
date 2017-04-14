/*
 * TODO: implement support for
 *     - CONFIG_TIMER_READS_ITS_FREQUENCY_AT_RUNTIME
 *     - CONFIG_TICKLESS_IDLE
 *     - CONFIG_SYS_POWER_MANAGEMENT
 *     - CONFIG_SYSTEM_CLOCK_DISABLE
 *     - CONFIG_KERNEL_EVENT_LOGGER_INTERRUPT
 *     - remove uneeded headers
 */

#include <kernel.h>
#include <arch/cpu.h>
#include <device.h>
#include <system_timer.h>
#include <board.h>

#if defined(CONFIG_TIMER_READS_ITS_FREQUENCY_AT_RUNTIME)
#error CONFIG_TIMER_READS_ITS_FREQUENCY_AT_RUNTIME not supported yet !
#endif
#if defined(CONFIG_TICKLESS_IDLE)
#error CONFIG_TICKLESS_IDLE not supported yet !
#endif
#if defined(CONFIG_SYS_POWER_MANAGEMENT)
#error CONFIG_SYS_POWER_MANAGEMENT not supported yet !
#endif
#if defined(CONFIG_SYSTEM_CLOCK_DISABLE)
#error CONFIG_SYSTEM_CLOCK_DISABLE not supported yet !
#endif
#if defined(CONFIG_KERNEL_EVENT_LOGGER_INTERRUPT)
#error CONFIG_KERNEL_EVENT_LOGGER_INTERRUPT not supported yet !
#endif

/* Timer counter register */
#define LEON_TIMER_CNT     0

/* Timer reload register */
#define LEON_TIMER_RL      4

/* Timer control register */
#define LEON_TIMER_CTRL    8
#define LEON_TIMER_CTRL_EN (1U << 0)
#define LEON_TIMER_CTRL_RL (1U << 1)
#define LEON_TIMER_CTRL_LD (1u << 2)


static uint32_t leon_sys_cycles;

static void leon_handle_timer_irq(void *unused)
{
	ARG_UNUSED(unused);

	leon_sys_cycles += sys_clock_hw_cycles_per_tick;

	_sys_clock_tick_announce();
}

uint32_t _timer_cycle_get_32(void)
{
	uint32_t cycles, count;

	do {
		cycles = leon_sys_cycles;
		count = sys_clock_hw_cycles_per_tick -
			sys_read32(LEON_TIMER1_BASE + LEON_TIMER_CNT);
	} while (cycles != leon_sys_cycles);

	return cycles + count;
}

int _sys_clock_driver_init(struct device *device)
{
	ARG_UNUSED(device);

	IRQ_CONNECT(LEON_TIMER1_IRQ, 0, leon_handle_timer_irq, NULL, 0);

	__ASSERT(sys_clock_hw_cycles_per_tick <= (1U << 24),
		 "sys_clock_hw_cycles_per_tick too large");

	sys_write32(sys_clock_hw_cycles_per_tick - 1,
		    LEON_TIMER1_BASE + LEON_TIMER_RL);

	sys_write32(LEON_TIMER_CTRL_LD | LEON_TIMER_CTRL_RL |
		    LEON_TIMER_CTRL_EN,
		    LEON_TIMER1_BASE + LEON_TIMER_CTRL);

	irq_enable(LEON_TIMER1_IRQ);

	return 0;
}
