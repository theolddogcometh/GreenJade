/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2948: Fibonacci F(n) mod 2^64 (_u surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_fib_u64_u(unsigned n);
 *     — F(n) modulo 2^64 (natural uint64 wrap). F(0)=0, F(1)=1.
 *       O(log n) multiplications via classic fast-doubling identities:
 *         F(2k)   = F(k) * (2*F(k+1) - F(k))
 *         F(2k+1) = F(k)^2 + F(k+1)^2
 *   uint64_t __gj_fib_u64_u  (alias)
 *   __libcgj_batch2948_marker = "libcgj-batch2948"
 *
 * CREATE-ONLY exclusive continuum wave (2941–2950). Distinct from
 * gj_fib_u64 (batch235) and gj_fib_u32 (batch848) — unique
 * gj_fib_u64_u name; no parent wires, no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2948_marker[] = "libcgj-batch2948";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2948_fib_pair — recursive fast doubling.
 *
 * On return: *a = F(n), *b = F(n+1), both mod 2^64.
 * All arithmetic is uint64_t; wraparound is the modular ring.
 */
static void
b2948_fib_pair(unsigned uN, uint64_t *pu64A, uint64_t *pu64B)
{
	uint64_t u64C;
	uint64_t u64D;
	uint64_t u64E;
	uint64_t u64F;

	if (uN == 0u) {
		*pu64A = 0ull;
		*pu64B = 1ull;
		return;
	}

	b2948_fib_pair(uN >> 1, &u64C, &u64D);
	/* c = F(k), d = F(k+1), k = n/2 */

	/* F(2k)   = F(k) * (2*F(k+1) - F(k))  mod 2^64 */
	u64E = u64C * ((u64D << 1) - u64C);
	/* F(2k+1) = F(k)^2 + F(k+1)^2         mod 2^64 */
	u64F = u64C * u64C + u64D * u64D;

	if ((uN & 1u) != 0u) {
		*pu64A = u64F;           /* F(2k+1) */
		*pu64B = u64E + u64F;    /* F(2k+2) */
	} else {
		*pu64A = u64E;           /* F(2k) */
		*pu64B = u64F;           /* F(2k+1) */
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fib_u64_u — F(n) mod 2^64 via fast doubling.
 *
 * Returns the n-th Fibonacci number reduced modulo 2^64.
 * F(0) = 0, F(1) = 1, F(2) = 1, ...
 * For large n the value is the natural wrap of unsigned 64-bit mul/add.
 */
uint64_t
gj_fib_u64_u(unsigned uN)
{
	uint64_t u64A;
	uint64_t u64B;

	(void)NULL;
	b2948_fib_pair(uN, &u64A, &u64B);
	(void)u64B;
	return u64A;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_fib_u64_u(unsigned uN)
    __attribute__((alias("gj_fib_u64_u")));
