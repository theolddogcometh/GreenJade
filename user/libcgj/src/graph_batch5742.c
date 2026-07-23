/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5742: display timing vertical active lines.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vactive_px_5742(void);
 *     - Returns stub vertical active lines (1080, 1080p).
 *   uint32_t __gj_vactive_px_5742  (alias)
 *   __libcgj_batch5742_marker = "libcgj-batch5742"
 *
 * Exclusive continuum CREATE-ONLY (5741-5750: display timings, EDID
 * checksum stub, mode set ready). Unique gj_vactive_px_5742 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5742_marker[] = "libcgj-batch5742";

/* Stub VACTIVE for 1920x1080 mode table entry. */
#define B5742_VACTIVE_PX  1080u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5742_vactive_px(void)
{
	return B5742_VACTIVE_PX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vactive_px_5742 - report stub vertical active line count.
 *
 * Always returns 1080. Integer-only display timing stub; does not
 * program a CRTC. Does not call libc. No parent wires.
 */
uint32_t
gj_vactive_px_5742(void)
{
	(void)NULL;
	return b5742_vactive_px();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vactive_px_5742(void)
    __attribute__((alias("gj_vactive_px_5742")));
