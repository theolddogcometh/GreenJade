/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5684: performance overlay 1% low FPS.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fps_1pct_low_5684(void);
 *     - Returns stub 1% low FPS (48). Soft pure-data overlay tag.
 *   uint32_t __gj_fps_1pct_low_5684  (alias)
 *   __libcgj_batch5684_marker = "libcgj-batch5684"
 *
 * Exclusive continuum CREATE-ONLY (5681-5690: performance overlay
 * metrics stubs — frametime, 1% lows, GPU busy %). Unique
 * gj_fps_1pct_low_5684 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5684_marker[] = "libcgj-batch5684";

/* Stub 1% low FPS (whole frames per second). */
#define B5684_FPS_1PCT_LOW  48u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5684_fps_1pct_low(void)
{
	return B5684_FPS_1PCT_LOW;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fps_1pct_low_5684 - report stub 1% low frames-per-second.
 *
 * Always returns 48. Integer-only overlay metric stub; does not rank a
 * real frametime histogram. Does not call libc. No parent wires.
 */
uint32_t
gj_fps_1pct_low_5684(void)
{
	(void)NULL;
	return b5684_fps_1pct_low();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fps_1pct_low_5684(void)
    __attribute__((alias("gj_fps_1pct_low_5684")));
