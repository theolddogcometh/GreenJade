/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3146: kibibytes to bytes (u-suffix).
 *
 * Surface (unique symbols):
 *   uint64_t gj_kb_to_bytes_u(uint64_t kb);
 *     - Return kb * 1024 (binary KiB → bytes), saturating at
 *       UINT64_MAX on overflow.
 *   uint64_t __gj_kb_to_bytes_u  (alias)
 *   __libcgj_batch3146_marker = "libcgj-batch3146"
 *
 * Milestone 3150 exclusive continuum CREATE-ONLY (3141-3150). Distinct
 * from gj_kb_to_bytes (batch989 wrap) and gj_kib_to_bytes (batch1581) —
 * unique gj_kb_to_bytes_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3146_marker[] = "libcgj-batch3146";

/* Bytes per kibibyte (2^10). */
#define B3146_BYTES_PER_KB  1024ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B3146_BYTES_PER_KB. */
static uint64_t
b3146_mul_kb(uint64_t u64Kb)
{
	if (u64Kb > (UINT64_MAX / B3146_BYTES_PER_KB)) {
		return UINT64_MAX;
	}
	return u64Kb * B3146_BYTES_PER_KB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kb_to_bytes_u - convert kibibytes to bytes (saturating).
 *
 * kb: count of 1024-byte units.
 *
 * Returns kb * 1024, or UINT64_MAX if the product would overflow.
 * No parent wires.
 */
uint64_t
gj_kb_to_bytes_u(uint64_t u64Kb)
{
	(void)NULL;
	return b3146_mul_kb(u64Kb);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_kb_to_bytes_u(uint64_t u64Kb)
    __attribute__((alias("gj_kb_to_bytes_u")));
