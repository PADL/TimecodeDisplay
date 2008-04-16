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

static char rcsId[] = "$Id$";

#include <HardwareSerial.h>

#include "TimecodeDisplay.h"

/*
 * Parse a quarter frame MTC message. We must read a full sequence
 * of eight quarter frame messages to achieve timecode lock.
 */
static TC_STATUS parseQuarterFrameMessage(SMPTE_TIME *tc, byte msg)
{
	byte msgType = (msg & 0xF0) >> 4;

	if (msgType > MTC_HOURS_COUNT_MSN) {
		TC_TRACE(TC_INVALID_MESSAGE_TYPE, msg);
		return TC_ERROR;
	} else if (tc->flags & SMPTE_TIME_RUNNING) {
		if (msgType % 8 != tc->counter % 8 &&
		    msgType % 8 != (tc->counter - 2) % 8) {
			/* Can't skip subframes */
			TC_TRACE(TC_MESSAGE_OUT_OF_SEQUENCE, msg);
			return TC_ERROR;
		}
		tc->counter++;
	} else if (msgType != MTC_FRAME_COUNT_LSN) {
		/* NOOP until frame boundary received */
		return TC_NOOP;
	} else {
		tcInitialize(tc);
		tc->flags |= SMPTE_TIME_RUNNING; /* Time is now running. */
		tc->counter = 1;
	}

	tc->subframes = msgType % 4;

	msg &= 0xF; /* message data is in lower four bits */

	switch (msgType) {
	case MTC_FRAME_COUNT_LSN:
		tc->frames |= msg;
		break;
	case MTC_FRAME_COUNT_MSN:
		msg &= 0x1; /* top three bits are reserved for future use */
		tc->frames |= msg << 4;
		break;
	case MTC_SECONDS_COUNT_LSN:
		tc->seconds |= msg;
		break;
	case MTC_SECONDS_COUNT_MSN:
		msg &= 0x3; /* top two bits are reserved for future use */
		tc->seconds |= msg << 4;
		break;
	case MTC_MINUTES_COUNT_LSN:
		tc->minutes |= msg;
		break;
	case MTC_MINUTES_COUNT_MSN:
		msg &= 0x3; /* top two bits are reserved for future use */
		tc->minutes |= msg << 4;
		break;
	case MTC_HOURS_COUNT_LSN:
		tc->hours |= msg;
		break;
	case MTC_HOURS_COUNT_MSN:
		tc->type = (msg & 0x6) >> 1;
		tc->hours |= (msg & 0x1) << 4;
		break;
	}

	if ((tc->counter % 8) == 0) {
		if (tcIsValid(tc)) {
			tc->flags |= SMPTE_TIME_VALID;
			return TC_QUARTER;
		} else {
			TC_TRACE(TC_INVALID_QF_TIMECODE, msg);
			return TC_ERROR;
		}
	}

	return TC_PULSE;
}

static TC_STATUS parseFullFrameMessage(SMPTE_TIME *tc, byte msg[4])
{
	tc->subframes = 0;
	tc->counter = 0;
	tc->type = (msg[0] & 0x60) >> 6;
	tc->hours = msg[0] & 0x1F;
	tc->minutes = msg[1] & 0xFF;
	tc->seconds = msg[2] & 0xFF;
	tc->frames = msg[3] & 0xFF;

	tc->flags &= ~(SMPTE_TIME_RUNNING);

	if (tcIsValid(tc)) {
		tc->flags |= SMPTE_TIME_VALID;
		/* timecode is now valid, but not running */
		return TC_FULL;
	}

	tc->flags &= ~(SMPTE_TIME_VALID);

	TC_TRACE(TC_INVALID_FF_TIMECODE, 0xA9);

	return TC_ERROR;
}

#define EXPECT_FULL_FRAME	0
#define EXPECT_QUARTER_FRAME	1
#define EXPECT_CHANNEL		2
#define EXPECT_MTC_HEADER	3
#define EXPECT_MTC_SUB_HEADER	4
#define EXPECT_MTC_DATA_0	5
#define EXPECT_MTC_DATA_1	6
#define EXPECT_MTC_DATA_2	7
#define EXPECT_MTC_DATA_3	8
#define EXPECT_EOX		9
#define EXPECT_ANY		10

static TC_STATUS parseMessage(SMPTE_TIME *tc)
{
	byte fullFrame[4];
	int state = EXPECT_ANY;

	while (Serial.available() > 0) {
		byte data = Serial.read();

		delay(3);

		switch (state) {
		case EXPECT_QUARTER_FRAME:
			return parseQuarterFrameMessage(tc, data);
			break;
		case EXPECT_FULL_FRAME:
			if (data != 0x7F) { /* real time universal system exclusive header */
				TC_TRACE(TC_UNKNOWN_SYSEX_MESSAGE, data);
				return TC_NOOP;
			}
			state = EXPECT_CHANNEL;
			break;
		case EXPECT_CHANNEL:
			if (data != 0x7F) { /* message intended for entire system */
				TC_TRACE(TC_INVALID_MIDI_CHANNEL, data);
				return TC_NOOP;
			}
			state = EXPECT_MTC_HEADER;
			break;
		case EXPECT_MTC_HEADER:
			if (data != 0x01) { /* 'MIDI Time Code' */
				TC_TRACE(TC_INVALID_MTC_HEADER, data);
				return TC_NOOP;
			}
			state = EXPECT_MTC_SUB_HEADER;
			break;
		case EXPECT_MTC_SUB_HEADER:
			if (data != 0x01) {/* Full Time Code Message */
				TC_TRACE(TC_INVALID_MTC_SUB_HEADER, data);
				return TC_ERROR;
			}
			state = EXPECT_MTC_DATA_0;
			break;
		case EXPECT_MTC_DATA_0:
		case EXPECT_MTC_DATA_1:
		case EXPECT_MTC_DATA_2:
		case EXPECT_MTC_DATA_3:
			fullFrame[state - EXPECT_MTC_DATA_0] = data;
			state++;
			break;
		case EXPECT_EOX:
			if (data != 0xF7) {
				TC_TRACE(TC_FULL_MESSAGE_TOO_LONG, data);
				return TC_ERROR; /* invalid message */
			}
			state = EXPECT_ANY;
			return parseFullFrameMessage(tc, fullFrame);
			break;
		case EXPECT_ANY:
			if (data & 0x80) {
				/* status bit is set */
				switch (data) {
				case MIDI_MESSAGE_TYPE_SYSEX_BEGIN:
					state = EXPECT_FULL_FRAME;
					break;
				case MIDI_MESSAGE_TYPE_MTC_QF:
					state = EXPECT_QUARTER_FRAME;
					break;
				default:
					break;
				}
			} /* else ignore other data */
			break;
		}
	}

	return TC_NOOP;
}

void tcFollow(SMPTE_TIME *tc)
{
	SMPTE_TIME dt;
	boolean bLocked = false;
	unsigned long lastQFMessage = 0;

	tcInitialize(&dt);	/* display time */
	tcInitialize(tc);	/* MTC time */

	while (1) {
		TC_STATUS tcStatus = parseMessage(tc);

		switch (tcStatus) {
		case TC_NOOP:		/* Not a MTC message */
			if (lastQFMessage + SMPTE_LOCK_WINDOW < millis()) {
				tc->flags &= ~(SMPTE_TIME_RUNNING);
			}
			break;
		case TC_PULSE:		/* Quarter frame message, not on double frame boundary */
			if (tc->subframes == 0) {
				lastQFMessage = millis();
				incrementFrame(&dt, 1);
				tcDisplay(&dt);
			}
			break;
		case TC_QUARTER:	/* Quarter frame message, on double frame boundary */
		case TC_FULL:		/* Full frame message */
			dt = *tc;

			/* Zero these out because we will OR in these values from MTC */
			tc->hours = 0;
			tc->minutes = 0;
			tc->seconds = 0;
			tc->frames = 0;

			if (tcStatus == TC_QUARTER) {
				lastQFMessage = millis();
				incrementFrame(&dt, 2); /* We are always two frames behind */
			}
			tcDisplay(&dt);
			break;
		case TC_ERROR:		/* Error, bail out */
		default:
			tcDisableDisplay();
			tcInitialize(tc);
			dt = *tc;
			break;
		}

		displaySync((tc->flags & SMPTE_TIME_RUNNING) != 0);

	}

	tcDisableDisplay();
}

