/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5748: display timing VSYNC pulse width.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vsync_width_5748(void);
 *     - Returns stub vertical sync width in lines (5, 1080p60).
 *   uint32_t __gj_vsync_width_5748  (alias)
 *   __libcgj_batch5748_marker = "libcgj-batch5748"
 *
 * Exclusive continuum CREATE-ONLY (5741-5750: display timings, EDID
 * checksum stub, mode set ready). Unique gj_vsync_width_5748 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5748_marker[] = "libcgj-batch5748";

/* Stub VSYNC width for 1080p60 CEA blanking. */
#define B5748_VSYNC_WIDTH  5u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5748_vsync_width(void)
{
	return B5748_VSYNC_WIDTH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vsync_width_5748 - report stub vertical sync pulse width.
 *
 * Always returns 5. Integer-only display timing stub; does not
 * program a CRTC. Does not call libc. No parent wires.
 */
uint32_t
gj_vsync_width_5748(void)
{
	(void)NULL;
	return b5748_vsync_width();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vsync_width_5748(void)
    __attribute__((alias("gj_vsync_width_5748")));
