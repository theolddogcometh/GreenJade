/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3787: dual polynomial string hash pair (_u).
 *
 * Surface (unique symbols):
 *   int gj_string_hash_pair_u(const unsigned char *s, size_t n,
 *                             uint64_t *h0, uint64_t *h1);
 *     - Write two independent polynomial hashes of s[0..n) into *h0 and
 *       *h1 using fixed (base, mod) pairs:
 *         h0: base 911382323,  mod 2^64 (native wrap)
 *         h1: base 972663749,  mod 1000000007
 *       Empty (n == 0) writes 0/0 and returns 0. NULL s with n > 0, or
 *       NULL h0/h1 → -1 (no writes on NULL outs). Success → 0.
 *   int __gj_string_hash_pair_u  (alias)
 *   __libcgj_batch3787_marker = "libcgj-batch3787"
 *
 * Exclusive continuum CREATE-ONLY (3781-3790). Distinct from
 * gj_rolling_hash_u / gj_poly_hash / gj_rk_hash — dual fixed-base pair
 * out-params; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3787_marker[] = "libcgj-batch3787";

enum {
	B3787_BASE0 = 911382323u,
	B3787_BASE1 = 972663749u,
	B3787_MOD1 = 1000000007u
};

/* ---- modular helpers (no __int128) ------------------------------------- */

static uint64_t
b3787_mulmod(uint64_t a, uint64_t b, uint64_t m)
{
	uint64_t r;

	if (m == 0u) {
		return a * b;
	}
	a %= m;
	b %= m;
	r = 0u;
	while (b != 0u) {
		if ((b & 1u) != 0u) {
			uint64_t s = m - r;

			r = (a >= s) ? (a - s) : (r + a);
		}
		{
			uint64_t s = m - a;

			a = (a >= s) ? (a - s) : (a + a);
		}
		b >>= 1;
	}
	return r;
}

static uint64_t
b3787_addmod(uint64_t a, uint64_t b, uint64_t m)
{
	uint64_t s;

	if (m == 0u) {
		return a + b;
	}
	a %= m;
	b %= m;
	s = m - a;
	if (b >= s) {
		return b - s;
	}
	return a + b;
}

static uint64_t
b3787_poly(const unsigned char *s, size_t n, uint64_t base, uint64_t mod)
{
	uint64_t h;
	size_t i;

	h = 0u;
	for (i = 0u; i < n; i++) {
		h = b3787_mulmod(h, base, mod);
		h = b3787_addmod(h, (uint64_t)s[i], mod);
	}
	return h;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_string_hash_pair_u — dual fixed-base polynomial hashes of s.
 *
 * Does not allocate. No parent wires.
 */
int
gj_string_hash_pair_u(const unsigned char *s, size_t n, uint64_t *h0,
                      uint64_t *h1)
{
	(void)NULL;
	if (h0 == NULL || h1 == NULL) {
		return -1;
	}
	if (n == 0u) {
		*h0 = 0u;
		*h1 = 0u;
		return 0;
	}
	if (s == NULL) {
		return -1;
	}

	*h0 = b3787_poly(s, n, (uint64_t)B3787_BASE0, 0u);
	*h1 = b3787_poly(s, n, (uint64_t)B3787_BASE1, (uint64_t)B3787_MOD1);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_string_hash_pair_u(const unsigned char *s, size_t n, uint64_t *h0,
                            uint64_t *h1)
    __attribute__((alias("gj_string_hash_pair_u")));
