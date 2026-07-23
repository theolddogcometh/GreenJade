/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch997: basis-points fraction of a u64 value.
 *
 * Surface (unique symbols):
 *   uint64_t gj_basis_points(uint64_t x, uint64_t bp);
 *     — Return (x * bp) / 10000 (1 bp = 0.01%).
 *   uint64_t __gj_basis_points  (alias)
 *   __libcgj_batch997_marker = "libcgj-batch997"
 *
 * Does NOT redefine gj_pct_of_u64 (batch996).
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch997_marker[] = "libcgj-batch997";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_basis_points — compute (x * bp) / 10000.
 *
 * u64X:  base value
 * u64Bp: basis points (10000 = 100%)
 *
 * Intermediate product wraps on overflow like plain C multiply.
 */
uint64_t
gj_basis_points(uint64_t u64X, uint64_t u64Bp)
{
	return (u64X * u64Bp) / 10000ull;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_basis_points(uint64_t u64X, uint64_t u64Bp)
    __attribute__((alias("gj_basis_points")));
