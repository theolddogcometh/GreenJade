/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1564: Jaccard similarity of character sets.
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_jaccard_ngram_hint(const char *a, const char *b);
 *     — Milli-Jaccard [0, 1000] of the unigram (char) sets of a and b.
 *       Presence is tracked as a 256-bit bitset (4×uint64). Empty-vs-empty
 *       returns 1000. NULL a or NULL b treated as empty set.
 *       Distinct from gj_jaccard_u64 (batch538).
 *   uint32_t __gj_str_jaccard_ngram_hint  (alias)
 *   __libcgj_batch1564_marker = "libcgj-batch1564"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1564_marker[] = "libcgj-batch1564";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * SWAR popcount64 — no __builtin_popcount (libgcc under -ffreestanding).
 */
static unsigned
b1564_popcount64(uint64_t u64X)
{
	u64X = u64X - ((u64X >> 1) & 0x5555555555555555ull);
	u64X = (u64X & 0x3333333333333333ull) +
	       ((u64X >> 2) & 0x3333333333333333ull);
	u64X = (u64X + (u64X >> 4)) & 0x0f0f0f0f0f0f0f0full;
	u64X = u64X + (u64X >> 8);
	u64X = u64X + (u64X >> 16);
	u64X = u64X + (u64X >> 32);
	return (unsigned)(u64X & 0x7full);
}

/* Fill 256-bit presence set from NUL-terminated string (or empty if NULL). */
static void
b1564_charset(const char *sz, uint64_t aBits[4])
{
	size_t i;

	aBits[0] = 0ull;
	aBits[1] = 0ull;
	aBits[2] = 0ull;
	aBits[3] = 0ull;

	if (sz == NULL) {
		return;
	}
	for (i = 0u; sz[i] != '\0'; i++) {
		unsigned char c = (unsigned char)sz[i];
		aBits[c >> 6] |= (uint64_t)1ull << (c & 63u);
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_jaccard_ngram_hint — milli-Jaccard of char presence sets.
 *
 * |A ∩ B| / |A ∪ B| × 1000, integer floor. Empty∩empty → 1000.
 */
uint32_t
gj_str_jaccard_ngram_hint(const char *a, const char *b)
{
	uint64_t aA[4];
	uint64_t aB[4];
	unsigned cInter = 0u;
	unsigned cUnion = 0u;
	int w;

	b1564_charset(a, aA);
	b1564_charset(b, aB);

	for (w = 0; w < 4; w++) {
		cInter += b1564_popcount64(aA[w] & aB[w]);
		cUnion += b1564_popcount64(aA[w] | aB[w]);
	}

	if (cUnion == 0u) {
		return 1000u;
	}
	/* cInter * 1000 fits in 32 bits (max 256000). */
	return (uint32_t)((cInter * 1000u) / cUnion);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_jaccard_ngram_hint(const char *a, const char *b)
    __attribute__((alias("gj_str_jaccard_ngram_hint")));
