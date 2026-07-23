/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4637: SI kilobytes to bytes (u-suffix, sat).
 *
 * Surface (unique symbols):
 *   uint64_t gj_si_kb_to_bytes_u(uint64_t kb);
 *     - Return kb * 1000 (SI decimal kB → bytes), saturating at
 *       UINT64_MAX on overflow.
 *   uint64_t __gj_si_kb_to_bytes_u  (alias)
 *   __libcgj_batch4637_marker = "libcgj-batch4637"
 *
 * Exclusive continuum CREATE-ONLY (4631-4640: kib_to_bytes_u,
 * mib_to_bytes_u, gib_to_bytes_u, bytes_to_kib_u, bytes_to_mib_u,
 * bytes_to_gib_u, si_kb_to_bytes_u, si_mb_to_bytes_u, bytes_round_page_u,
 * batch_id_4640).
 *
 * Collision note: theme listed gj_kb_to_bytes_u, but that symbol already
 * exists in batch3146 as binary *1024. This TU exports the SI *1000
 * surface as gj_si_kb_to_bytes_u only — no multi-def. Distinct from
 * gj_kb_to_bytes (batch989) and gj_kib_to_bytes_u (batch4631). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4637_marker[] = "libcgj-batch4637";

/* Bytes per SI kilobyte (10^3). */
#define B4637_BYTES_PER_KB  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B4637_BYTES_PER_KB. */
static uint64_t
b4637_mul_kb(uint64_t u64Kb)
{
	if (u64Kb > (UINT64_MAX / B4637_BYTES_PER_KB)) {
		return UINT64_MAX;
	}
	return u64Kb * B4637_BYTES_PER_KB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_si_kb_to_bytes_u - convert SI kilobytes to bytes (saturating).
 *
 * kb: count of 1000-byte units.
 *
 * Returns kb * 1000, or UINT64_MAX if the product would overflow.
 * No parent wires.
 */
uint64_t
gj_si_kb_to_bytes_u(uint64_t u64Kb)
{
	(void)NULL;
	return b4637_mul_kb(u64Kb);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_si_kb_to_bytes_u(uint64_t u64Kb)
    __attribute__((alias("gj_si_kb_to_bytes_u")));
