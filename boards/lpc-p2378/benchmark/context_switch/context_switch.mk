#
# Wizard autogenerated makefile.
# DO NOT EDIT, use the context_switch_user.mk file instead.
#

# Constants automatically defined by the selected modules


# Our target application
TRG += context_switch

context_switch_PREFIX = "arm-none-eabi-"

context_switch_SUFFIX = ""

context_switch_SRC_PATH = boards/ek-lpc-p2378/benchmark/context_switch

context_switch_HW_PATH = boards/ek-lpc-p2378

# Files automatically generated by the wizard. DO NOT EDIT, USE context_switch_USER_CSRC INSTEAD!
context_switch_WIZARD_CSRC = \
	bertos/benchmark/context_switch.c \
	bertos/kern/signal.c \
	bertos/kern/kfile.c \
	bertos/cpu/arm/drv/timer_lpc2.c \
	bertos/mware/formatwr.c \
	bertos/struct/heap.c \
	bertos/cpu/arm/drv/ser_arm.c \
	bertos/cpu/arm/drv/timer_arm.c \
	bertos/drv/timer.c \
	bertos/mware/event.c \
	bertos/kern/proc.c \
	bertos/cpu/arm/drv/ser_lpc2.c \
	bertos/drv/ser.c \
	bertos/mware/hex.c \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE context_switch_USER_PCSRC INSTEAD!
context_switch_WIZARD_PCSRC = \
	 \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE context_switch_USER_CPPASRC INSTEAD!
context_switch_WIZARD_CPPASRC = \
	bertos/cpu/arm/hw/switch_ctx_arm.S \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE context_switch_USER_CXXSRC INSTEAD!
context_switch_WIZARD_CXXSRC = \
	 \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE context_switch_USER_ASRC INSTEAD!
context_switch_WIZARD_ASRC = \
	 \
	#

context_switch_CPPFLAGS = -D'CPU_FREQ=(72000000UL)' -D'ARCH=(ARCH_DEFAULT)' -D'WIZ_AUTOGEN' -I$(context_switch_HW_PATH) -I$(context_switch_SRC_PATH) $(context_switch_CPU_CPPFLAGS) $(context_switch_USER_CPPFLAGS)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
context_switch_LDFLAGS = $(context_switch_CPU_LDFLAGS) $(context_switch_WIZARD_LDFLAGS) $(context_switch_USER_LDFLAGS)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
context_switch_CPPAFLAGS = $(context_switch_CPU_CPPAFLAGS) $(context_switch_WIZARD_CPPAFLAGS) $(context_switch_USER_CPPAFLAGS)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
context_switch_CSRC = $(context_switch_CPU_CSRC) $(context_switch_WIZARD_CSRC) $(context_switch_USER_CSRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
context_switch_PCSRC = $(context_switch_CPU_PCSRC) $(context_switch_WIZARD_PCSRC) $(context_switch_USER_PCSRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
context_switch_CPPASRC = $(context_switch_CPU_CPPASRC) $(context_switch_WIZARD_CPPASRC) $(context_switch_USER_CPPASRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
context_switch_CXXSRC = $(context_switch_CPU_CXXSRC) $(context_switch_WIZARD_CXXSRC) $(context_switch_USER_CXXSRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
context_switch_ASRC = $(context_switch_CPU_ASRC) $(context_switch_WIZARD_ASRC) $(context_switch_USER_ASRC)

# CPU specific flags and options, defined in the CPU definition files.
# Automatically generated by the wizard. PLEASE DO NOT EDIT!
context_switch_CPU_CPPASRC = bertos/cpu/arm/hw/crt_arm7tdmi.S bertos/cpu/arm/hw/vectors_lpc2.S
context_switch_CPU_CPPAFLAGS = -g -gdwarf-2
context_switch_CPU_CPPFLAGS = -O0 -g3 -gdwarf-2 -fverbose-asm -Ibertos/cpu/arm/ -D__ARM_LPC2378__
context_switch_CPU_CSRC = bertos/cpu/arm/hw/init_lpc2.c bertos/cpu/arm/drv/vic_lpc2.c
context_switch_PROGRAMMER_CPU = lpc2378
context_switch_STOPFLASH_SCRIPT = bertos/prg_scripts/arm/stopopenocd.sh
context_switch_CPU = arm7tdmi
context_switch_STOPDEBUG_SCRIPT = bertos/prg_scripts/arm/stopopenocd.sh
context_switch_DEBUG_SCRIPT = bertos/prg_scripts/arm/debug.sh
context_switch_CPU_LDFLAGS = -nostartfiles -Wl,--no-warn-mismatch -T bertos/cpu/arm/scripts/lpc2378.ld
context_switch_FLASH_SCRIPT = bertos/prg_scripts/arm/flash.sh

include $(context_switch_SRC_PATH)/context_switch_user.mk
