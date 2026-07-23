/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4633: gibibytes to bytes (u-suffix, saturating).
 *
 * Surface (unique symbols):
 *   uint64_t gj_gib_to_bytes_u(uint64_t gib);
 *     - Return gib * 1073741824 (binary GiB → bytes), saturating at
 *       UINT64_MAX on overflow.
 *   uint64_t __gj_gib_to_bytes_u  (alias)
 *   __libcgj_batch4633_marker = "libcgj-batch4633"
 *
 * Exclusive continuum CREATE-ONLY (4631-4640: kib_to_bytes_u,
 * mib_to_bytes_u, gib_to_bytes_u, bytes_to_kib_u, bytes_to_mib_u,
 * bytes_to_gib_u, kb_to_bytes_u SI, mb_to_bytes_u SI, bytes_round_page_u,
 * batch_id_4640). Distinct from gj_gib_to_bytes (batch1583) and
 * gj_gb_to_bytes_u (batch3148) — unique gj_gib_to_bytes_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4633_marker[] = "libcgj-batch4633";

/* Bytes per gibibyte (2^30). */
#define B4633_BYTES_PER_GIB  1073741824ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B4633_BYTES_PER_GIB. */
static uint64_t
b4633_mul_gib(uint64_t u64Gib)
{
	if (u64Gib > (UINT64_MAX / B4633_BYTES_PER_GIB)) {
		return UINT64_MAX;
	}
	return u64Gib * B4633_BYTES_PER_GIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gib_to_bytes_u - convert gibibytes to bytes (saturating).
 *
 * gib: count of 1073741824-byte units.
 *
 * Returns gib * 1073741824, or UINT64_MAX if the product would overflow.
 * No parent wires.
 */
uint64_t
gj_gib_to_bytes_u(uint64_t u64Gib)
{
	(void)NULL;
	return b4633_mul_gib(u64Gib);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_gib_to_bytes_u(uint64_t u64Gib)
    __attribute__((alias("gj_gib_to_bytes_u")));
