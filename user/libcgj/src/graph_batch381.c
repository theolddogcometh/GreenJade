/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch381: floor average of two uint64_t values
 * without intermediate overflow.
 *
 * Surface (unique symbols):
 *   uint64_t gj_avg_u64(uint64_t a, uint64_t b);
 *     — Floor of (a + b) / 2 computed without overflow of a + b.
 *       Equivalent to floor((a + b) / 2) for all a, b in [0, UINT64_MAX],
 *       including cases where a + b would wrap a 64-bit sum.
 *   uint64_t __gj_avg_u64  (alias)
 *   __libcgj_batch381_marker = "libcgj-batch381"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch381_marker[] = "libcgj-batch381";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_avg_u64 — floor((a + b) / 2) without overflow.
 *
 * Identity: (a & b) + ((a ^ b) >> 1)
 *   bits set in both contribute fully; differing bits contribute half
 *   of their XOR (carry-free floor of the unpaired bits).
 */
uint64_t
gj_avg_u64(uint64_t u64A, uint64_t u64B)
{
	return (u64A & u64B) + ((u64A ^ u64B) >> 1);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_avg_u64(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_avg_u64")));
