/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch495: freestanding Count-Min Sketch (CMS).
 * Clean-room pure C — integer/pointer only. Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party sketch / hash source was copied.
 *
 * Distinct from Bloom surfaces:
 *   gj_bloom_*     → graph_batch125.c
 *   gj_bloom64_*   → graph_batch208.c
 *
 * Surface (unique symbols):
 *   void     gj_cms_add(uint32_t *table, size_t depth, size_t width,
 *                       const void *key, size_t n);
 *     — Increment one cell per row for key. Row-major layout:
 *         cell(r, c) = table[r * width + c]
 *       Column for row r: (h1 + r * h2) mod width (Kirsch–Mitzenmacher).
 *       Saturating +1 (no wrap past UINT32_MAX). No-op on bad args.
 *   uint32_t gj_cms_est(const uint32_t *table, size_t depth, size_t width,
 *                       const void *key, size_t n);
 *     — Classic CMS frequency estimate: min over the depth probe cells.
 *       Returns 0 on bad args or empty sketch dimensions.
 *   __gj_cms_add / __gj_cms_est  (aliases)
 *   __libcgj_batch495_marker = "libcgj-batch495"
 *
 * Hashing: two independent inline FNV-1a 64 digests (static to this TU —
 * not the batch41 exports). h2 forced odd so the linear sequence has full
 * period when width is a power of two.
 *
 * Caller owns storage and zeroes table before use (depth * width cells).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch495_marker[] = "libcgj-batch495";

/* FNV-1a 64 offset bases (distinct pair for double-hashing). */
#define B495_FNV_OFF1  0xcbf29ce484222325ULL /* standard FNV-1a 64 offset */
#define B495_FNV_OFF2  0x84222325cbf29ce4ULL /* distinct second basis */
#define B495_FNV_PRIME 0x100000001b3ULL

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Inline FNV-1a 64 with caller-chosen offset basis. Static — does not
 * collide with batch41 fnv1a_64.
 */
static uint64_t
b495_fnv1a64(uint64_t uOff, const void *pKey, size_t cb)
{
	const unsigned char *p;
	uint64_t h;
	size_t i;

	h = uOff;
	if (pKey == NULL || cb == 0u) {
		return h;
	}
	p = (const unsigned char *)pKey;
	for (i = 0u; i < cb; i++) {
		h ^= (uint64_t)p[i];
		h *= B495_FNV_PRIME;
	}
	return h;
}

/* Two independent 64-bit hashes for Kirsch–Mitzenmacher row probes. */
static void
b495_hashes(const void *pKey, size_t cb, uint64_t *pH1, uint64_t *pH2)
{
	uint64_t h1;
	uint64_t h2;

	/*
	 * Second digest: FNV-1a over the same key but with a distinct
	 * offset, then mixed with the first so empty keys still differ.
	 */
	h1 = b495_fnv1a64(B495_FNV_OFF1, pKey, cb);
	h2 = b495_fnv1a64(B495_FNV_OFF2 ^ 0x9e3779b97f4a7c15ULL, pKey, cb);
	h2 ^= h1 >> 33;
	h2 *= 0xff51afd7ed558ccdULL;
	/* Force odd: full period when width is a power of two. */
	h2 |= 1ULL;

	*pH1 = h1;
	*pH2 = h2;
}

/*
 * Column index for row iRow: (h1 + iRow * h2) mod width.
 * width must be non-zero (caller-checked).
 */
static size_t
b495_col(uint64_t h1, uint64_t h2, size_t iRow, size_t nWidth)
{
	uint64_t uIdx;

	uIdx = h1 + (uint64_t)iRow * h2;
	return (size_t)(uIdx % (uint64_t)nWidth);
}

/* ---- public API -------------------------------------------------------- */

/*
 * gj_cms_add — insert one occurrence of key into the Count-Min Sketch.
 *
 * For each row r in [0, depth): saturating-increment cell
 *   table[r * width + (h1 + r * h2) % width]
 * No-op if table is NULL, depth/width is 0, or key is NULL with n != 0.
 */
void
gj_cms_add(uint32_t *table, size_t depth, size_t width, const void *key,
           size_t n)
{
	uint64_t h1;
	uint64_t h2;
	size_t iRow;

	if (table == NULL || depth == 0u || width == 0u) {
		return;
	}
	if (key == NULL && n != 0u) {
		return;
	}

	b495_hashes(key, n, &h1, &h2);

	for (iRow = 0u; iRow < depth; iRow++) {
		size_t iCol;
		size_t iCell;
		uint32_t uCur;

		iCol = b495_col(h1, h2, iRow, width);
		iCell = iRow * width + iCol;
		uCur = table[iCell];
		if (uCur < 0xffffffffu) {
			table[iCell] = uCur + 1u;
		}
	}
}

/*
 * gj_cms_est — estimate frequency of key (min over depth probe cells).
 *
 * Returns 0 when table is NULL, depth/width is 0, or key is NULL with n != 0.
 * Otherwise returns the minimum of the depth counters for key (classic CMS).
 */
uint32_t
gj_cms_est(const uint32_t *table, size_t depth, size_t width, const void *key,
           size_t n)
{
	uint64_t h1;
	uint64_t h2;
	size_t iRow;
	uint32_t uMin;

	if (table == NULL || depth == 0u || width == 0u) {
		return 0u;
	}
	if (key == NULL && n != 0u) {
		return 0u;
	}

	b495_hashes(key, n, &h1, &h2);

	uMin = 0xffffffffu;
	for (iRow = 0u; iRow < depth; iRow++) {
		size_t iCol;
		size_t iCell;
		uint32_t uCur;

		iCol = b495_col(h1, h2, iRow, width);
		iCell = iRow * width + iCol;
		uCur = table[iCell];
		if (uCur < uMin) {
			uMin = uCur;
		}
	}
	return uMin;
}

void __gj_cms_add(uint32_t *table, size_t depth, size_t width, const void *key,
                  size_t n)
    __attribute__((alias("gj_cms_add")));
uint32_t __gj_cms_est(const uint32_t *table, size_t depth, size_t width,
                      const void *key, size_t n)
    __attribute__((alias("gj_cms_est")));
