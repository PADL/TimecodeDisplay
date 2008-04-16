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

#include <HardwareSerial.cpp>

#include "TimecodeDisplay.h"

static unsigned long timeToNextSubframe(SMPTE_TIME *tc)
{
	int divisor;
	unsigned long interval;

	switch (tc->type) {
	case SMPTE_TIME_TYPE_24:
		divisor = 2400;
		break;
	case SMPTE_TIME_TYPE_2398:
		divisor = 2398;
		break;
	case SMPTE_TIME_TYPE_25:
		divisor = 2500;
		break;
	case SMPTE_TIME_TYPE_30:
	case SMPTE_TIME_TYPE_30_DROP:
		divisor = 3000;
		break;
	case SMPTE_TIME_TYPE_2997:
	case SMPTE_TIME_TYPE_2997_DROP:
		divisor = 2997;
		break;
	case SMPTE_TIME_TYPE_60:
	case SMPTE_TIME_TYPE_60_DROP:
		divisor = 6000;
		break;
	case SMPTE_TIME_TYPE_5994:
	case SMPTE_TIME_TYPE_5994_DROP:
		divisor = 5994;
		break;
	case SMPTE_TIME_TYPE_50:
		divisor = 5000;
		break;
	default:
		divisor = 2400;
		break;
	}

	divisor *= tc->subframeDivisor;

	interval = 1000UL * 100UL / (unsigned long)divisor;

	return interval;
}

void tcFree(SMPTE_TIME *tc)
{
	unsigned long interval;

	tcInitialize(tc, 1); /* no need for quarter frame accuracy here */

	tc->flags = SMPTE_TIME_VALID | SMPTE_TIME_RUNNING;

	interval = timeToNextSubframe(tc);

	for (tc->counter = 0; ; tc->counter++) {
		tcDisplay(tc);
		delay(interval);
		incrementFrame(tc);
	}

	tcDisableDisplay();
}

