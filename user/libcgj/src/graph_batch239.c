/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch239: polynomial rolling hash over a byte buffer.
 *
 * Surface (unique symbols):
 *   uint64_t gj_poly_hash(const void *data, size_t n,
 *                         uint64_t base, uint64_t mod);
 *     — one-shot polynomial hash of data[0..n):
 *         h = (...((b[0]*base + b[1])*base + ... ) + b[n-1]) mod mod
 *       Empty (n==0) → 0. data==NULL with n>0 → 0.
 *       mod==0 means natural uint64 wrap (mod 2^64).
 *   __gj_poly_hash  (alias)
 *   __libcgj_batch239_marker = "libcgj-batch239"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No __uint128_t / no libgcc __umodti3. No malloc,
 * no errno. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch239_marker[] = "libcgj-batch239";

/* ---- modular helpers (no __uint128_t / no libgcc __umodti3) ------------ */

/*
 * (a * b) mod m. When m==0, returns a*b with uint64 wrap.
 * Binary modular multiply: freestanding-safe (no 128-bit rem).
 */
static uint64_t
b239_mulmod(uint64_t a, uint64_t b, uint64_t m)
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
b239_addmod(uint64_t a, uint64_t b, uint64_t m)
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
 * gj_poly_hash — polynomial rolling hash of n bytes at data.
 *
 * Horner scheme: h = 0; for each byte c: h = (h * base + c) mod mod.
 * When mod==0, multiplies and adds wrap mod 2^64 (native uint64).
 *
 * Special cases:
 *   data == NULL && n > 0 → 0
 *   n == 0                → 0
 *   mod == 1              → 0 (everything ≡ 0 mod 1)
 */
uint64_t
gj_poly_hash(const void *data, size_t n, uint64_t base, uint64_t mod)
{
	const unsigned char *p;
	uint64_t h;
	size_t i;

	if (data == NULL || n == 0u) {
		return 0u;
	}

	p = (const unsigned char *)data;
	h = 0u;
	for (i = 0u; i < n; i++) {
		h = b239_mulmod(h, base, mod);
		h = b239_addmod(h, (uint64_t)p[i], mod);
	}
	return h;
}

uint64_t __gj_poly_hash(const void *data, size_t n, uint64_t base, uint64_t mod)
    __attribute__((alias("gj_poly_hash")));
