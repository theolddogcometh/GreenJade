/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch279: human size strings → u64 bytes (1024-based).
 *
 * Surface (unique symbols):
 *   int gj_parse_size_u64(const char *s, uint64_t *out);
 *     — Parse decimal size with optional 1024-based suffix:
 *         ""  bare digits  → bytes
 *         K/k → * 1024
 *         M/m → * 1024^2
 *         G/g → * 1024^3
 *       On success write byte count to *out and return 0.
 *       On failure return -1 (no write). No errno (freestanding).
 *   __gj_parse_size_u64  (alias)
 *   __libcgj_batch279_marker = "libcgj-batch279"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   dehumanize_number / expand_number / humanize_number → graph_batch30/36
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch279_marker[] = "libcgj-batch279";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b279_mul_u64 — *pu *= uMul with overflow detect.
 * Returns 0 on success, -1 if the product would exceed UINT64_MAX.
 */
static int
b279_mul_u64(uint64_t *pu, uint64_t uMul)
{
	uint64_t u;

	if (pu == NULL) {
		return -1;
	}
	u = *pu;
	if (uMul == 0ull) {
		*pu = 0ull;
		return 0;
	}
	if (u > (UINT64_MAX / uMul)) {
		return -1;
	}
	*pu = u * uMul;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_size_u64 — human size text → byte count (1024-based suffixes).
 *
 * Grammar (full-string match, no leading/trailing junk):
 *   [0-9]+ [KkMmGg]?
 *
 * Multipliers (binary / IEC power-of-two, not SI 1000):
 *   (none) → 1
 *   K/k    → 1024
 *   M/m    → 1024*1024
 *   G/g    → 1024*1024*1024
 *
 * Edge cases:
 *   s == NULL || out == NULL → -1
 *   empty / no digits / non-digit junk / overflow → -1
 *   "0", "0K", "0M", "0G" → 0
 *
 * Examples:
 *   "1K" → 1024
 *   "2M" → 2097152
 *   "3G" → 3221225472
 *   "42" → 42
 */
int
gj_parse_size_u64(const char *s, uint64_t *out)
{
	const char *p;
	uint64_t u;
	int nDig;
	char ch;
	uint64_t uMul;

	if (s == NULL || out == NULL) {
		return -1;
	}

	p = s;
	if (*p < '0' || *p > '9') {
		return -1;
	}

	u = 0ull;
	nDig = 0;
	while (*p >= '0' && *p <= '9') {
		uint64_t uDig = (uint64_t)(*p - '0');

		if (u > (UINT64_MAX - uDig) / 10ull) {
			return -1;
		}
		u = u * 10ull + uDig;
		p++;
		nDig++;
	}
	if (nDig == 0) {
		return -1;
	}

	ch = *p;
	uMul = 1ull;
	if (ch == 'K' || ch == 'k') {
		uMul = 1024ull;
		p++;
	} else if (ch == 'M' || ch == 'm') {
		uMul = 1024ull * 1024ull;
		p++;
	} else if (ch == 'G' || ch == 'g') {
		uMul = 1024ull * 1024ull * 1024ull;
		p++;
	} else if (ch != '\0') {
		return -1;
	}

	if (b279_mul_u64(&u, uMul) != 0) {
		return -1;
	}
	if (*p != '\0') {
		return -1;
	}

	*out = u;
	return 0;
}

int __gj_parse_size_u64(const char *s, uint64_t *out)
    __attribute__((alias("gj_parse_size_u64")));
