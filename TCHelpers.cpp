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

#include <LedControl.h>
#include <HardwareSerial.h>

#include "TimecodeDisplay.h"

static SMPTE_TIME smpteZeros;

static bool bDisplayState = false;
static bool bSyncLedState = false;

static inline void tcToggleDisplayState(bool bVisible)
{
	if (bDisplayState != bVisible) {
		gLedControl.shutdown(0, !bVisible);
		bDisplayState = bVisible;
	}
}

void displaySync(boolean bSync)
{
	if (bSyncLedState != bSync) {
		digitalWrite(LED_PIN, bSync ? HIGH : LOW);
		bSyncLedState = bSync;
	}
}

#ifdef TC_DEBUG
static void tcDisplaySerial(SMPTE_TIME *tc)
{
	if (tc->hours < 10)
		Serial.print(0, DEC);
	Serial.print(tc->hours, DEC);
	Serial.print(':', BYTE);

	if (tc->minutes < 10)
		Serial.print(0, DEC);
	Serial.print(tc->minutes, DEC);
	Serial.print(':', BYTE);

	if (tc->seconds < 10)
		Serial.print(0, DEC);
	Serial.print(tc->seconds, DEC);
	Serial.print(':', BYTE);

	if (tc->frames < 10)
		Serial.print(0, DEC);
	Serial.print(tc->frames, DEC);

	Serial.print('.', BYTE);
	Serial.println(tc->subframes, DEC);

	digitalWrite(LED_PIN, tc->seconds % 2);
}
#endif /* TC_DEBUG */

static void tcDisplayLed(SMPTE_TIME *tc)
{
	/* Current display state to avoid refreshing if unnecessary */
	static byte previousDigits[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	byte digits[8];
	int i;

	if (tc->subframes != 0) {
		return;
	}

	digits[0] = tc->frames % 10;
	digits[1] = tc->frames / 10;
	digits[2] = tc->seconds % 10;
	digits[3] = tc->seconds / 10;
	digits[4] = tc->minutes % 10;
	digits[5] = tc->minutes / 10;
	digits[6] = tc->hours % 10;
	digits[7] = tc->hours / 10;

	for (i = 0; i < 8; i++) {
		if (digits[i] != previousDigits[i]) {
			bool bDP;

			if (i == 0)
				bDP = isDropFrame(tc);
			else
				bDP = ((i % 2) == 0);

			gLedControl.setDigit(0, i, digits[i], bDP);
			previousDigits[i] = digits[i];
		}
	}

	tcToggleDisplayState(true);
}

#ifdef TC_DEBUG
void tcDebugLed(byte data[4])
{
	gLedControl.setChar(0, 0, (data[3] & 0x0F) >> 0, false);
	gLedControl.setChar(0, 1, (data[3] & 0xF0) >> 4, false);
	gLedControl.setChar(0, 2, (data[2] & 0x0F) >> 0, false);
	gLedControl.setChar(0, 3, (data[2] & 0xF0) >> 4, false);
	gLedControl.setChar(0, 4, (data[1] & 0x0F) >> 0, false);
	gLedControl.setChar(0, 5, (data[1] & 0xF0) >> 4, false);
	gLedControl.setChar(0, 6, (data[0] & 0x0F) >> 0, false);
	gLedControl.setChar(0, 7, (data[0] & 0xF0) >> 4, false);
	tcToggleDisplayState(true);
	delay(1000);
}

void tcDebugLed(byte data)
{
	byte data2[4];

	data2[0] = 0;
	data2[1] = 0;
	data2[2] = 0;
	data2[3] = data;

	tcDebugLed(data2);
}
#endif /* TC_DEBUG */

void tcDisplay(SMPTE_TIME *tc)
{
	/* If we don't have valid timecode, display zeros. */
	if ((tc->flags & SMPTE_TIME_VALID) == 0) {
		tc = &smpteZeros;
	}

#ifdef TC_DEBUG
	if (bDebug)
		tcDisplaySerial(tc);
#endif

	tcDisplayLed(tc);
}

void tcInitialize(SMPTE_TIME *tc, int subframeDivisor, unsigned long type)
{
	tc->subframes = 0;
	tc->subframeDivisor = subframeDivisor;
	tc->counter = 0;
	tc->type = type;
	tc->flags = 0;
	tc->hours = 0;
	tc->minutes = 0;
	tc->seconds = 0;
	tc->frames = 0;
}

boolean isDropFrame(SMPTE_TIME *tc)
{
	boolean drop;

	switch (tc->type) {
	case SMPTE_TIME_TYPE_30_DROP:
	case SMPTE_TIME_TYPE_2997_DROP:
	case SMPTE_TIME_TYPE_60_DROP:
	case SMPTE_TIME_TYPE_5994_DROP:
		drop = true;
		break;
	default:
		drop = false;
		break;
	}

	return drop;
}

static inline unsigned int getFrameDivisor(SMPTE_TIME *tc)
{
	int divisor;

	switch (tc->type) {
	case SMPTE_TIME_TYPE_24:
	case SMPTE_TIME_TYPE_2398:
		divisor = 24;
		break;
	case SMPTE_TIME_TYPE_25:
		divisor = 25;
		break;
	case SMPTE_TIME_TYPE_30:
	case SMPTE_TIME_TYPE_30_DROP:
	case SMPTE_TIME_TYPE_2997:
	case SMPTE_TIME_TYPE_2997_DROP:
		divisor = 30;
		break;
	case SMPTE_TIME_TYPE_60:
	case SMPTE_TIME_TYPE_5994:
	case SMPTE_TIME_TYPE_60_DROP:
	case SMPTE_TIME_TYPE_5994_DROP:
		divisor = 60;
		break;
	case SMPTE_TIME_TYPE_50:
		divisor = 50;
		break;
	default:
		divisor = 24;
		break;
	}

	return divisor;
}

/*
 * Increment tc by specified number of subframes. n must be less
 * than the subframe divisor * frame rate.
 */
void incrementSubframe(SMPTE_TIME *tc, unsigned int n)
{
	while (1) {
		unsigned int frameDivisor;
		unsigned int subframeIncrement;
		unsigned int frameIncrement;

		subframeIncrement = n % tc->subframeDivisor;
		frameIncrement = n / tc->subframeDivisor;

		if (tc->subframes + subframeIncrement < tc->subframeDivisor) {
			tc->subframes += subframeIncrement;

			if (frameIncrement == 0)
				break;
		} else {
			frameIncrement++; /* carry */
		}

		tc->subframes = 0;

		frameDivisor = getFrameDivisor(tc);

		if (tc->frames + frameIncrement < frameDivisor) {
			tc->frames += frameIncrement;
			break;
		}

		tc->frames = 0;

		if (tc->seconds < 59) {
			tc->seconds++;
			break;
		}

		tc->seconds = 0;

		/*
		 * Drop timecode drops the first two frames of each minutes,
		 * except for every tenth minute.
		 */
		if (isDropFrame(tc) && ((tc->minutes + 1) % 10) != 0) {
			tc->frames = 2;
		}

		if (tc->minutes < 59) {
			tc->minutes++;
			break;
		}

		tc->minutes = 0;

		if (tc->hours < 23) {
			tc->hours++;
			break;
		}

		tc->hours = 0;
	}
}

void incrementFrame(SMPTE_TIME *tc, unsigned int n)
{
	incrementSubframe(tc, n * tc->subframeDivisor);
}

boolean tcIsValid(SMPTE_TIME *tc)
{
	if (tc->subframes > tc->subframeDivisor)
		return false;

	if (tc->type > SMPTE_TIME_TYPE_MAX)
		return false;

	if (tc->hours > 23)
		return false;

	if (tc->minutes > 59)
		return false;

	if (tc->seconds > 59)
		return false;

	if (tc->frames > getFrameDivisor(tc) - 1)
		return false;

	return true;
}

void tcEnableDisplay(void)
{
	tcToggleDisplayState(true);
}

void tcDisableDisplay(void)
{
	tcToggleDisplayState(false);
}

void tcInitializeDisplay(void)
{
	gLedControl.setScanLimit(0, 7);
	gLedControl.clearDisplay(0);
	gLedControl.setIntensity(0, 15);
}

