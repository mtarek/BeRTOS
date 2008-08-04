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
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Driver for the 24xx16 and 24xx256 I2C EEPROMS (implementation)
 *
 *
 * \version $Id$
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#include "eeprom.h"

#warning TODO:Test and complete this module for arm platform.

#if 0
#include "cfg/cfg_eeprom.h"  // CONFIG_EEPROM_VERIFY
#include <cfg/macros.h>  // MIN()
#include <cfg/debug.h>

#include <cpu/attr.h>
#include CPU_HEADER(twi)

#include <drv/wdt.h>

#include <mware/byteorder.h> // cpu_to_be16()

#include <string.h>  // memset()


// Configuration sanity checks
#if !defined(CONFIG_EEPROM_VERIFY) || (CONFIG_EEPROM_VERIFY != 0 && CONFIG_EEPROM_VERIFY != 1)
	#error CONFIG_EEPROM_VERIFY must be defined to either 0 or 1
#endif

/**
 * EEPROM ID code
 */
#define EEPROM_ID  0xA0

/**
 * This macros form the correct slave address for EEPROMs
 */
#define EEPROM_ADDR(x) (EEPROM_ID | (((uint8_t)(x)) << 1))




/**
 * Copy \c count bytes from buffer \c buf to
 * eeprom at address \c addr.
 */
static bool eeprom_writeRaw(e2addr_t addr, const void *buf, size_t count)
{
	bool result = true;
	ASSERT(addr + count <= EEPROM_SIZE);

	while (count && result)
	{
		/*
		 * Split write in multiple sequential mode operations that
		 * don't cross page boundaries.
		 */
		size_t size =
			MIN(count, (size_t)(EEPROM_BLKSIZE - (addr & (EEPROM_BLKSIZE - 1))));

	#if CONFIG_EEPROM_TYPE == EEPROM_24XX16
		/*
		 * The 24LC16 uses the slave address as a 3-bit
		 * block address.
		 */
		uint8_t blk_addr = (uint8_t)((addr >> 8) & 0x07);
		uint8_t blk_offs = (uint8_t)addr;

		result =
			twi_start_w(EEPROM_ADDR(blk_addr))
			&& twi_send(&blk_offs, sizeof blk_offs)
			&& twi_send(buf, size);

	#elif CONFIG_EEPROM_TYPE == EEPROM_24XX256

		// 24LC256 wants big-endian addresses
		uint16_t addr_be = cpu_to_be16(addr);

		result =
			twi_start_w(EEPROM_ID)
			&& twi_send((uint8_t *)&addr_be, sizeof addr_be)
			&& twi_send(buf, size);

	#else
		#error Unknown device type
	#endif

		twi_stop();

		// DEBUG
		//kprintf("addr=%d, count=%d, size=%d, *#?=%d\n",
		//	addr, count, size,
		//	(EEPROM_BLKSIZE - (addr & (EEPROM_BLKSIZE - 1)))
		//);

		/* Update count and addr for next operation */
		count -= size;
		addr += size;
		buf = ((const char *)buf) + size;
	}

	if (!result)
		TRACEMSG("Write error!");
	return result;
}


#if CONFIG_EEPROM_VERIFY
/**
 * Check that the contents of an EEPROM range
 * match with a provided data buffer.
 *
 * \return true on success.
 */
static bool eeprom_verify(e2addr_t addr, const void *buf, size_t count)
{
	uint8_t verify_buf[16];
	bool result = true;

	while (count && result)
	{
		/* Split read in smaller pieces */
		size_t size = MIN(count, sizeof verify_buf);

		/* Read back buffer */
		if (eeprom_read(addr, verify_buf, size))
		{
			if (memcmp(buf, verify_buf, size) != 0)
			{
				TRACEMSG("Data mismatch!");
				result = false;
			}
		}
		else
		{
			TRACEMSG("Read error!");
			result = false;
		}

		/* Update count and addr for next operation */
		count -= size;
		addr += size;
		buf = ((const char *)buf) + size;
	}

	return result;
}
#endif /* CONFIG_EEPROM_VERIFY */


bool eeprom_write(e2addr_t addr, const void *buf, size_t count)
{
#if CONFIG_EEPROM_VERIFY
	int retries = 5;

	while (retries--)
		if (eeprom_writeRaw(addr, buf, count)
				&& eeprom_verify(addr, buf, count))
			return true;

	return false;

#else /* !CONFIG_EEPROM_VERIFY */
	return eeprom_writeRaw(addr, buf, count);
#endif /* !CONFIG_EEPROM_VERIFY */
}


/**
 * Copy \c count bytes at address \c addr
 * from eeprom to RAM to buffer \c buf.
 *
 * \return true on success.
 */
bool eeprom_read(e2addr_t addr, void *buf, size_t count)
{
	ASSERT(addr + count <= EEPROM_SIZE);

#if CONFIG_EEPROM_TYPE == EEPROM_24XX16
	/*
	 * The 24LC16 uses the slave address as a 3-bit
	 * block address.
	 */
	uint8_t blk_addr = (uint8_t)((addr >> 8) & 0x07);
	uint8_t blk_offs = (uint8_t)addr;

	bool res =
		twi_start_w(EEPROM_ADDR(blk_addr))
		&& twi_send(&blk_offs, sizeof blk_offs)
		&& twi_start_r(EEPROM_ADDR(blk_addr))
		&& twi_recv(buf, count);

#elif CONFIG_EEPROM_TYPE == EEPROM_24XX256

	// 24LC256 wants big-endian addresses
	addr = cpu_to_be16(addr);

	bool res =
		twi_start_w(EEPROM_ID)
		&& twi_send((uint8_t *)&addr, sizeof(addr))
		&& twi_start_r(EEPROM_ID)
		&& twi_recv(buf, count);
#else
	#error Unknown device type
#endif

	twi_stop();

	if (!res)
		TRACEMSG("Read error!");
	return res;
}


/**
 * Write a single character \a c at address \a addr.
 */
bool eeprom_write_char(e2addr_t addr, char c)
{
	return eeprom_write(addr, &c, 1);
}


/**
 * Read a single character at address \a addr.
 *
 * \return the requested character or -1 in case of failure.
 */
int eeprom_read_char(e2addr_t addr)
{
	char c;

	if (eeprom_read(addr, &c, 1))
		return c;
	else
		return -1;
}


/**
 * Erase specified part of eeprom, writing 0xFF.
 *
 * \param addr   starting address
 * \param count  length of block to erase
 */
void eeprom_erase(e2addr_t addr, size_t count)
{
	uint8_t buf[EEPROM_BLKSIZE];
	memset(buf, 0xFF, sizeof buf);

	// Clear all but struct hw_info at start of eeprom
	while (count)
	{
		// Long operation, reset watchdog
		wdt_reset();

		size_t size = MIN(count, sizeof buf);
		eeprom_write(addr, buf, size);
		addr += size;
		count -= size;
	}
}


/**
 * Initialize TWI module.
 */
void eeprom_init(void)
{
	twi_init();
}


#ifdef _DEBUG

#include <string.h>

void eeprom_test(void)
{
	static const char magic[14] = "Humpty Dumpty";
	char buf[sizeof magic];
	size_t i;

	// Write something to EEPROM using unaligned sequential writes
	for (i = 0; i < 42; ++i)
	{
		wdt_reset();
		eeprom_write(i * sizeof magic, magic, sizeof magic);
	}

	// Read back with single-byte reads
	for (i = 0; i < 42 * sizeof magic; ++i)
	{
		wdt_reset();
		eeprom_read(i, buf, 1);
		kprintf("EEPROM byte read: %c (%d)\n", buf[0], buf[0]);
		ASSERT(buf[0] == magic[i % sizeof magic]);
	}

	// Read back again using sequential reads
	for (i = 0; i < 42; ++i)
	{
		wdt_reset();
		memset(buf, 0, sizeof buf);
		eeprom_read(i * sizeof magic, buf, sizeof magic);
		kprintf("EEPROM seq read @ 0x%x: '%s'\n", i * sizeof magic, buf);
		ASSERT(memcmp(buf, magic, sizeof magic) == 0);
	}
}

#endif // _DEBUG
#endif
