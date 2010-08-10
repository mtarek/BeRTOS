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
* -->
*
*
* \brief Embedded flash for cpu.
*
* \author Francesco Sacchi <batt@develer.com>
* \author Daniele Basile <asterix@develer.com>
*
* $WIZ$ module_name = "flash"
* $WIZ$ module_depends = "kfile"
*/

#ifndef DRV_FLASH_H
#define DRV_FLASH_H

#include "cfg/cfg_emb_flash.h"

#include <cfg/macros.h>

#include <io/kblock.h>
#include <io/kfile.h>
#include <io/kfile_block.h>

/*
 * Embedded flash error flags
 */
#define FLASH_WR_OK             0     ///< Write ok.
#define FLASH_NOT_ERASED     BV(1)    ///< Flash memory was not erased before to write it.
#define FLASH_WR_PROTECT     BV(2)    ///< Write not allowed the flash memory was protected.
#define FLASH_WR_TIMEOUT     BV(3)    ///<

struct FlashHardware;

/**
 * EmbFlash KFile context structure.
 */
typedef struct Flash
{
	KBlock blk;
	struct FlashHardware *hw;
	#if !CONFIG_FLASH_DISABLE_OLD_API
	union {
		KFile fd;
		KFileBlock fdblk;
	} DEPRECATED;
	#endif /* !CONFIG_FLASH_DISABLE_OLD_API */
} Flash;

/**
 * ID for FLASH
 */
#define KBT_FLASH MAKE_ID('F', 'L', 'A', 'S')

/**
* Convert + ASSERT from generic KFile to Flash.
*/
INLINE Flash *FLASH_CAST(KBlock *fls)
{
	ASSERT(fls->priv.type == KBT_FLASH);
	return (Flash *)fls;
}

void flash_hw_init(Flash *fls);
void flash_hw_initUnbuffered(Flash *fls);

#include CPU_HEADER(flash)

INLINE void flash_init(Flash *fls)
{
	flash_hw_init(fls);

	#if !CONFIG_FLASH_DISABLE_OLD_API
		kfileblock_init(&fls->fdblk, &fls->blk);
	#endif /* !CONFIG_FLASH_DISABLE_OLD_API */
}

INLINE void flash_initUnbuffered(Flash *fls)
{
	flash_hw_initUnbuffered(fls);
}


#endif /* DRV_FLASH_H */

