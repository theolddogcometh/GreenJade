/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3148: gibibytes to bytes (u-suffix).
 *
 * Surface (unique symbols):
 *   uint64_t gj_gb_to_bytes_u(uint64_t gb);
 *     - Return gb * 1073741824 (binary GiB → bytes), saturating at
 *       UINT64_MAX on overflow.
 *   uint64_t __gj_gb_to_bytes_u  (alias)
 *   __libcgj_batch3148_marker = "libcgj-batch3148"
 *
 * Milestone 3150 exclusive continuum CREATE-ONLY (3141-3150). Distinct
 * from gj_gb_to_bytes (batch992 wrap) and gj_gib_to_bytes (batch1583) —
 * unique gj_gb_to_bytes_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3148_marker[] = "libcgj-batch3148";

/* Bytes per gibibyte (2^30). */
#define B3148_BYTES_PER_GB  1073741824ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B3148_BYTES_PER_GB. */
static uint64_t
b3148_mul_gb(uint64_t u64Gb)
{
	if (u64Gb > (UINT64_MAX / B3148_BYTES_PER_GB)) {
		return UINT64_MAX;
	}
	return u64Gb * B3148_BYTES_PER_GB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gb_to_bytes_u - convert gibibytes to bytes (saturating).
 *
 * gb: count of 1073741824-byte units.
 *
 * Returns gb * 1073741824, or UINT64_MAX if the product would overflow.
 * No parent wires.
 */
uint64_t
gj_gb_to_bytes_u(uint64_t u64Gb)
{
	(void)NULL;
	return b3148_mul_gb(u64Gb);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_gb_to_bytes_u(uint64_t u64Gb)
    __attribute__((alias("gj_gb_to_bytes_u")));
