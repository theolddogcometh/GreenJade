/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5741: display timing horizontal active pixels.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hactive_px_5741(void);
 *     - Returns stub horizontal active pixels (1920, 1080p).
 *   uint32_t __gj_hactive_px_5741  (alias)
 *   __libcgj_batch5741_marker = "libcgj-batch5741"
 *
 * Exclusive continuum CREATE-ONLY (5741-5750: display timings, EDID
 * checksum stub, mode set ready). Unique gj_hactive_px_5741 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5741_marker[] = "libcgj-batch5741";

/* Stub HACTIVE for 1920x1080 mode table entry. */
#define B5741_HACTIVE_PX  1920u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5741_hactive_px(void)
{
	return B5741_HACTIVE_PX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hactive_px_5741 - report stub horizontal active pixel count.
 *
 * Always returns 1920. Integer-only display timing stub; does not
 * program a CRTC. Does not call libc. No parent wires.
 */
uint32_t
gj_hactive_px_5741(void)
{
	(void)NULL;
	return b5741_hactive_px();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hactive_px_5741(void)
    __attribute__((alias("gj_hactive_px_5741")));
