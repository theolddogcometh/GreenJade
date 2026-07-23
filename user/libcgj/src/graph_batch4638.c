/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4638: SI megabytes to bytes (u-suffix, sat).
 *
 * Surface (unique symbols):
 *   uint64_t gj_si_mb_to_bytes_u(uint64_t mb);
 *     - Return mb * 1000000 (SI decimal MB → bytes), saturating at
 *       UINT64_MAX on overflow.
 *   uint64_t __gj_si_mb_to_bytes_u  (alias)
 *   __libcgj_batch4638_marker = "libcgj-batch4638"
 *
 * Exclusive continuum CREATE-ONLY (4631-4640: kib_to_bytes_u,
 * mib_to_bytes_u, gib_to_bytes_u, bytes_to_kib_u, bytes_to_mib_u,
 * bytes_to_gib_u, si_kb_to_bytes_u, si_mb_to_bytes_u, bytes_round_page_u,
 * batch_id_4640).
 *
 * Collision note: theme listed gj_mb_to_bytes_u, but that symbol already
 * exists in batch3147 as binary *1048576. This TU exports the SI
 * *1000000 surface as gj_si_mb_to_bytes_u only — no multi-def. Distinct
 * from gj_mb_to_bytes (batch991) and gj_mib_to_bytes_u (batch4632). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4638_marker[] = "libcgj-batch4638";

/* Bytes per SI megabyte (10^6). */
#define B4638_BYTES_PER_MB  1000000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B4638_BYTES_PER_MB. */
static uint64_t
b4638_mul_mb(uint64_t u64Mb)
{
	if (u64Mb > (UINT64_MAX / B4638_BYTES_PER_MB)) {
		return UINT64_MAX;
	}
	return u64Mb * B4638_BYTES_PER_MB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_si_mb_to_bytes_u - convert SI megabytes to bytes (saturating).
 *
 * mb: count of 1000000-byte units.
 *
 * Returns mb * 1000000, or UINT64_MAX if the product would overflow.
 * No parent wires.
 */
uint64_t
gj_si_mb_to_bytes_u(uint64_t u64Mb)
{
	(void)NULL;
	return b4638_mul_mb(u64Mb);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_si_mb_to_bytes_u(uint64_t u64Mb)
    __attribute__((alias("gj_si_mb_to_bytes_u")));
