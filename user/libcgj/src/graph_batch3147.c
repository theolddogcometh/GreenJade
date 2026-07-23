/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3147: mebibytes to bytes (u-suffix).
 *
 * Surface (unique symbols):
 *   uint64_t gj_mb_to_bytes_u(uint64_t mb);
 *     - Return mb * 1048576 (binary MiB → bytes), saturating at
 *       UINT64_MAX on overflow.
 *   uint64_t __gj_mb_to_bytes_u  (alias)
 *   __libcgj_batch3147_marker = "libcgj-batch3147"
 *
 * Milestone 3150 exclusive continuum CREATE-ONLY (3141-3150). Distinct
 * from gj_mb_to_bytes (batch991 wrap) and gj_mib_to_bytes (batch1582) —
 * unique gj_mb_to_bytes_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3147_marker[] = "libcgj-batch3147";

/* Bytes per mebibyte (2^20). */
#define B3147_BYTES_PER_MB  1048576ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B3147_BYTES_PER_MB. */
static uint64_t
b3147_mul_mb(uint64_t u64Mb)
{
	if (u64Mb > (UINT64_MAX / B3147_BYTES_PER_MB)) {
		return UINT64_MAX;
	}
	return u64Mb * B3147_BYTES_PER_MB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mb_to_bytes_u - convert mebibytes to bytes (saturating).
 *
 * mb: count of 1048576-byte units.
 *
 * Returns mb * 1048576, or UINT64_MAX if the product would overflow.
 * No parent wires.
 */
uint64_t
gj_mb_to_bytes_u(uint64_t u64Mb)
{
	(void)NULL;
	return b3147_mul_mb(u64Mb);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_mb_to_bytes_u(uint64_t u64Mb)
    __attribute__((alias("gj_mb_to_bytes_u")));
