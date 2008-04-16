/*
 * Copyright (c) 2008, PADL Software Pty Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of PADL Software nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PADL SOFTWARE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL PADL SOFTWARE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef SMPTETypes_h
#define SMPTETypes_h			1

/* Frame rates */
#define SMPTE_TIME_TYPE_24		0
#define SMPTE_TIME_TYPE_25		1
#define SMPTE_TIME_TYPE_30_DROP		2
#define SMPTE_TIME_TYPE_30		3
#define SMPTE_TIME_TYPE_2997		4
#define SMPTE_TIME_TYPE_2997_DROP	5
#define SMPTE_TIME_TYPE_60		6
#define SMPTE_TIME_TYPE_5994		7
#define SMPTE_TIME_TYPE_60_DROP		8
#define SMPTE_TIME_TYPE_5994_DROP	9
#define SMPTE_TIME_TYPE_50		10
#define SMPTE_TIME_TYPE_2398		11
#define SMPTE_TIME_TYPE_MAX		SMPTE_TIME_TYPE_2398

/* Flags */
#define SMPTE_TIME_VALID		0x01
#define SMPTE_TIME_RUNNING		0x02

/* Time in milliseconds before we lose sync */
#define SMPTE_LOCK_WINDOW		100

typedef struct _SMPTE_TIME {
	int subframes;
	int subframeDivisor;
	unsigned long counter;		/* messages received */
	unsigned long type;		/* SMPTE_TIME_TYPE_XXX */
	unsigned long flags;		/* SMPTE_TIME_XXX */
	int hours;
	int minutes;
	int seconds;
	int frames;
} SMPTE_TIME;

typedef enum _TC_STATUS {
	TC_ERROR = -1,
	TC_NOOP = 0,
	TC_PULSE = 1,
	TC_QUARTER = 2,
	TC_FULL = 3
} TC_STATUS;

/* Trace codes */
#define TC_INVALID_MESSAGE_TYPE		0xA1
#define TC_MESSAGE_OUT_OF_SEQUENCE	0xA2
#define TC_INVALID_QF_TIMECODE		0xA3
#define TC_UNKNOWN_SYSEX_MESSAGE	0xA4
#define TC_INVALID_MIDI_CHANNEL		0xA5
#define TC_INVALID_MTC_HEADER		0xA6
#define TC_INVALID_MTC_SUB_HEADER	0xA7
#define TC_FULL_MESSAGE_TOO_LONG	0xA8
#define TC_INVALID_FF_TIMECODE		0xA9

#endif /* SMPTETypes_h */

