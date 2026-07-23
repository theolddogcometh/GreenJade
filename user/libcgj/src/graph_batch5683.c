/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5683: performance overlay maximum frametime.
 *
 * Surface (unique symbols):
 *   uint32_t gj_frametime_us_max_5683(void);
 *     - Returns stub maximum frametime in microseconds (33333 ~ 30 FPS).
 *   uint32_t __gj_frametime_us_max_5683  (alias)
 *   __libcgj_batch5683_marker = "libcgj-batch5683"
 *
 * Exclusive continuum CREATE-ONLY (5681-5690: performance overlay
 * metrics stubs — frametime, 1% lows, GPU busy %). Unique
 * gj_frametime_us_max_5683 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5683_marker[] = "libcgj-batch5683";

/* Stub maximum frametime: 33333 us (~30.0 FPS floor spike). */
#define B5683_FRAMETIME_US_MAX  33333u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5683_frametime_us_max(void)
{
	return B5683_FRAMETIME_US_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_frametime_us_max_5683 - report stub maximum frametime (microseconds).
 *
 * Always returns 33333. Integer-only overlay metric stub; does not
 * sample a real present queue. Does not call libc. No parent wires.
 */
uint32_t
gj_frametime_us_max_5683(void)
{
	(void)NULL;
	return b5683_frametime_us_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_frametime_us_max_5683(void)
    __attribute__((alias("gj_frametime_us_max_5683")));
