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
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Low-level timer module for Qt emulator (interface).
 */
#ifndef DRV_TIMER_QT_H
#define DRV_TIMER_QT_H

// HW dependent timer initialization

#define DEFINE_TIMER_ISR     void timer_isr(void)
#define TIMER_TICKS_PER_SEC  250
#define TIMER_HW_CNT         (1<<31) /* We assume 32bit integers here */

/// Type of time expressed in ticks of the hardware high-precision timer.
typedef unsigned int hptime_t;

/// Frequency of the hardware high-precision timer.
#define TIMER_HW_HPTICKS_PER_SEC  1000

/// Not needed.
#define timer_hw_irq() do {} while (0)

#endif /* DRV_TIMER_QT_H */
