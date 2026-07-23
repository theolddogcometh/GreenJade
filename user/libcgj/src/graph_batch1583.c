/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1583: gibibytes to bytes.
 *
 * Surface (unique symbols):
 *   uint64_t gj_gib_to_bytes(uint64_t gib);
 *     — Return gib * 1073741824 (binary GiB → bytes), saturating at
 *       UINT64_MAX on overflow.
 *   uint64_t __gj_gib_to_bytes  (alias)
 *   __libcgj_batch1583_marker = "libcgj-batch1583"
 *
 * Distinct from gj_gb_to_bytes (batch992) — explicit GiB surface only.
 * Does NOT redefine gj_kib_to_bytes / gj_mib_to_bytes.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1583_marker[] = "libcgj-batch1583";

/* Bytes per gibibyte (2^30). */
#define B1583_BYTES_PER_GIB  1073741824ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B1583_BYTES_PER_GIB. */
static uint64_t
b1583_mul_gib(uint64_t u64Gib)
{
	if (u64Gib > (UINT64_MAX / B1583_BYTES_PER_GIB)) {
		return UINT64_MAX;
	}
	return u64Gib * B1583_BYTES_PER_GIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gib_to_bytes — convert gibibytes to bytes (saturating).
 *
 * u64Gib: count of 1073741824-byte units
 * Returns u64Gib * 1073741824, or UINT64_MAX if the product would overflow.
 */
uint64_t
gj_gib_to_bytes(uint64_t u64Gib)
{
	return b1583_mul_gib(u64Gib);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_gib_to_bytes(uint64_t u64Gib)
    __attribute__((alias("gj_gib_to_bytes")));
