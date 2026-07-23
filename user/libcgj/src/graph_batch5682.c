/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5682: performance overlay minimum frametime.
 *
 * Surface (unique symbols):
 *   uint32_t gj_frametime_us_min_5682(void);
 *     - Returns stub minimum frametime in microseconds (8333 ~ 120 FPS).
 *   uint32_t __gj_frametime_us_min_5682  (alias)
 *   __libcgj_batch5682_marker = "libcgj-batch5682"
 *
 * Exclusive continuum CREATE-ONLY (5681-5690: performance overlay
 * metrics stubs — frametime, 1% lows, GPU busy %). Unique
 * gj_frametime_us_min_5682 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5682_marker[] = "libcgj-batch5682";

/* Stub minimum frametime: 8333 us (~120.0 FPS peak). */
#define B5682_FRAMETIME_US_MIN  8333u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5682_frametime_us_min(void)
{
	return B5682_FRAMETIME_US_MIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_frametime_us_min_5682 - report stub minimum frametime (microseconds).
 *
 * Always returns 8333. Integer-only overlay metric stub; does not
 * sample a real present queue. Does not call libc. No parent wires.
 */
uint32_t
gj_frametime_us_min_5682(void)
{
	(void)NULL;
	return b5682_frametime_us_min();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_frametime_us_min_5682(void)
    __attribute__((alias("gj_frametime_us_min_5682")));
