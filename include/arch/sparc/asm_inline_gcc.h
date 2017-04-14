#ifndef _ASM_INLINE_GCC_H
#define _ASM_INLINE_GCC_H

#include <stddef.h>
#include <stdint.h>
#include <sys_io.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 * The file must not be included directly
 * Include arch/cpu.h instead
 */

#ifndef _ASMLANGUAGE

static inline unsigned int _ctz(uint32_t op)
{
	unsigned int b = 0;

	if (!(op & 0xffffU)) {
		b += 16;
		op >>= 16;
	}
	if (!(op & 0xffU)) {
		b += 8;
		op >>= 8;
	}
	if (!(op & 0xfU)) {
		b += 4;
		op >>= 4;
	}
	if (!(op & 0x3U)) {
		b += 2;
		op >>= 2;
	}

	return b + (op & 1);
}

static inline unsigned int _clz(uint32_t op)
{
	unsigned int b = 0;

	if (!(op & 0xffff0000U)) {
		b += 16;
		op <<= 16;
	}
	if (!(op & 0xff000000U)) {
		b += 8;
		op <<= 8;
	}
	if (!(op & 0xf0000000U)) {
		b += 4;
		op <<= 4;
	}
	if (!(op & 0xc0000000U)) {
		b += 2;
		op <<= 2;
	}

	return b + !(op & 0x80000000);
}

/**
 *
 * @brief find least significant bit set in a 32-bit word
 *
 * This routine finds the first bit set starting from the least significant bit
 * in the argument passed in and returns the index of that bit.  Bits are
 * numbered starting at 1 from the least significant bit.  A return value of
 * zero indicates that the value passed is zero.
 *
 * @return least significant bit set, 0 if @a op is 0
 */
static inline unsigned int find_lsb_set(uint32_t op)
{
	if (!op)
		return 0;

	return 32 - _ctz(op);
}

/**
 *
 * @brief find most significant bit set in a 32-bit word
 *
 * This routine finds the first bit set starting from the most significant bit
 * in the argument passed in and returns the index of that bit.  Bits are
 * numbered starting at 1 from the least significant bit.  A return value of
 * zero indicates that the value passed is zero.
 *
 * @return most significant bit set, 0 if @a op is 0
 */
static inline unsigned int find_msb_set(uint32_t op)
{
	if (!op)
		return 0;

	return 32 - _clz(op);
}

static inline uint32_t sys_read32(mem_addr_t addr)
{
	return *(volatile uint32_t *)addr;
}


static inline void sys_write32(uint32_t data, mem_addr_t addr)
{
	*(volatile uint32_t *)addr = data;
}

static inline uint8_t sys_read8(mem_addr_t addr)
{
        return *(volatile uint8_t *)addr;
}

static inline void sys_write8(uint8_t data, mem_addr_t addr)
{
        *(volatile uint8_t *)addr = data;
}

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif
