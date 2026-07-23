/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch235: Fibonacci F(n) mod 2^64 via fast doubling.
 *
 * Surface (unique symbols):
 *   uint64_t gj_fib_u64(unsigned n);
 *     — F(n) modulo 2^64 (natural uint64 wrap). F(0)=0, F(1)=1.
 *       O(log n) multiplications via the classic fast-doubling identities:
 *         F(2k)   = F(k) * (2*F(k+1) - F(k))
 *         F(2k+1) = F(k)^2 + F(k+1)^2
 *   __gj_fib_u64  (alias)
 *   __libcgj_batch235_marker = "libcgj-batch235"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch235_marker[] = "libcgj-batch235";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b235_fib_pair — recursive fast doubling.
 *
 * On return: *a = F(n), *b = F(n+1), both mod 2^64.
 * All arithmetic is uint64_t; wraparound is the modular ring.
 */
static void
b235_fib_pair(unsigned n, uint64_t *a, uint64_t *b)
{
	uint64_t c, d;
	uint64_t e, f;

	if (n == 0u) {
		*a = 0ull;
		*b = 1ull;
		return;
	}

	b235_fib_pair(n >> 1, &c, &d);
	/* c = F(k), d = F(k+1), k = n/2 */

	/* F(2k)   = F(k) * (2*F(k+1) - F(k))  mod 2^64 */
	e = c * ((d << 1) - c);
	/* F(2k+1) = F(k)^2 + F(k+1)^2         mod 2^64 */
	f = c * c + d * d;

	if ((n & 1u) != 0u) {
		*a = f;         /* F(2k+1) */
		*b = e + f;     /* F(2k+2) = F(2k) + F(2k+1) */
	} else {
		*a = e;         /* F(2k) */
		*b = f;         /* F(2k+1) */
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fib_u64 — F(n) mod 2^64 via fast doubling.
 *
 * Returns the n-th Fibonacci number reduced modulo 2^64.
 * F(0) = 0, F(1) = 1, F(2) = 1, ...
 * For large n the value is the natural wrap of unsigned 64-bit multiply/add.
 */
uint64_t
gj_fib_u64(unsigned n)
{
	uint64_t a, b;

	b235_fib_pair(n, &a, &b);
	(void)b;
	return a;
}

uint64_t __gj_fib_u64(unsigned n)
    __attribute__((alias("gj_fib_u64")));
