/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6247: limited-range 8-bit Y validity stub.
 *
 * Surface (unique symbols):
 *   int gj_cs_y_limited8_ok_6247(uint8_t y);
 *     - Return 1 if y is in the classic studio limited-range window
 *       [16, 235] (BT.709 / BT.2020 8-bit luma footroom/headroom), else 0.
 *   int __gj_cs_y_limited8_ok_6247  (alias)
 *   __libcgj_batch6247_marker = "libcgj-batch6247"
 *
 * Exclusive continuum CREATE-ONLY (6241-6250: color space BT.709 /
 * BT.2020 stubs). Unique gj_cs_y_limited8_ok_6247 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6247_marker[] = "libcgj-batch6247";

/* Studio limited-range 8-bit luma window (ITU-R BT.601/709/2020). */
#define B6247_Y_MIN  16u
#define B6247_Y_MAX  235u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6247_y_ok(uint8_t u8Y)
{
	if (u8Y < B6247_Y_MIN) {
		return 0;
	}
	if (u8Y > B6247_Y_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cs_y_limited8_ok_6247 - 1 if y is in limited-range [16, 235].
 *
 * Screens 8-bit luma samples for BT.709 / BT.2020 limited-range
 * pipelines before matrix / range conversion stubs. Pure predicate.
 * Does not call libc. No parent wires.
 */
int
gj_cs_y_limited8_ok_6247(uint8_t u8Y)
{
	(void)NULL;
	return b6247_y_ok(u8Y);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cs_y_limited8_ok_6247(uint8_t u8Y)
    __attribute__((alias("gj_cs_y_limited8_ok_6247")));
