/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5688: performance overlay visibility lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_overlay_visible_5688(void);
 *     - Returns 1 (overlay visible). Soft pure-data product tag.
 *   uint32_t __gj_overlay_visible_5688  (alias)
 *   __libcgj_batch5688_marker = "libcgj-batch5688"
 *
 * Exclusive continuum CREATE-ONLY (5681-5690: performance overlay
 * metrics stubs — frametime, 1% lows, GPU busy %). Unique
 * gj_overlay_visible_5688 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5688_marker[] = "libcgj-batch5688";

/* Overlay HUD visible by default in this stub wave. */
#define B5688_OVERLAY_VISIBLE  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5688_overlay_visible(void)
{
	return B5688_OVERLAY_VISIBLE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_overlay_visible_5688 - report performance overlay visibility lamp.
 *
 * Always returns 1 (visible). Integer-only stub; does not draw pixels
 * or open a compositor surface. Does not call libc. No parent wires.
 */
uint32_t
gj_overlay_visible_5688(void)
{
	(void)NULL;
	return b5688_overlay_visible();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_overlay_visible_5688(void)
    __attribute__((alias("gj_overlay_visible_5688")));
