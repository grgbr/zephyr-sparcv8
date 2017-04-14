KBUILD_CFLAGS += $(call cc-option,-ffunction-sections,) \
                 $(call cc-option,-fdata-sections,)

ifeq ($(CONFIG_DEBUG), y)
KBUILD_CFLAGS += -fvar-tracking -fvar-tracking-assignments -enable-frame-pointer
endif

include $(srctree)/arch/$(ARCH)/soc/$(SOC_PATH)/Makefile

cflags-$(CONFIG_LTO) += $(call cc-option,-flto,)

cflags-leon2 = $(call cc-option,-mcpu=leon -msoft-float -mno-user-mode -mflat)
cflags-leon3 = $(call cc-option,-mcpu=leon3 -msoft-float -mno-user-mode -mflat)

ifeq ($(CONFIG_FLOAT), y)
$(error SPARC hardware floating point operations not yet supported)
endif

aflags-$(CONFIG_CPU_LEON2) += $(cflags-leon2)
cflags-$(CONFIG_CPU_LEON2) += $(cflags-leon2)
cxxflags-$(CONFIG_CPU_LEON2) += $(cflags-leon2)

aflags-$(CONFIG_CPU_LEON3) += $(cflags-leon3)
cflags-$(CONFIG_CPU_LEON3) += $(cflags-leon3)
cxxflags-$(CONFIG_CPU_LEON3) += $(cflags-leon3)

KBUILD_AFLAGS += $(aflags-y)
KBUILD_CFLAGS += $(cflags-y)
KBUILD_CXXFLAGS += $(cflags-y)

soc-cxxflags ?= $(soc-cflags)
soc-aflags ?= $(soc-cflags)
KBUILD_CFLAGS += $(soc-cflags)
KBUILD_CXXFLAGS += $(soc-cxxflags)
KBUILD_AFLAGS += $(soc-aflags)
