/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch240: Rabin–Karp first-match string search.
 *
 * Surface (unique symbols):
 *   ssize_t gj_rk_find(const char *hay, const char *needle);
 *     — First byte index of needle in hay, or -1 if not found / NULL args.
 *       Empty needle → 0 (like strstr). Hash hits are always verified
 *       byte-for-byte (no false-positive returns).
 *   __gj_rk_find  (alias)
 *   __libcgj_batch240_marker = "libcgj-batch240"
 *
 * Classic Rabin–Karp: polynomial rolling hash over unsigned bytes with
 * base B240_BASE and natural uint64_t wrap (mod 2^64). No __uint128_t.
 * On every hash equality, memcmp-style verification confirms a true match.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno, no libc string. Distinct
 * from KMP (batch216) and Boyer–Moore–Horspool (batch217).
 */

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

/* Polynomial base; not a power of two so longer windows still mix. */
#define B240_BASE 257ull

const char __libcgj_batch240_marker[] = "libcgj-batch240";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Measure NUL-terminated length without libc.
 */
static size_t
b240_strlen(const char *sz)
{
	size_t n = 0u;

	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/*
 * Return 1 if a[0..n) equals b[0..n) as unsigned bytes; else 0.
 */
static int
b240_eq(const unsigned char *a, const unsigned char *b, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		if (a[i] != b[i]) {
			return 0;
		}
	}
	return 1;
}

/*
 * Polynomial hash of s[0..n) with base B240_BASE, mod 2^64.
 */
static uint64_t
b240_hash(const unsigned char *s, size_t n)
{
	uint64_t h = 0ull;
	size_t i;

	for (i = 0u; i < n; i++) {
		h = h * B240_BASE + (uint64_t)s[i];
	}
	return h;
}

/*
 * power = B240_BASE^(n-1) mod 2^64 for n >= 1 (used to drop the leading
 * byte of a rolling window). For n == 1 the leading factor is base^0 = 1.
 */
static uint64_t
b240_pow_nm1(size_t n)
{
	uint64_t p = 1ull;
	size_t i;

	for (i = 1u; i < n; i++) {
		p *= B240_BASE;
	}
	return p;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rk_find — first index of needle in hay via Rabin–Karp, or -1.
 *
 * hay / needle: NUL-terminated C strings (bytes; no locale / case folding).
 * Empty needle is found at index 0 (including when hay is empty).
 * NULL either argument → -1.
 * Hay may be arbitrarily long (scanned until NUL or match).
 *
 * Every hash collision is verified with a full byte compare so a match
 * return is never a false positive.
 */
ssize_t
gj_rk_find(const char *hay, const char *needle)
{
	size_t nLen;
	size_t pos;
	uint64_t hNeedle;
	uint64_t hWindow;
	uint64_t power;
	const unsigned char *h;
	const unsigned char *n;
	unsigned char nextc;
	unsigned char drop;

	if (hay == NULL || needle == NULL) {
		return (ssize_t)-1;
	}

	/* Empty needle: conventional strstr empty match at index 0. */
	if (needle[0] == '\0') {
		return (ssize_t)0;
	}

	n = (const unsigned char *)needle;
	h = (const unsigned char *)hay;
	nLen = b240_strlen(needle);

	hNeedle = b240_hash(n, nLen);
	power = b240_pow_nm1(nLen);

	/* Ensure hay has at least nLen bytes before hashing the first window. */
	{
		size_t i;

		for (i = 0u; i < nLen; i++) {
			if (h[i] == '\0') {
				return (ssize_t)-1;
			}
		}
	}
	hWindow = b240_hash(h, nLen);

	pos = 0u;
	for (;;) {
		if (hWindow == hNeedle && b240_eq(h + pos, n, nLen) != 0) {
			return (ssize_t)pos;
		}

		nextc = h[pos + nLen];
		if (nextc == '\0') {
			break;
		}

		/* Roll: drop leading byte, append next. uint64 wrap is the ring. */
		drop = h[pos];
		hWindow = hWindow - (uint64_t)drop * power;
		hWindow = hWindow * B240_BASE + (uint64_t)nextc;
		pos++;
	}

	return (ssize_t)-1;
}

ssize_t __gj_rk_find(const char *hay, const char *needle)
    __attribute__((alias("gj_rk_find")));
