/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch180: Rabin-Karp rolling hash.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party hash source was copied.
 *
 * Surface (unique symbols):
 *   uint64_t gj_rk_hash(const unsigned char *p, size_t n,
 *                       uint64_t base, uint64_t mod);
 *     — polynomial hash of p[0..n):
 *         h = (...((p[0]*base + p[1])*base + ... ) + p[n-1]) mod mod
 *       Empty (n==0) or NULL p with n==0 → 0. NULL p with n>0 → 0.
 *       mod==0 means natural uint64 wrap (mod 2^64).
 *   uint64_t gj_rk_roll(uint64_t h, unsigned char outc, unsigned char inc,
 *                       uint64_t base_pow, uint64_t base, uint64_t mod);
 *     — slide a fixed-width window: drop leading outc, append inc.
 *         h' = ((h - outc * base_pow) * base + inc) mod mod
 *       Caller supplies base_pow = base^(window_len-1) mod mod.
 *       mod==0 means natural uint64 wrap.
 *   __gj_rk_hash / __gj_rk_roll  (aliases)
 *   __libcgj_batch180_marker = "libcgj-batch180"
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch180_marker[] = "libcgj-batch180";

/* ---- modular helpers (no __uint128_t / no libgcc __umodti3) ------------ */

/*
 * (a * b) mod m. When m==0, returns a*b with uint64 wrap.
 * Binary modular multiply: freestanding-safe (no 128-bit rem).
 */
static uint64_t
b180_mulmod(uint64_t a, uint64_t b, uint64_t m)
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

/*
 * (a + b) mod m. When m==0, returns a+b with uint64 wrap.
 */
static uint64_t
b180_addmod(uint64_t a, uint64_t b, uint64_t m)
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

/*
 * (a - b) mod m, with a,b already reduced or free. When m==0, uint64 wrap.
 */
static uint64_t
b180_submod(uint64_t a, uint64_t b, uint64_t m)
{
	if (m == 0u) {
		return a - b;
	}
	a %= m;
	b %= m;
	if (a >= b) {
		return a - b;
	}
	return m - (b - a);
}

/* ---- gj_rk_hash -------------------------------------------------------- */

/*
 * One-shot Rabin-Karp polynomial hash over n bytes.
 * h = 0; for each byte c: h = (h * base + c) mod mod.
 */
uint64_t
gj_rk_hash(const unsigned char *p, size_t n, uint64_t base, uint64_t mod)
{
	uint64_t h = 0u;
	size_t i;

	if (p == NULL || n == 0u) {
		return 0u;
	}

	for (i = 0u; i < n; i++) {
		h = b180_mulmod(h, base, mod);
		h = b180_addmod(h, (uint64_t)p[i], mod);
	}
	return h;
}

uint64_t __gj_rk_hash(const unsigned char *p, size_t n, uint64_t base,
                      uint64_t mod)
    __attribute__((alias("gj_rk_hash")));

/* ---- gj_rk_roll -------------------------------------------------------- */

/*
 * Rolling update for a window of length L:
 *   drop outc (weight base_pow = base^(L-1)), append inc.
 * Numerically: h = ((h - outc*base_pow)*base + inc) mod mod.
 */
uint64_t
gj_rk_roll(uint64_t h, unsigned char outc, unsigned char inc,
           uint64_t base_pow, uint64_t base, uint64_t mod)
{
	uint64_t term;

	term = b180_mulmod((uint64_t)outc, base_pow, mod);
	h = b180_submod(h, term, mod);
	h = b180_mulmod(h, base, mod);
	h = b180_addmod(h, (uint64_t)inc, mod);
	return h;
}

uint64_t __gj_rk_roll(uint64_t h, unsigned char outc, unsigned char inc,
                      uint64_t base_pow, uint64_t base, uint64_t mod)
    __attribute__((alias("gj_rk_roll")));
