/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5681: performance overlay average frametime.
 *
 * Surface (unique symbols):
 *   uint32_t gj_frametime_us_avg_5681(void);
 *     - Returns stub average frametime in microseconds (16667 ~ 60 FPS).
 *   uint32_t __gj_frametime_us_avg_5681  (alias)
 *   __libcgj_batch5681_marker = "libcgj-batch5681"
 *
 * Exclusive continuum CREATE-ONLY (5681-5690: performance overlay
 * metrics stubs — frametime, 1% lows, GPU busy %). Unique
 * gj_frametime_us_avg_5681 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5681_marker[] = "libcgj-batch5681";

/* Stub average frametime: 16667 us (~60.0 FPS). */
#define B5681_FRAMETIME_US_AVG  16667u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5681_frametime_us_avg(void)
{
	return B5681_FRAMETIME_US_AVG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_frametime_us_avg_5681 - report stub average frametime (microseconds).
 *
 * Always returns 16667. Integer-only overlay metric stub; does not
 * sample a real present queue. Does not call libc. No parent wires.
 */
uint32_t
gj_frametime_us_avg_5681(void)
{
	(void)NULL;
	return b5681_frametime_us_avg();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_frametime_us_avg_5681(void)
    __attribute__((alias("gj_frametime_us_avg_5681")));
