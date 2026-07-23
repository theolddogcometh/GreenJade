/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5689: performance overlay sample window.
 *
 * Surface (unique symbols):
 *   uint32_t gj_overlay_sample_window_5689(void);
 *     - Returns stub sample window length in frames (120).
 *   uint32_t __gj_overlay_sample_window_5689  (alias)
 *   __libcgj_batch5689_marker = "libcgj-batch5689"
 *
 * Exclusive continuum CREATE-ONLY (5681-5690: performance overlay
 * metrics stubs — frametime, 1% lows, GPU busy %). Unique
 * gj_overlay_sample_window_5689 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5689_marker[] = "libcgj-batch5689";

/* Rolling sample window length (frames) for overlay metrics. */
#define B5689_SAMPLE_WINDOW_FRAMES  120u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5689_sample_window(void)
{
	return B5689_SAMPLE_WINDOW_FRAMES;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_overlay_sample_window_5689 - report overlay metrics sample window.
 *
 * Always returns 120 (frames). Integer-only stub; does not allocate a
 * ring buffer. Does not call libc. No parent wires.
 */
uint32_t
gj_overlay_sample_window_5689(void)
{
	(void)NULL;
	return b5689_sample_window();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_overlay_sample_window_5689(void)
    __attribute__((alias("gj_overlay_sample_window_5689")));
