/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6248: limited-range 8-bit C validity stub.
 *
 * Surface (unique symbols):
 *   int gj_cs_c_limited8_ok_6248(uint8_t c);
 *     - Return 1 if c is in the classic studio limited-range chroma
 *       window [16, 240] (BT.709 / BT.2020 8-bit Cb/Cr), else 0.
 *   int __gj_cs_c_limited8_ok_6248  (alias)
 *   __libcgj_batch6248_marker = "libcgj-batch6248"
 *
 * Exclusive continuum CREATE-ONLY (6241-6250: color space BT.709 /
 * BT.2020 stubs). Unique gj_cs_c_limited8_ok_6248 surface only; no
 * multi-def. Distinct from gj_cs_y_limited8_ok_6247. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6248_marker[] = "libcgj-batch6248";

/* Studio limited-range 8-bit chroma window (ITU-R). */
#define B6248_C_MIN  16u
#define B6248_C_MAX  240u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6248_c_ok(uint8_t u8C)
{
	if (u8C < B6248_C_MIN) {
		return 0;
	}
	if (u8C > B6248_C_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cs_c_limited8_ok_6248 - 1 if c is in limited-range [16, 240].
 *
 * Screens 8-bit Cb/Cr samples for BT.709 / BT.2020 limited-range
 * pipelines. Pure predicate; no YUV plane access. Does not call libc.
 * No parent wires.
 */
int
gj_cs_c_limited8_ok_6248(uint8_t u8C)
{
	(void)NULL;
	return b6248_c_ok(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cs_c_limited8_ok_6248(uint8_t u8C)
    __attribute__((alias("gj_cs_c_limited8_ok_6248")));
