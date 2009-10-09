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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 * \brief Simple AX25 data link layer implementation.
 *
 * For now, only UI frames without any Layer 3 protocol are handled.
 * This however is enough to send/receive APRS packets.
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 *
 */

#include "ax25.h"
#include "cfg/cfg_ax25.h"

#include <algo/crc_ccitt.h>

#define LOG_LEVEL  AX25_LOG_LEVEL
#define LOG_FORMAT AX25_LOG_FORMAT
#include <cfg/log.h>

#include <string.h> //memset, memcmp
#include <ctype.h>  //isalnum, toupper

#define DECODE_CALL(buf, addr) \
	for (unsigned i = 0; i < sizeof((addr)); i++) \
	{ \
		(addr)[i] = *(buf)++ >> 1; \
	}

static void ax25_decode(AX25Ctx *ctx)
{
	AX25Msg msg;
	uint8_t *buf = ctx->buf;

	DECODE_CALL(buf, msg.dst.call);
	msg.dst.ssid = (*buf++ >> 1) & 0x0F;

	DECODE_CALL(buf, msg.src.call);
	msg.src.ssid = (*buf >> 1) & 0x0F;

	LOG_INFO("SRC[%.6s-%d], DST[%.6s-%d]\n", msg.src.call, msg.src.ssid, msg.dst.call, msg.dst.ssid);

	/* Repeater addresses */
	#if CONFIG_AX25_RPT_LST
		for (msg.rpt_cnt = 0; !(*buf++ & 0x01) && (msg.rpt_cnt < countof(msg.rpt_lst)); msg.rpt_cnt++)
		{
			DECODE_CALL(buf, msg.rpt_lst[msg.rpt_cnt].call);
			msg.rpt_lst[msg.rpt_cnt].ssid = (*buf >> 1) & 0x0F;
			LOG_INFO("RPT%d[%.6s-%d]\n", msg.rpt_cnt, msg.rpt_lst[msg.rpt_cnt].call, msg.rpt_lst[msg.rpt_cnt].ssid);
		}
	#else
		while (!(*buf++ & 0x01))
		{
			char rpt[6];
			uint8_t ssid;
			DECODE_CALL(buf, rpt);
			ssid = (*buf >> 1) & 0x0F;
			LOG_INFO("RPT[%.6s-%d]\n", rpt, ssid);
		}
	#endif

	msg.ctrl = *buf++;
	if (msg.ctrl != AX25_CTRL_UI)
	{
		LOG_WARN("Only UI frames are handled, got [%02X]\n", msg.ctrl);
		return;
	}

	msg.pid = *buf++;
	if (msg.pid != AX25_PID_NOLAYER3)
	{
		LOG_WARN("Only frames without layer3 protocol are handled, got [%02X]\n", msg.pid);
		return;
	}

	msg.len = ctx->frm_len - 2 - (buf - ctx->buf);
	msg.info = buf;
	LOG_INFO("DATA: %.*s\n", msg.len, msg.info);

	if (ctx->hook)
		ctx->hook(&msg);
}


/**
 * Check if there are any AX25 messages to be processed.
 * This function read available characters from the medium and search for
 * any AX25 messages.
 * If a message is found it is decoded and the linked callback executed.
 * This function may be blocking if there are no available chars and the KFile
 * used in \a ctx to access the medium is configured in blocking mode.
 *
 * \param ctx AX25 context to operate on.
 */
void ax25_poll(AX25Ctx *ctx)
{
	int c;

	while ((c = kfile_getc(ctx->ch)) != EOF)
	{
		if (!ctx->escape && c == HDLC_FLAG)
		{
			if (ctx->frm_len >= AX25_MIN_FRAME_LEN)
			{
				if (ctx->crc_in == AX25_CRC_CORRECT)
				{
					LOG_INFO("Frame found!\n");
					ax25_decode(ctx);
				}
				else
				{
					LOG_INFO("CRC error, computed [%04X]\n", ctx->crc_in);
				}
			}
			ctx->sync = true;
			ctx->crc_in = CRC_CCITT_INIT_VAL;
			ctx->frm_len = 0;
			continue;
		}

		if (!ctx->escape && c == HDLC_RESET)
		{
			LOG_INFO("HDLC reset\n");
			ctx->sync = false;
			continue;
		}

		if (!ctx->escape && c == AX25_ESC)
		{
			ctx->escape = true;
			continue;
		}

		if (ctx->sync)
		{
			if (ctx->frm_len < CONFIG_AX25_FRAME_BUF_LEN)
			{
				ctx->buf[ctx->frm_len++] = c;
				ctx->crc_in = updcrc_ccitt(c, ctx->crc_in);
			}
			else
			{
				LOG_INFO("Buffer overrun");
				ctx->sync = false;
			}
		}
		ctx->escape = false;
	}

	if (kfile_error(ctx->ch))
	{
		LOG_ERR("Channel error [%04x]\n", kfile_error(ctx->ch));
		kfile_clearerr(ctx->ch);
	}
}

static void ax25_putchar(AX25Ctx *ctx, uint8_t c)
{
	if (c == HDLC_FLAG || c == HDLC_RESET
		|| c == AX25_ESC)
		kfile_putc(AX25_ESC, ctx->ch);
	ctx->crc_out = updcrc_ccitt(c, ctx->crc_out);
	kfile_putc(c, ctx->ch);
}

static void ax25_sendCall(AX25Ctx *ctx, const AX25Call *addr)
{
	unsigned len = MIN(sizeof(addr->call), strlen(addr->call));

	for (unsigned i = 0; i < len; i++)
	{
		uint8_t c = addr->call[i];
		ASSERT(isalnum(c) || c == ' ');
		c = toupper(c);
		ax25_putchar(ctx, c << 1);
	}

	/* Fill with spaces the rest of the CALL if it's shorter */
	if (len < sizeof(addr->call))
		for (unsigned i = 0; i < sizeof(addr->call) - len; i++)
			ax25_putchar(ctx, ' ' << 1);
}

/**
 * Send an AX25 frame on the channel.
 * \param ctx AX25 context to operate on.
 * \param dst the destination callsign for the frame, \see AX25_CALL
 *        for a handy way to create a callsign on the fly.
 * \param src the source callsign for the frame, \see AX25_CALL
 *        for a handy way to create a callsign on the fly.
 * \param _buf payload buffer.
 * \param len length of the payload.
 */
void ax25_send(AX25Ctx *ctx, const AX25Call *dst, const AX25Call *src, const void *_buf, size_t len)
{
	const uint8_t *buf = (const uint8_t *)_buf;

	ctx->crc_out = CRC_CCITT_INIT_VAL;
	kfile_putc(HDLC_FLAG, ctx->ch);

	ax25_sendCall(ctx, dst);
	ax25_putchar(ctx, dst->ssid << 1);

	ax25_sendCall(ctx, src);
	ax25_putchar(ctx, (src->ssid << 1) | 0x01);
	ax25_putchar(ctx, AX25_CTRL_UI);
	ax25_putchar(ctx, AX25_PID_NOLAYER3);

	while (len--)
		ax25_putchar(ctx, *buf++);

	/*
	 * According to AX25 protocol,
	 * CRC is sent in reverse order!
	 */
	uint8_t crcl = (ctx->crc_out & 0xff) ^ 0xff;
	uint8_t crch = (ctx->crc_out >> 8) ^ 0xff;
	ax25_putchar(ctx, crcl);
	ax25_putchar(ctx, crch);

	ASSERT(ctx->crc_out == AX25_CRC_CORRECT);

	kfile_putc(HDLC_FLAG, ctx->ch);
}


/**
 * Init the AX25 protocol decoder.
 *
 * \param ctx AX25 context to init.
 * \param channel Used to gain access to the physical medium
 * \param hook Callback function called when a message is received
 */
void ax25_init(AX25Ctx *ctx, KFile *channel, ax25_callback_t hook)
{
	ASSERT(ctx);
	ASSERT(channel);

	memset(ctx, 0, sizeof(*ctx));
	ctx->ch = channel;
	ctx->hook = hook;
	ctx->crc_in = ctx->crc_out = CRC_CCITT_INIT_VAL;
}
