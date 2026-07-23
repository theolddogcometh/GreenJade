/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch848: uint32 Fibonacci; overflow → 0.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fib_u32(unsigned n);
 *     — F(0)=0, F(1)=1. If F(n) does not fit in uint32_t → 0.
 *   uint32_t __gj_fib_u32  (alias)
 *   __libcgj_batch848_marker = "libcgj-batch848"
 *
 * Distinct from gj_fib_u64 (batch235) which wraps — this TU saturates to 0.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch848_marker[] = "libcgj-batch848";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fib_u32 — F(n) if it fits in uint32_t, else 0.
 *
 * F(47) = 2971215073 is the largest Fibonacci number in uint32_t.
 * F(48) overflows → 0. Iterative add with pre-add overflow check.
 */
uint32_t
gj_fib_u32(unsigned uN)
{
	uint32_t u32A;
	uint32_t u32B;
	unsigned uI;

	if (uN == 0u) {
		return 0u;
	}
	if (uN == 1u) {
		return 1u;
	}

	u32A = 0u;
	u32B = 1u;
	for (uI = 2u; uI <= uN; uI++) {
		uint32_t u32Next;

		if (u32B > 0xffffffffu - u32A) {
			return 0u; /* F(i) would overflow */
		}
		u32Next = u32A + u32B;
		u32A = u32B;
		u32B = u32Next;
	}
	return u32B;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fib_u32(unsigned uN)
    __attribute__((alias("gj_fib_u32")));
