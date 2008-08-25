/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2001, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001, 2008 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief Process scheduler (public interface).
 *
 * \version $Id$
 * \author Bernie Innocenti <bernie@codewiz.org>
 */
#ifndef KERN_PROC_H
#define KERN_PROC_H

#include "cfg/cfg_kern.h"
#include <cfg/compiler.h>

#if CONFIG_KERN_PREEMPT
	#include <cfg/debug.h> // ASSERT()
#endif

#include <cpu/types.h> // cpustack_t

/*
 * Forward declaration. The definition of struct Process is private to the
 * scheduler and hidden in proc_p.h.
 */
struct Process;

/* Task scheduling services */
void proc_init(void);
struct Process *proc_new_with_name(const char* name, void (*entry)(void), iptr_t data, size_t stacksize, cpustack_t *stack);

#if !CONFIG_KERN_MONITOR
	#define proc_new(entry,data,size,stack) proc_new_with_name(NULL,(entry),(data),(size),(stack))
#else
	#define proc_new(entry,data,size,stack) proc_new_with_name(#entry,(entry),(data),(size),(stack))
#endif

void proc_exit(void);
void proc_yield(void);
#define proc_switch proc_yield /* OBSOLETE */

int proc_testSetup(void);
int proc_testRun(void);
int proc_testTearDown(void);

struct Process *proc_current(void);
iptr_t proc_currentUserData(void);
void proc_rename(struct Process *proc, const char *name);
const char *proc_name(struct Process *proc);
const char *proc_currentName(void);

#if CONFIG_KERN_PRI
	void proc_setPri(struct Process *proc, int pri);
#else
	INLINE void proc_setPri(UNUSED_ARG(struct Process *,proc), UNUSED_ARG(int, pri))
	{
	}
#endif

/**
 * Disable preemptive task switching.
 *
 * The scheduler maintains a global nesting counter.  Task switching is
 * effectively re-enabled only when the number of calls to proc_permit()
 * matches the number of calls to proc_forbid().
 *
 * \note Calling functions that could sleep while task switching is disabled
 * is dangerous and unsupported.
 *
 * \note proc_permit() expands inline to 1-2 asm instructions, so it's a
 * very efficient locking primitive in simple but performance-critical
 * situations.  In all other cases, semaphores offer a more flexible and
 * fine-grained locking primitive.
 *
 * \sa proc_permit()
 */
INLINE void proc_forbid(void)
{
	#if CONFIG_KERN_PREEMPT
		// No need to protect against interrupts here.
		extern int preempt_forbid_cnt;
		++preempt_forbid_cnt;

		/*
		 * Make sure preempt_forbid_cnt is flushed to memory so the
		 * preemption softirq will see the correct value from now on.
		 */
		MEMORY_BARRIER;
	#endif
}

/**
 * Re-enable preemptive task switching.
 *
 * \sa proc_forbid()
 */
INLINE void proc_permit(void)
{
	#if CONFIG_KERN_PREEMPT

		/*
		 * This is to ensure any global state changed by the process gets
		 * flushed to memory before task switching is re-enabled.
		 */
		MEMORY_BARRIER;

		/* No need to protect against interrupts here. */
		extern int preempt_forbid_cnt;
		--preempt_forbid_cnt;
		ASSERT(preempt_forbid_cnt >= 0);

		/*
		 * This ensures preempt_forbid_cnt is flushed to memory immediately
		 * so the preemption interrupt sees the correct value.
		 */
		MEMORY_BARRIER;

	#endif
}


/**
 * Execute a block of \a CODE atomically with respect to task scheduling.
 */
#define PROC_ATOMIC(CODE) \
	do { \
		proc_forbid(); \
		CODE; \
		proc_permit(); \
	} while(0)

#ifndef CONFIG_KERN_MINSTACKSIZE

	#if (ARCH & ARCH_EMUL)
		/* We need a large stack because system libraries are bloated */
		#define CONFIG_KERN_MINSTACKSIZE  65536
	#else
		/**
		 * Default stack size for each thread, in bytes.
		 *
		 * The goal here is to allow a minimal task to save all of its
		 * registers twice, plus push a maximum of 32 variables on the
		 * stack.
		 *
		 * The actual size computed by the default formula is:
		 *   AVR:    102
		 *   i386:   156
		 *   ARM:    164
		 *   x86_64: 184
		 *
		 * Note that on most 16bit architectures, interrupts will also
		 * run on the stack of the currently running process.  Nested
		 * interrupts will greatly increases the amount of stack space
		 * required per process.  Use irqmanager to minimize stack
		 * usage.
		 */
		#define CONFIG_KERN_MINSTACKSIZE  \
		    (CPU_SAVED_REGS_CNT * 2 * sizeof(cpustack_t) \
		    + 32 * sizeof(int))
	#endif
#endif

#define CONFIG_PROC_DEFSTACKSIZE CONFIG_KERN_MINSTACKSIZE // OBSOLETE

/* Memory fill codes to help debugging */
#if CONFIG_KERN_MONITOR
	#include <cpu/types.h>
	#if (SIZEOF_CPUSTACK_T == 1)
		/* 8bit cpustack_t */
		#define CONFIG_KERN_STACKFILLCODE  0xA5
		#define CONFIG_KERN_MEMFILLCODE    0xDB
	#elif (SIZEOF_CPUSTACK_T == 2)
		/* 16bit cpustack_t */
		#define CONFIG_KERN_STACKFILLCODE  0xA5A5
		#define CONFIG_KERN_MEMFILLCODE    0xDBDB
	#elif (SIZEOF_CPUSTACK_T == 4)
		/* 32bit cpustack_t */
		#define CONFIG_KERN_STACKFILLCODE  0xA5A5A5A5UL
		#define CONFIG_KERN_MEMFILLCODE    0xDBDBDBDBUL
	#elif (SIZEOF_CPUSTACK_T == 8)
		/* 64bit cpustack_t */
		#define CONFIG_KERN_STACKFILLCODE  0xA5A5A5A5A5A5A5A5ULL
		#define CONFIG_KERN_MEMFILLCODE    0xDBDBDBDBDBDBDBDBULL
	#else
		#error No cpustack_t size supported!
	#endif
#endif

#endif /* KERN_PROC_H */
