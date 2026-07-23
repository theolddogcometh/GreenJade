/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3786: polynomial rolling hash one-shot (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_rolling_hash_u(const unsigned char *s, size_t n,
 *                              uint64_t base, uint64_t mod);
 *     - Horner polynomial hash of s[0..n):
 *         h = 0; for each byte c: h = (h * base + c) mod mod
 *       NULL s or n == 0 → 0. mod == 0 means natural uint64 wrap
 *       (mod 2^64). mod == 1 → 0. No __int128; binary mulmod.
 *   uint64_t __gj_rolling_hash_u  (alias)
 *   __libcgj_batch3786_marker = "libcgj-batch3786"
 *
 * Exclusive continuum CREATE-ONLY (3781-3790). Distinct from gj_rk_hash
 * (batch180) and gj_poly_hash (batch239) — unique _u surface; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3786_marker[] = "libcgj-batch3786";

/* ---- modular helpers (no __int128 / no libgcc __umodti3) --------------- */

static uint64_t
b3786_mulmod(uint64_t a, uint64_t b, uint64_t m)
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
b3786_addmod(uint64_t a, uint64_t b, uint64_t m)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rolling_hash_u — one-shot polynomial rolling hash over n bytes.
 *
 * Does not allocate. No parent wires.
 */
uint64_t
gj_rolling_hash_u(const unsigned char *s, size_t n, uint64_t base,
                  uint64_t mod)
{
	uint64_t h;
	size_t i;

	(void)NULL;
	if (s == NULL || n == 0u) {
		return 0u;
	}

	h = 0u;
	for (i = 0u; i < n; i++) {
		h = b3786_mulmod(h, base, mod);
		h = b3786_addmod(h, (uint64_t)s[i], mod);
	}
	return h;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_rolling_hash_u(const unsigned char *s, size_t n,
                             uint64_t base, uint64_t mod)
    __attribute__((alias("gj_rolling_hash_u")));
