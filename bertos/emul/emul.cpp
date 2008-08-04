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
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000, 2001 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \version $Id$
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * \brief Qt-based emulator framework for embedded applications (implementation)
 */

#include "emul.h"
#include "emulwin.h"

#include "cfg/cfg_kern.h"

#include <cfg/module.h>



#include <cstdlib> // std::exit()

#include <QtGui/qapplication.h>


/// The global emulator instance.
Emulator *emul;

#if CONFIG_KERNEL
	#include <mware/list.h>

	/// List of process stacks
	List StackFreeList;

	// HACK: Reserve 64KB of stack space for kernel processes
	const int NPROC = 8;
	int stacks[NPROC][(64 * 1024) / sizeof(int)];
#endif

Emulator::Emulator(int &argc, char **argv) :
	emulApp(new QApplication(argc, argv)),
	emulWin(new EmulWin(this))
{
	emulWin->show();
}


Emulator::~Emulator()
{
	// we don't delete emulWin because it automatically
	// deletes itself when closed
	delete emulApp;
}


NORETURN void Emulator::quit()
{
	// WHAT A KLUDGE!
	this->~Emulator();
	emul = NULL;

	// do we have a better way to shut down the emulation?
	exit(0);
}

MOD_DEFINE(emul)

/// Main emulator entry point.
extern "C" void emul_init(int *argc, char *argv[])
{
	// setup global emulator pointer
	emul = new Emulator(*argc, argv);

#if CONFIG_KERNEL
	LIST_INIT(&StackFreeList);
	for (int i = 0; i < NPROC; i++)
		ADDTAIL(&StackFreeList, (Node *)stacks[i]);
#endif

	MOD_INIT(emul);
}

extern "C" void emul_cleanup()
{
	MOD_CLEANUP(emul);

	delete emul;
	emul = NULL;
}

extern "C" void emul_idle()
{
	// We process GUI events when the application is idle.
	emul->emulApp->processEvents();
	usleep(1000);
}

