/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@codewiz.org>
 * All Rights Reserved.
 * -->
 *
 * \brief High level serial I/O API
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.2  2004/05/23 18:21:53  bernie
 * Trim CVS logs and cleanup header info.
 *
 */
#ifndef DRV_SER_H
#define DRV_SER_H

#include "compiler.h"
#include <mware/fifobuf.h>
#include "config.h"

/*!
 * \name Serial Error/status flags
 *
 * Some of these flags map directly to the flags
 * in AVR UART Status Register(USR).
 * \todo  flags of DSP56k aren't mapped to these flags. Luckily
 *        these flags doesn't collide with the DSP56k ones,
 *        which are from 0x0100 to 0x8000
 */
/*\{*/
#if defined(__AVR__)
	/* Software errors */
	#define SERRF_RXFIFOOVERRUN  BV(0)  /*!< Rx FIFO buffer overrun */
	#define SERRF_RXTIMEOUT      BV(5)  /*!< Receive timeout */
	#define SERRF_TXTIMEOUT      BV(6)  /*!< Transmit timeout */

	/* Hardware errors */
	#define SERRF_RXSROVERRUN    BV(3)  /*!< Rx shift register overrun */
	#define SERRF_FRAMEERROR     BV(4)  /*!< Stop bit missing */
	#define SERRF_PARITYERROR    BV(7)  /*!< Parity error */
#elif defined(__m56800__)
	/* Software errors */
	#define SERRF_RXFIFOOVERRUN  BV(0)  /*!< Rx FIFO buffer overrun */
	#define SERRF_RXTIMEOUT      BV(1)  /*!< Receive timeout */
	#define SERRF_TXTIMEOUT      BV(2)  /*!< Transmit timeout */

	/* Hardware errors */
	#define SERRF_PARITYERROR    BV(8)  /*!< Parity error */
	#define SERRF_FRAMEERROR     BV(9)  /*!< Stop bit missing */
	#define SERRF_NOISEERROR     BV(10) /*!< Noise error */
	#define SERRF_RXSROVERRUN    BV(11) /*!< Rx shift register overrun */
#else
	#error unknown architecture
#endif
/*\}*/

/*! \name Parity settings for ser_setparity() */
/*\{*/
#define SER_PARITY_NONE  0
#define SER_PARITY_EVEN  2
#define SER_PARITY_ODD   3
/*\}*/

/*!
 * \name Serial hw numbers
 *
 * \{
 */
enum
{
	#if defined(__AVR_ATmega64__)
	SER_UART0,
	SER_UART1,
	SER_SPI,

	#elif defined(__AVR_ATmega103__)
	SER_UART0,
	SER_SPI,

	#elif defined (__m56800__)
	SER_UART0,
	SER_UART1,

	#else
		#error unknown architecture
	#endif

	SER_CNT       /**< Number of serial ports */
};
/* @} */


struct SerialHardware;

/*! Human-readable serial error descriptions */
extern const char * const serial_errors[8];

/*! Serial handle structure */
struct Serial
{
	/*! Physical port number */
	unsigned int unit;

	bool is_open;

	/*!
	 * \name FIFO transmit and receive buffers.
	 *
	 * Declared volatile because handled asinchronously by interrupts.
	 *
	 * \{
	 */
	volatile FIFOBuffer txfifo;
	volatile FIFOBuffer	rxfifo;
	unsigned char txbuffer[CONFIG_SER_TXBUFSIZE];
	unsigned char rxbuffer[CONFIG_SER_RXBUFSIZE];
	/* \} */

#ifdef CONFIG_SER_RXTIMEOUT
	time_t rxtimeout;
#endif
#ifdef CONFIG_SER_TXTIMEOUT
	time_t txtimeout;
#endif

	/*! Holds the flags defined above.  Will be 0 when no errors have occurred. */
	REGISTER uint16_t status;

	/*! Low-level interface to hardware. */
	struct SerialHardware* hw;
};


/* Function prototypes */
extern int ser_putchar(int c, struct Serial *port);
extern int ser_getchar(struct Serial *port);
extern int ser_getchar_nowait(struct Serial *port);
extern int ser_write(struct Serial *port, const void *buf, size_t len);
extern int ser_read(struct Serial *port, char *buf, size_t size);
extern int ser_print(struct Serial *port, const char *s);
extern int ser_gets(struct Serial *port, char *buf, int size);
extern int ser_gets_echo(struct Serial *port, char *buf, int size, bool echo);
extern int ser_printf(struct Serial *port, const char *format, ...) FORMAT(__printf__, 2, 3);
extern void ser_setbaudrate(struct Serial *port, unsigned long rate);
extern void ser_setparity(struct Serial *port, int parity);
extern void ser_purge(struct Serial *port);
extern struct Serial *ser_open(unsigned int unit);
extern void ser_close(struct Serial *port);
#if defined(CONFIG_SER_RXTIMEOUT) || defined(CONFIG_SER_TXTIMEOUT)
	extern void ser_settimeouts(struct Serial *port, time_t rxtimeout, time_t txtimeout);
#endif

/*!
 * \name Additional functions implemented as macros
 *
 * \{
 */
#define ser_getstatus(h)    ((h)->status)
#define ser_setstatus(h, x) ((h)->status = (x))
/* \} */

#endif /* DRV_SER_H */
