/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5747: display timing HSYNC pulse width.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hsync_width_5747(void);
 *     - Returns stub horizontal sync width in pixels (44, 1080p60).
 *   uint32_t __gj_hsync_width_5747  (alias)
 *   __libcgj_batch5747_marker = "libcgj-batch5747"
 *
 * Exclusive continuum CREATE-ONLY (5741-5750: display timings, EDID
 * checksum stub, mode set ready). Unique gj_hsync_width_5747 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5747_marker[] = "libcgj-batch5747";

/* Stub HSYNC width for 1080p60 CEA blanking. */
#define B5747_HSYNC_WIDTH  44u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5747_hsync_width(void)
{
	return B5747_HSYNC_WIDTH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hsync_width_5747 - report stub horizontal sync pulse width.
 *
 * Always returns 44. Integer-only display timing stub; does not
 * program a CRTC. Does not call libc. No parent wires.
 */
uint32_t
gj_hsync_width_5747(void)
{
	(void)NULL;
	return b5747_hsync_width();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hsync_width_5747(void)
    __attribute__((alias("gj_hsync_width_5747")));
