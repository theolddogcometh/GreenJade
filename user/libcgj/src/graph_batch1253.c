/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1253: constant-time equality of two uint64_t.
 *
 * Surface (unique symbols):
 *   int gj_ct_eq_u64(uint64_t a, uint64_t b);
 *     — 1 if a == b, else 0; no data-dependent branches on a/b.
 *   int __gj_ct_eq_u64  (alias)
 *   __libcgj_batch1253_marker = "libcgj-batch1253"
 *
 * Distinct from gj_ct_eq (buffer) — unique scalar surface.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1253_marker[] = "libcgj-batch1253";

/*
 * gj_ct_eq_u64 — constant-time equality of two 64-bit words.
 */
int
gj_ct_eq_u64(uint64_t u64A, uint64_t u64B)
{
	uint64_t u64X;
	uint32_t u32Or;

	u64X = u64A ^ u64B;
	u32Or = (uint32_t)u64X | (uint32_t)(u64X >> 32);

	/*
	 * u32Or==0 → 1; nonzero → 0.
	 * (u32Or | (0u - u32Or)) >> 31 is 0 iff u32Or==0, else 1;
	 * XOR with 1 inverts.
	 */
	return (int)((((u32Or | (0u - u32Or)) >> 31) ^ 1u) & 1u);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ct_eq_u64(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_ct_eq_u64")));
