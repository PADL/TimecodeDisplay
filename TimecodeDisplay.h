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

#ifndef TimecodeDisplay_h
#define TimecodeDisplay_h		1

#include <WConstants.h>
#include <LedControl.h>

#include "SMPTETypes.h"
#include "MTCTypes.h"

#define TC_DEBUG	1

#define LED_PIN		13

extern SMPTE_TIME gLocation;
extern LedControl gLedControl;
#ifdef TC_DEBUG
extern boolean bDebug;
#endif

/* TCFollow.cpp */
void tcFollow(SMPTE_TIME *tc);

/* TCFree.cpp */
void tcFree(SMPTE_TIME *tc);

/* TCHelpers.cpp */
void tcInitialize(SMPTE_TIME *tc, int subframeDivisor = 4, unsigned long type = SMPTE_TIME_TYPE_24);
void tcDisplay(SMPTE_TIME *tc);
void tcEnableDisplay(void);
void tcDisableDisplay(void);
void tcInitializeDisplay(void);
boolean isDropFrame(SMPTE_TIME *tc);
void displaySync(boolean bSync);
void incrementSubframe(SMPTE_TIME *tc, unsigned int n = 1);
void incrementFrame(SMPTE_TIME *tc, unsigned int n = 1);
boolean tcIsValid(SMPTE_TIME *tc);
#ifdef TC_DEBUG
void tcDebugLed(byte value);
void tcDebugLed(byte values[4]);
#endif

#ifdef TC_DEBUG
#define TC_TRACE(err, arg)	do { \
		byte traceMsg[4] = { (err), 0, 0, (arg) }; \
		tcDebugLed(traceMsg); \
	} while (0)
#else
#define TC_TRACE(err, arg)
#endif /* TC_DEBUG */

#endif /* TimecodeDisplay_h */

