#
# Copyright 2009 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile template for BeRTOS wizard.
#
# Author: Lorenzo Berni <duplo@develer.com>
#
#

# Constants automatically defined by the selected modules


# Our target application
TRG += kernel-only_arm

kernel-only_arm_PREFIX = "arm-none-eabi-"

kernel-only_arm_SUFFIX = ""

# Files automatically generated by the wizard. DO NOT EDIT, USE kernel-only_arm_USER_CSRC INSTEAD!
kernel-only_arm_WIZARD_CSRC = \
	bertos/kern/proc.c \
	bertos/kern/sem.c \
	bertos/kern/signal.c \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE kernel-only_arm_USER_PCSRC INSTEAD!
kernel-only_arm_WIZARD_PCSRC = \
	 \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE kernel-only_arm_USER_CPPASRC INSTEAD!
kernel-only_arm_WIZARD_CPPASRC = \
	bertos/cpu/arm/hw/switch_ctx_arm.S \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE kernel-only_arm_USER_CXXSRC INSTEAD!
kernel-only_arm_WIZARD_CXXSRC = \
	 \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE kernel-only_arm_USER_ASRC INSTEAD!
kernel-only_arm_WIZARD_ASRC = \
	 \
	#

kernel-only_arm_CPPFLAGS = -D'CPU_FREQ=(48023000UL)' -D'ARCH=(ARCH_DEFAULT)' -D'WIZ_AUTOGEN' -Iexamples/benchmark/kernel-only_arm/ $(kernel-only_arm_CPU_CPPFLAGS) $(kernel-only_arm_USER_CPPFLAGS)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
kernel-only_arm_LDFLAGS = $(kernel-only_arm_CPU_LDFLAGS) $(kernel-only_arm_WIZARD_LDFLAGS) $(kernel-only_arm_USER_LDFLAGS)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
kernel-only_arm_CPPAFLAGS = $(kernel-only_arm_CPU_CPPAFLAGS) $(kernel-only_arm_WIZARD_CPPAFLAGS) $(kernel-only_arm_USER_CPPAFLAGS)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
kernel-only_arm_CSRC = $(kernel-only_arm_CPU_CSRC) $(kernel-only_arm_WIZARD_CSRC) $(kernel-only_arm_USER_CSRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
kernel-only_arm_PCSRC = $(kernel-only_arm_CPU_PCSRC) $(kernel-only_arm_WIZARD_PCSRC) $(kernel-only_arm_USER_PCSRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
kernel-only_arm_CPPASRC = $(kernel-only_arm_CPU_CPPASRC) $(kernel-only_arm_WIZARD_CPPASRC) $(kernel-only_arm_USER_CPPASRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
kernel-only_arm_CXXSRC = $(kernel-only_arm_CPU_CXXSRC) $(kernel-only_arm_WIZARD_CXXSRC) $(kernel-only_arm_USER_CXXSRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
kernel-only_arm_ASRC = $(kernel-only_arm_CPU_ASRC) $(kernel-only_arm_WIZARD_ASRC) $(kernel-only_arm_USER_ASRC)

# CPU specific flags and options, defined in the CPU definition files.
# Automatically generated by the wizard. PLEASE DO NOT EDIT!
kernel-only_arm_CPU_CPPAFLAGS = -O0 -g -gdwarf-2 -g
kernel-only_arm_CPU_CPPFLAGS = -O0 -g3 -gdwarf-2 -fverbose-asm -Ibertos/cpu/arm/ -D__ARM_AT91SAM7X256__
kernel-only_arm_PROGRAMMER_CPU = at91sam7
kernel-only_arm_STOPFLASH_SCRIPT = bertos/prg_scripts/arm/stopopenocd.sh
kernel-only_arm_CPU = arm7tdmi
kernel-only_arm_STOPDEBUG_SCRIPT = bertos/prg_scripts/arm/stopopenocd.sh
kernel-only_arm_CPU_CPPASRC = bertos/cpu/arm/hw/crtat91sam7_rom.S
kernel-only_arm_DEBUG_SCRIPT = bertos/prg_scripts/arm/debug.sh
kernel-only_arm_CPU_LDFLAGS = -nostartfiles -Wl,--no-warn-mismatch -T bertos/cpu/arm/scripts/at91sam7_256_rom.ld
kernel-only_arm_FLASH_SCRIPT = bertos/prg_scripts/arm/flash.sh
