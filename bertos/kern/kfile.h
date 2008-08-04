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
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001, 2003 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Virtual KFile I/O interface.
 * KFile is a generic interface for file I/O.
 * It uses an object-oriented model to supply
 * a generic interface for drivers to communicate.
 * This module contains only definitions, the instance structure
 * and an API.
 * Each KFile user should implement at least some methods.
 * E.G.
 * If you have a serial driver and want to comply to KFile interface,
 * you have to declare your context structure:
 *
 * \code
 * typedef struct KFileSerial
 * {
 *		KFile fd;
 *		Serial *ser;
 * } KFileSerial;
 * \endcode
 *
 * You should also supply a macro for casting KFile to KFileSerial:
 *
 * \code
 * INLINE KFileSerial * KFILESERIAL(KFile *fd)
 * {
 *		ASSERT(fd->_type == KFT_SERIAL);
 *		return (KFileSerial *)fd;
 * }
 * \endcode
 *
 * Then you can implement as much interface functions as you like
 * and leave the others to NULL.
 * ser_close implementation example:
 *
 * \code
 * static int ser_kfile_close(struct KFile *fd)
 * {
 *		KFileSerial *fds = KFILESERIAL(fd);
 *		ser_close(fds->ser);
 *		return 0;
 * }
 * \endcode
 * KFILESERIAL macro helps to ensure that obj passed is really a Serial.
 *
 * KFile interface do not supply the open function: this is deliberate,
 * because in embedded systems each device has its own init parameters.
 * For the same reason specific file settings (like baudrate for Serial, for example)
 * are demanded to specific driver implementation.
 *
 * \version $Id$
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef KERN_KFILE_H
#define KERN_KFILE_H

#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <cfg/macros.h>

/* fwd decl */
struct KFile;

typedef int32_t kfile_off_t;    ///< KFile offset type, used by kfile_seek function.
typedef uint32_t kfile_size_t;   ///< KFile size type, used in kfile struct.

/**
 * Costants for repositioning read/write file offset.
 * These are needed because on some embedded platforms
 * ANSI I/O library may not be present.
 */
typedef enum KSeekMode
{
	KSM_SEEK_SET, ///< Seek from file beginning.
	KSM_SEEK_CUR, ///< Seek from file current position.
	KSM_SEEK_END, ///< Seek from file end.
} KSeekMode;

/**
 * Prototypes for KFile access functions.
 * I/O file functions must be ANSI compliant.
 * \note A KFile user can choose which function subset to implement,
 *       but has to set to NULL unimplemented features.
 * \{
 */

/**
 * Read from file.
 * \return the number of bytes read.
 */
typedef size_t (*ReadFunc_t) (struct KFile *fd, void *buf, size_t size);

/**
 * Write to file.
 * \return the number of bytes written.
 */
typedef size_t (*WriteFunc_t) (struct KFile *fd, const void *buf, size_t size);

/**
 * Seek into file (if seekable).
 * \return the new file offset or EOF on errors.
 */
typedef kfile_off_t (*SeekFunc_t) (struct KFile *fd, kfile_off_t offset, KSeekMode whence);

/**
 * Close and reopen file \a fd.
 * The reopening is done with the former file parameters and access modes.
 */
typedef struct KFile * (*ReOpenFunc_t) (struct KFile *fd);

/**
 * Close file.
 * \return 0 on success, EOF on errors.
 */
typedef int (*CloseFunc_t) (struct KFile *fd);

/**
 * Flush file I/O.
 * \return 0 on success, EOF on errors.
 */
typedef int (*FlushFunc_t) (struct KFile *fd);

/**
 * Get file error mask.
 * \return 0 on success or file error code, device specific.
 */
typedef int (*ErrorFunc_t) (struct KFile *fd);

/**
 * Clear errors.
 */
typedef void (*ClearErrFunc_t) (struct KFile *fd);
/* \} */

/**
 * Context data for callback functions which operate on
 * pseudo files.
 * \note If you change interface, remember to add corresponding access function.
 */
typedef struct KFile
{
	ReadFunc_t     read;
	WriteFunc_t    write;
	ReOpenFunc_t   reopen;
	CloseFunc_t    close;
	SeekFunc_t     seek;
	FlushFunc_t    flush;
	ErrorFunc_t    error;
	ClearErrFunc_t clearerr;
	DB(id_t _type); ///< Used to keep trace, at runtime, of obj type.

	/* NOTE: these must _NOT_ be size_t on 16bit CPUs! */
	kfile_off_t  seek_pos;
	kfile_size_t size;
} KFile;

/**
 * Generic implementation of kfile_seek.
 */
kfile_off_t kfile_genericSeek(struct KFile *fd, kfile_off_t offset, KSeekMode whence);

/**
 * Generic implementation of kfile_reopen.
 */
struct KFile * kfile_genericReopen(struct KFile *fd);

int kfile_putc(int c, struct KFile *fd); ///< Generic putc implementation using kfile_write.
int kfile_getc(struct KFile *fd);  ///< Generic getc implementation using kfile_read.
int kfile_printf(struct KFile *fd, const char *format, ...);
int kfile_print(struct KFile *fd, const char *s);
int kfile_gets(struct KFile *fd, char *buf, int size);
int kfile_gets_echo(struct KFile *fd, char *buf, int size, bool echo);

/**
 * Interface functions for KFile access.
 * \note Remember to change following functions if KFile interface changes.
 * \{
 */
INLINE size_t kfile_read(struct KFile *fd, void *buf, size_t size)
{
	ASSERT(fd->read);
	return fd->read(fd, buf, size);
}

INLINE size_t kfile_write(struct KFile *fd, const void *buf, size_t size)
{
	ASSERT(fd->write);
	return fd->write(fd, buf, size);
}

INLINE KFile * kfile_reopen(struct KFile *fd)
{
	ASSERT(fd->reopen);
	return fd->reopen(fd);
}

INLINE int kfile_close(struct KFile *fd)
{
	ASSERT(fd->close);
	return fd->close(fd);
}

INLINE kfile_off_t kfile_seek(struct KFile *fd, kfile_off_t offset, KSeekMode whence)
{
	ASSERT(fd->seek);
	return fd->seek(fd, offset, whence);
}

INLINE int kfile_flush(struct KFile *fd)
{
	ASSERT(fd->flush);
	return fd->flush(fd);
}

INLINE int kfile_error(struct KFile *fd)
{
	ASSERT(fd->error);
	return fd->error(fd);
}

INLINE void kfile_clearerr(struct KFile *fd)
{
	ASSERT(fd->clearerr);
	fd->clearerr(fd);
}
/* \} */

/**
 * Kfile test function.
 */
int kfile_testSetUp(void);
int kfile_testRun(KFile *fd, uint8_t *test_buf, uint8_t *save_buf, size_t size);
int kfile_testTearDown(void);

#endif /* KERN_KFILE_H */
