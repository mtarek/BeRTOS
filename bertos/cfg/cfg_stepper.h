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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Configuration file for stepper motor module.
 *
 * \version $Id$
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef CFG_STEPPER_H
#define CFG_STEPPER_H

/**
 * Logging level definition.
 *
 * Use 0 to log only the error messages
 * Use 1 to log the error and warning messages
 * Use 2 to log all messages
 */
#define STEPPER_LOG_LEVEL           2

/**
 * Set logging verbosity.
 *
 * If verbosity is zero print short log messages.
 */
#define STEPPER_LOG_VERBOSITY        0


/// Max number of the stepper motor.
#define CONFIG_NUM_STEPPER_MOTORS    6

/// Max number of the timer usable on target to drive stepper motor.
#define CONFIG_TC_STEPPER_MAX_NUM    6

#endif /* CFG_STEPPER_H */

