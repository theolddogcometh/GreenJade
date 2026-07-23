/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2947: uint32 Fibonacci; overflow → 0 (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fib_u32_u(unsigned n);
 *     — F(0)=0, F(1)=1. If F(n) does not fit in uint32_t → 0.
 *   uint32_t __gj_fib_u32_u  (alias)
 *   __libcgj_batch2947_marker = "libcgj-batch2947"
 *
 * CREATE-ONLY exclusive continuum wave (2941–2950). Distinct from
 * gj_fib_u32 (batch848) and gj_fib_u64 (batch235) — unique
 * gj_fib_u32_u name; no parent wires, no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2947_marker[] = "libcgj-batch2947";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * F(n) if it fits in uint32_t, else 0.
 * F(47) = 2971215073 is the largest Fibonacci number in uint32_t.
 * F(48) overflows → 0. Iterative add with pre-add overflow check.
 */
static uint32_t
b2947_fib(unsigned uN)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fib_u32_u — F(n) if it fits in uint32_t, else 0.
 *
 * F(0)=0, F(1)=1. Overflow of F(n) → 0 (soft fail).
 */
uint32_t
gj_fib_u32_u(unsigned uN)
{
	(void)NULL;
	return b2947_fib(uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fib_u32_u(unsigned uN)
    __attribute__((alias("gj_fib_u32_u")));
