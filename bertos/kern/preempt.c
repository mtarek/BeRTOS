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
 * \brief Simple realtime multitasking scheduler.
 *        Context switching is only done cooperatively.
 *
 * \version $Id: proc.c 1616 2008-08-10 19:41:26Z bernie $
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#include "proc_p.h"
#include "proc.h"

#include <cpu/frame.h> // CPU_IDLE

#include <unistd.h> // XXX alarm()


/*
 * The time sharing scheduler forces a task switch when the current
 * process has exhausted its quantum.
 */
uint16_t Quantum;

/**
 * Disable preemptive task switching.
 *
 * The scheduler maintains a per-process nesting counter.  Task switching is
 * effectively re-enabled only when the number of calls to proc_permit()
 * matches the number of calls to proc_forbid().
 *
 * Calling functions that could sleep while task switching is disabled
 * is dangerous, although supported.  Preemptive task switching is
 * resumed while the process is sleeping and disabled again as soon as
 * it wakes up again.
 *
 * \sa proc_permit()
 */
void proc_forbid(void)
{
	/* No need to protect against interrupts here. */
	++CurrentProcess->forbid_cnt;
}

/**
 * Re-enable preemptive task switching.
 *
 * \sa proc_forbid()
 */
void proc_permit(void)
{
	/* No need to protect against interrupts here. */
	--CurrentProcess->forbid_cnt;
}

static void (*irq_handlers[100])(void); // FIXME


void proc_preempt(void)
{
	TRACE;

	ATOMIC(LIST_ASSERT_VALID(&ProcReadyList));

	IRQ_DISABLE;
	/* Poll on the ready queue for the first ready process */
	while (!(CurrentProcess = (struct Process *)list_remHead(&ProcReadyList)))
	{
		/*
		 * Make sure we physically reenable interrupts here, no matter what
		 * the current task status is. This is important because if we
		 * are idle-spinning, we must allow interrupts, otherwise no
		 * process will ever wake up.
		 *
		 * During idle-spinning, an interrupt can occur and it may
		 * modify \p ProcReadyList. To ensure that compiler reload this
		 * variable every while cycle we call CPU_MEMORY_BARRIER.
		 * The memory barrier ensure that all variables used in this context
		 * are reloaded.
		 */
		IRQ_ENABLE;
		CPU_IDLE;
		MEMORY_BARRIER;
		IRQ_DISABLE;
	}
	IRQ_ENABLE;
}

void proc_preempt_timer(void)
{
	// TODO: check Quantum

	alarm(1);
	ATOMIC(SCHED_ENQUEUE(CurrentProcess));
	proc_schedule();
}

void proc_schedule(void)
{
	kill(0, SIGUSR1);
}

void proc_yield(void)
{
	ATOMIC(SCHED_ENQUEUE(CurrentProcess));

	proc_schedule();
}

/* signal handler */
void irq_entry(int signum)
{
	Process *old_process;

	TRACEMSG("storing %p:%s", CurrentProcess, CurrentProcess->monitor.name);
	CurrentProcess->leaving = false;
	getcontext(&CurrentProcess->context);
	/* We get here in two ways: directly, and after setcontext() below */

	if (CurrentProcess->leaving)
	{
		TRACEMSG("leaving to %p:%s", CurrentProcess, CurrentProcess->monitor.name);
		return;
	}

	old_process = CurrentProcess;

	irq_handlers[signum]();

	if (old_process != CurrentProcess)
	{
		TRACEMSG("launching %p:%s", CurrentProcess, CurrentProcess->monitor.name);
		CurrentProcess->leaving = true;
		setcontext(&CurrentProcess->context);
		/* not reached */
	}

	TRACEMSG("keeping %p:%s", CurrentProcess, CurrentProcess->monitor.name);
}

void preempt_init(void)
{
	struct sigaction act;
	act.sa_handler = irq_entry;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGUSR1);
	sigaddset(&act.sa_mask, SIGALRM);
	act.sa_flags = SA_RESTART; /* | SA_SIGINFO; */

	irq_handlers[SIGUSR1] = proc_preempt;
	irq_handlers[SIGALRM] = proc_preempt_timer;
	sigaction(SIGUSR1, &act, NULL);
	sigaction(SIGALRM, &act, NULL);

	alarm(1);
}