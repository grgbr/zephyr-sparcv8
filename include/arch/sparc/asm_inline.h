#ifndef _ASM_INLINE_H
#define _ASM_INLINE_H

#if !defined(CONFIG_SPARC)
#error arch/sparc/include/asm_inline.h is for SPARC CPUs only
#endif

/*
 * The file must not be included directly
 * Include arch/cpu.h instead
 */

#if defined(__GNUC__)
#include <arch/sparc/asm_inline_gcc.h>
#else
#error "Supports only GNU C compiler"
#endif

#endif
