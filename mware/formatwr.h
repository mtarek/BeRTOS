/*!
 * \file
 * <!--
 * Copyright (C) 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \version $Id$
 *
 * \brief Basic "printf", "sprintf" and "fprintf" formatter.
 */

/*
 * $Log$
 * Revision 1.1  2004/05/23 15:43:16  bernie
 * Import mware modules.
 *
 */
#ifndef MWARE_FORMATWR_H
#define MWARE_FORMATWR_H

#include <stdarg.h>

int _formatted_write(
	const char *format,
	void put_one_char(char, void *),
	void *secret_pointer,
	va_list ap);

#ifdef __AVR__
	#include <avr/pgmspace.h>
	int _formatted_write_P(
		const char * PROGMEM format,
		void put_one_char(char, void *),
		void *secret_pointer,
		va_list ap);
#endif /* __AVR__ */

#endif /* MWARE_FORMATWR_H */

