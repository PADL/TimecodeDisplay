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

#ifndef MTCTypes_h
#define MTCTypes_h			1

/* Full frame message */
#define MIDI_MESSAGE_TYPE_SYSEX_BEGIN	0xF0
#define MIDI_MESSAGE_TYPE_SYSEX_END	0xF7

/* Quarter frame message */
#define MIDI_MESSAGE_TYPE_MTC_QF	0xF1

#define MTC_FRAME_COUNT_LSN		0
#define MTC_FRAME_COUNT_MSN		1
#define MTC_SECONDS_COUNT_LSN		2
#define MTC_SECONDS_COUNT_MSN		3
#define MTC_MINUTES_COUNT_LSN		4
#define MTC_MINUTES_COUNT_MSN		5
#define MTC_HOURS_COUNT_LSN		6
#define MTC_HOURS_COUNT_MSN		7
#define MTC_SMPTE_TYPE			MTC_HOURS_COUNT_MSN

#endif /* MTCTypes_h */

