/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1747: basis-points-of-u64 resource share.
 *
 * Surface (unique symbols):
 *   uint64_t gj_basis_points_of_u64(uint64_t x, uint64_t bp);
 *     — Return (x * bp) / 10000 (1 bp = 0.01%). Overflow-safe path
 *       when x * bp would exceed UINT64_MAX (divide-first).
 *   uint64_t __gj_basis_points_of_u64  (alias)
 *   __libcgj_batch1747_marker = "libcgj-batch1747"
 *
 * Distinct from gj_basis_points (batch997) and gj_percent_of_u64
 * (batch1746). Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1747_marker[] = "libcgj-batch1747";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1747_bp(uint64_t u64X, uint64_t u64Bp)
{
	if (u64Bp == 0ull || u64X == 0ull) {
		return 0ull;
	}
	if (u64X > UINT64_MAX / u64Bp) {
		return (u64X / 10000ull) * u64Bp +
		    ((u64X % 10000ull) * u64Bp) / 10000ull;
	}
	return (u64X * u64Bp) / 10000ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_basis_points_of_u64 — compute (x * bp) / 10000 for fine shares.
 *
 * u64X:  base resource quantity
 * u64Bp: basis points (10000 = 100%)
 *
 * Overflow-safe integer path; pure arithmetic for cgroup/resource
 * exclusive data math.
 */
uint64_t
gj_basis_points_of_u64(uint64_t u64X, uint64_t u64Bp)
{
	return b1747_bp(u64X, u64Bp);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_basis_points_of_u64(uint64_t u64X, uint64_t u64Bp)
    __attribute__((alias("gj_basis_points_of_u64")));
