/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1513: constant-time equality mask for uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_ct_eq_mask_u64(uint64_t a, uint64_t b);
 *     — Returns all-bits-one (~0ull) if a == b, else 0. Branch-free on
 *       a/b values (full-width mask suitable for AND/select chains).
 *   uint64_t __gj_ct_eq_mask_u64  (alias)
 *   __libcgj_batch1513_marker = "libcgj-batch1513"
 *
 * Avoid multi-def: gj_ct_eq_u64 / __gj_ct_eq_u64 live in graph_batch1253.c
 * and return 0/1. This TU exports the mask form only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No real crypto copied.
 */

#include <stdint.h>

const char __libcgj_batch1513_marker[] = "libcgj-batch1513";

/*
 * gj_ct_eq_mask_u64 — constant-time equality mask of two 64-bit words.
 *
 * a == b → 0xffffffffffffffffull; else 0.
 */
uint64_t
gj_ct_eq_mask_u64(uint64_t u64A, uint64_t u64B)
{
	uint64_t u64X;
	uint64_t u64Neg;
	uint64_t u64Or;

	u64X = u64A ^ u64B;
	/*
	 * u64X == 0 → equal. Fold to a 0/1 bit without content branches:
	 * (x | -x) has MSB set iff x != 0; invert that bit for equal=1.
	 */
	u64Neg = 0ull - u64X;
	u64Or = u64X | u64Neg;
	/* MSB of u64Or is 1 iff unequal; invert → 1 iff equal. */
	return 0ull - ((u64Or >> 63) ^ 1ull);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ct_eq_mask_u64(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_ct_eq_mask_u64")));
