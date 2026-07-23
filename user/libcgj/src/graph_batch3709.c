/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3709: buffer bytes needed for u32 decimal string.
 *
 * Surface (unique symbols):
 *   size_t gj_u32_to_dec_need_u(uint32_t n);
 *     - Bytes needed (including trailing NUL) to write the base-10
 *       representation of n as a NUL-terminated string with no leading
 *       zeros except for n == 0. Range is [2, 11]:
 *         0          → 2  ("0" + NUL)
 *         42         → 3  ("42" + NUL)
 *         UINT32_MAX → 11 ("4294967295" + NUL)
 *   size_t __gj_u32_to_dec_need_u  (alias)
 *   __libcgj_batch3709_marker = "libcgj-batch3709"
 *
 * Exclusive continuum CREATE-ONLY (3701-3710). Distinct from
 * gj_u32_digit_count_u (batch3705 digit count only) — this surface is
 * buffer-need (digits + NUL). Unique to_dec_need_u; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3709_marker[] = "libcgj-batch3709";

/* ---- freestanding helpers ---------------------------------------------- */

/* Base-10 digit length of n; 1 when n == 0. */
static unsigned
b3709_digit_count(uint32_t uN)
{
	if (uN < 10u) {
		return 1u;
	}
	if (uN < 100u) {
		return 2u;
	}
	if (uN < 1000u) {
		return 3u;
	}
	if (uN < 10000u) {
		return 4u;
	}
	if (uN < 100000u) {
		return 5u;
	}
	if (uN < 1000000u) {
		return 6u;
	}
	if (uN < 10000000u) {
		return 7u;
	}
	if (uN < 100000000u) {
		return 8u;
	}
	if (uN < 1000000000u) {
		return 9u;
	}
	return 10u;
}

/* Bytes needed including trailing NUL. */
static size_t
b3709_to_dec_need(uint32_t uN)
{
	return (size_t)b3709_digit_count(uN) + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_to_dec_need_u - bytes needed (incl. NUL) for decimal of n.
 *
 * n: unsigned 32-bit value
 *
 * Returns digit_count(n) + 1 in [2, 11]. No parent wires.
 */
size_t
gj_u32_to_dec_need_u(uint32_t n)
{
	(void)NULL;
	return b3709_to_dec_need(n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_to_dec_need_u(uint32_t n)
    __attribute__((alias("gj_u32_to_dec_need_u")));
