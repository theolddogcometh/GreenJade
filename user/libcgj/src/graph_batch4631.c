/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4631: kibibytes to bytes (u-suffix, saturating).
 *
 * Surface (unique symbols):
 *   uint64_t gj_kib_to_bytes_u(uint64_t kib);
 *     - Return kib * 1024 (binary KiB → bytes), saturating at
 *       UINT64_MAX on overflow.
 *   uint64_t __gj_kib_to_bytes_u  (alias)
 *   __libcgj_batch4631_marker = "libcgj-batch4631"
 *
 * Exclusive continuum CREATE-ONLY (4631-4640: kib_to_bytes_u,
 * mib_to_bytes_u, gib_to_bytes_u, bytes_to_kib_u, bytes_to_mib_u,
 * bytes_to_gib_u, kb_to_bytes_u SI, mb_to_bytes_u SI, bytes_round_page_u,
 * batch_id_4640). Distinct from gj_kib_to_bytes (batch1581) — unique
 * gj_kib_to_bytes_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4631_marker[] = "libcgj-batch4631";

/* Bytes per kibibyte (2^10). */
#define B4631_BYTES_PER_KIB  1024ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B4631_BYTES_PER_KIB. */
static uint64_t
b4631_mul_kib(uint64_t u64Kib)
{
	if (u64Kib > (UINT64_MAX / B4631_BYTES_PER_KIB)) {
		return UINT64_MAX;
	}
	return u64Kib * B4631_BYTES_PER_KIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kib_to_bytes_u - convert kibibytes to bytes (saturating).
 *
 * kib: count of 1024-byte units.
 *
 * Returns kib * 1024, or UINT64_MAX if the product would overflow.
 * No parent wires.
 */
uint64_t
gj_kib_to_bytes_u(uint64_t u64Kib)
{
	(void)NULL;
	return b4631_mul_kib(u64Kib);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_kib_to_bytes_u(uint64_t u64Kib)
    __attribute__((alias("gj_kib_to_bytes_u")));
