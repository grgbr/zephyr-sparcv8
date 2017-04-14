#define _LINKER
#define _ASMLANGUAGE

#include <autoconf.h>
#include <sections.h>
#include <linker-defs.h>
#include <linker-tool.h>

#ifdef CONFIG_XIP
  #define ROMABLE_REGION ROM
  #define RAMABLE_REGION SYSRAM
#else
  #define ROMABLE_REGION SYSRAM
  #define RAMABLE_REGION SYSRAM
#endif

ENTRY(_reset)

MEMORY
{
    ROM       (rx)  : ORIGIN = 0x00000000, LENGTH = 64K
    SYSRAM    (rwx) : ORIGIN = 0x70000000, LENGTH = 640K
    /* Used by and documented in include/linker/intlist.ld */
    IDT_LIST  (wx)  : ORIGIN = 0xFFFFF7FF, LENGTH = 2K
}

SECTIONS
{
	GROUP_START(ROMABLE_REGION)

	_image_rom_start = .;

	SECTION_PROLOGUE(_TEXT_SECTION_NAME,,)
	{
		. = ALIGN(4K);
		KEEP(*(.trap))
		KEEP(*(".trap.*"))

		_image_text_start = ALIGN(4);
		*(.text)
		*(".text.*")
		*(.gnu.linkonce.t.*)
	} GROUP_LINK_IN(ROMABLE_REGION)

	_image_text_end = .;

#include <linker/common-rom.ld>

	SECTION_PROLOGUE(_RODATA_SECTION_NAME,,)
	{
		. = ALIGN(4);
		*(.rodata)
		*(".rodata.*")
		*(.gnu.linkonce.r.*)
	} GROUP_LINK_IN(ROMABLE_REGION)

	_image_rom_end = .;

	__data_rom_start = ALIGN(8);

	GROUP_END(ROMABLE_REGION)


	GROUP_START(RAMABLE_REGION)

	SECTION_DATA_PROLOGUE(_DATA_SECTION_NAME,,)
	{
		_image_ram_start = .;
		__data_ram_start = .;

#ifdef CONFIG_GEN_SW_ISR_TABLE
		KEEP(*(SW_ISR_TABLE))
#endif
		*(.data)
		*(".data.*")
		*(.sdata .sdata.* .gnu.linkonce.s.*)
		*(.sdata2 .sdata2.* .gnu.linkonce.s2.*)
	} GROUP_DATA_LINK_IN(RAMABLE_REGION, ROMABLE_REGION)

#include <linker/common-ram.ld>

	__data_ram_end = .;

	SECTION_DATA_PROLOGUE(_BSS_SECTION_NAME,(NOLOAD),)
	{
		. = ALIGN(8);
		__bss_start = .;
		*(.sbss)
		*(".sbss.*")
		*(.bss)
		*(".bss.*")
		COMMON_SYMBOLS
	}  GROUP_LINK_IN(RAMABLE_REGION)

	__bss_end = ALIGN(8);

	SECTION_PROLOGUE(_NOINIT_SECTION_NAME,(NOLOAD),)
	{
		*(.noinit)
		*(".noinit.*")
	} GROUP_LINK_IN(RAMABLE_REGION)

	_image_ram_end = .;
	_end = .;

	GROUP_END(RAMABLE_REGION)

#ifdef CONFIG_GEN_ISR_TABLES
#include <linker/intlist.ld>
#endif
}
