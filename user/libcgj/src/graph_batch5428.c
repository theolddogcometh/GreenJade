/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5428: texture format support bitmask probe.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tex_format_supported_5428(uint32_t support_mask,
 *                                         uint32_t format_code);
 *     - Return 1 when bit format_code is set in support_mask
 *       (format_code in 0..31), else 0. Soft freestanding model of
 *       Deck GPU texture format capability queries.
 *   uint32_t __gj_tex_format_supported_5428  (alias)
 *   __libcgj_batch5428_marker = "libcgj-batch5428"
 *
 * Exclusive continuum CREATE-ONLY (5421-5430). Deck GPU path: OpenGL ES
 * / GLX / EGL / shader / texture stubs. Unique gj_*_5428 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5428_marker[] = "libcgj-batch5428";

/* Support mask width in bits (uint32_t capability word). */
#define B5428_FMT_BITS  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5428_supported(uint32_t uMask, uint32_t uFmt)
{
	uint32_t uBit;

	if (uFmt >= B5428_FMT_BITS) {
		return 0u;
	}
	uBit = 1u << uFmt;
	return ((uMask & uBit) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tex_format_supported_5428 - test a format bit in a support mask.
 *
 * support_mask: capability word (bit N = format N supported).
 * format_code:  format index in 0..31; >= 32 always yields 0.
 * Returns 1 if supported, else 0. Freestanding Deck GPU texture path
 * helper. No parent wires.
 */
uint32_t
gj_tex_format_supported_5428(uint32_t uMask, uint32_t uFmt)
{
	(void)NULL;
	return b5428_supported(uMask, uFmt);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tex_format_supported_5428(uint32_t uMask, uint32_t uFmt)
    __attribute__((alias("gj_tex_format_supported_5428")));
