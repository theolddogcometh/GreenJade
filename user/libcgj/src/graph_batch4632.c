/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4632: mebibytes to bytes (u-suffix, saturating).
 *
 * Surface (unique symbols):
 *   uint64_t gj_mib_to_bytes_u(uint64_t mib);
 *     - Return mib * 1048576 (binary MiB → bytes), saturating at
 *       UINT64_MAX on overflow.
 *   uint64_t __gj_mib_to_bytes_u  (alias)
 *   __libcgj_batch4632_marker = "libcgj-batch4632"
 *
 * Exclusive continuum CREATE-ONLY (4631-4640: kib_to_bytes_u,
 * mib_to_bytes_u, gib_to_bytes_u, bytes_to_kib_u, bytes_to_mib_u,
 * bytes_to_gib_u, kb_to_bytes_u SI, mb_to_bytes_u SI, bytes_round_page_u,
 * batch_id_4640). Distinct from gj_mib_to_bytes (batch1582) and
 * gj_mb_to_bytes_u (batch3147 binary) — unique gj_mib_to_bytes_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4632_marker[] = "libcgj-batch4632";

/* Bytes per mebibyte (2^20). */
#define B4632_BYTES_PER_MIB  1048576ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B4632_BYTES_PER_MIB. */
static uint64_t
b4632_mul_mib(uint64_t u64Mib)
{
	if (u64Mib > (UINT64_MAX / B4632_BYTES_PER_MIB)) {
		return UINT64_MAX;
	}
	return u64Mib * B4632_BYTES_PER_MIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mib_to_bytes_u - convert mebibytes to bytes (saturating).
 *
 * mib: count of 1048576-byte units.
 *
 * Returns mib * 1048576, or UINT64_MAX if the product would overflow.
 * No parent wires.
 */
uint64_t
gj_mib_to_bytes_u(uint64_t u64Mib)
{
	(void)NULL;
	return b4632_mul_mib(u64Mib);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_mib_to_bytes_u(uint64_t u64Mib)
    __attribute__((alias("gj_mib_to_bytes_u")));
