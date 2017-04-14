#include <kernel.h>
#include <arch/cpu.h>
#include <uart.h>
#include <board.h>

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
#error "Leon interrupt driven UART not yet supported"
#endif

#define LEON_UART_DATA       (0U)
#define LEON_UART_STATUS     (4U)
#define LEON_UART_STATUS_TH  (1U << 2)
#define LEON_UART_CTRL       (8U)
#define LEON_UART_CTRL_TE    (1U << 1) /* Transmit enable */
#define LEON_UART_SCAL       (0xcU)

static unsigned char leon_pollout_uart(struct device *dev, unsigned char c)
{
	const struct uart_device_config *cfg = (struct uart_device_config *)
	                                       dev->config->config_info;

	/* Wait till ready to transmit */
	while (!(sys_read8(cfg->regs + LEON_UART_STATUS) & LEON_UART_STATUS_TH))
		;

	/* Fire ! */
	sys_write8(c, cfg->regs + LEON_UART_DATA);

	return c;
}

static int leon_init_uart(struct device *dev)
{
	const struct uart_device_config *cfg = (struct uart_device_config *)
	                                       dev->config->config_info;

	/* TODO: setup scaler / baud rate... */
	sys_write8(LEON_UART_CTRL_TE, cfg->regs + LEON_UART_CTRL);

	return 0;
}

static const struct uart_driver_api leon_uart_drv = {
	.poll_out  = leon_pollout_uart,
};

static const struct uart_device_config leon_uart_cfg1 = {
	.regs = LEON_UART1_BASE
};

DEVICE_AND_API_INIT(leon_uart1, "uart1", &leon_init_uart, NULL, &leon_uart_cfg1,
	    PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEVICE, &leon_uart_drv);
