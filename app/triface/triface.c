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
 * Copyright 2003, 2004, 2006, 2008 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \version $Id$
 *
 * \author Marco Benelli <marco@develer.com>
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 * \brief Windowing system test.
 */


#include <drv/timer.h>
#include <drv/buzzer.h>
#include <drv/ser.h>
#include <drv/sipo.h>

#include <cfg/macros.h>
#include <mware/parser.h>
#include <net/keytag.h>

#include "protocol.h"
#include "hw_input.h"
#include "hw_adc.h"


static KFileSerial fd_ser;

int main(void)
{
	/* SPI Port Initialization */
	sipo_init();

	kdbg_init();
	timer_init();
	adc_init();
	buz_init();

	IRQ_ENABLE;
	INPUT_INIT;

	/* Initialize Tag serial port and data structure */
	TagPacket pkt;

	/* Open the main communication port */
	ser_init(&fd_ser, CONFIG_SER_HOSTPORT);
	ser_setbaudrate(&fd_ser, CONFIG_SER_HOSTPORTBAUDRATE);

	ser_init(pkt.tag_ser, TAG_SER_PORT);
	ser_setbaudrate(pkt.tag_ser, TAG_SER_BAUDRATE);

	pkt.comm_ser = &fd_ser;
	keytag_init(&pkt);

	protocol_init(&fd_ser.fd);

	// Main loop
	for(;;)
	{
		protocol_run(&fd_ser.fd);
		keytag_poll(&pkt);
	}

	return 0;
}


