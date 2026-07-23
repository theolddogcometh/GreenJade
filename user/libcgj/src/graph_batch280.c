/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch280: format uint64_t byte count as a
 * human-readable 1024-based size string (integer floor only).
 *
 * Surface (unique symbols):
 *   int gj_format_size_u64(uint64_t n, char *out, size_t cap);
 *     — Write a NUL-terminated IEC-style power-of-1024 size into out.
 *       Units (floor division): B K M G T P E
 *         0          → "0B"
 *         512        → "512B"
 *         1024       → "1K"
 *         1536       → "1K"   (floor; no fractional digit)
 *         1048576    → "1M"
 *         UINT64_MAX → "15E"
 *       Returns the number of characters written excluding NUL on
 *       success, or -1 on error (out == NULL, cap == 0, or result
 *       including NUL does not fit in cap). On error out is undefined.
 *   __gj_format_size_u64  (alias)
 *   __libcgj_batch280_marker = "libcgj-batch280"
 *
 * Distinct from humanize_number / expand_number (graph_batch30.c): this
 * is a gj-prefixed freestanding u64-only helper with no errno/suffix
 * argument. Clean-room pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno,
 * no float, no libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch280_marker[] = "libcgj-batch280";

/* Unit suffixes: index 0 = bytes, then KiB..EiB (1024-based floor). */
static const char g_b280_suf[] = "BKMGTPE";

/* Max scale index (E); after 6× floor(/1024), u64 always fits in that unit. */
#define B280_MAX_SCALE 6

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b280_u64_digits — write decimal digits of uVal into aTmp reversed.
 * Returns digit count (at least 1 for zero). aTmp must hold >= 20 bytes.
 */
static int
b280_u64_digits(uint64_t uVal, char *aTmp, int nTmpCap)
{
	int nDig = 0;

	if (uVal == 0ull) {
		if (nTmpCap < 1) {
			return 0;
		}
		aTmp[0] = '0';
		return 1;
	}
	while (uVal > 0ull && nDig < nTmpCap) {
		aTmp[nDig++] = (char)('0' + (int)(uVal % 10ull));
		uVal /= 10ull;
	}
	return nDig;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_format_size_u64 — format n bytes as floor(1024^k) human size.
 *
 * Scales n down by integer division by 1024 until n < 1024 or the E
 * unit is reached. Emits "<digits><suffix>" with no space and no
 * fractional part (freestanding, no float).
 *
 * Edge cases:
 *   out == NULL or cap == 0 → -1
 *   cap too small for digits + suffix + NUL → -1
 * Success: length excluding NUL (same spirit as humanize_number).
 */
int
gj_format_size_u64(uint64_t n, char *out, size_t cap)
{
	uint64_t uVal;
	int nIdx;
	char aDig[20];
	int nDig;
	int iDig;
	size_t iOut;
	size_t cbNeed;

	if (out == NULL || cap == 0u) {
		return -1;
	}

	uVal = n;
	nIdx = 0;
	while (uVal >= 1024ull && nIdx < B280_MAX_SCALE) {
		uVal /= 1024ull;
		nIdx++;
	}

	nDig = b280_u64_digits(uVal, aDig, (int)sizeof(aDig));
	if (nDig <= 0) {
		return -1;
	}

	/* digits + 1 unit char + NUL */
	cbNeed = (size_t)nDig + 2u;
	if (cap < cbNeed) {
		return -1;
	}

	iOut = 0u;
	for (iDig = nDig; iDig > 0; iDig--) {
		out[iOut++] = aDig[iDig - 1];
	}
	out[iOut++] = g_b280_suf[nIdx];
	out[iOut] = '\0';
	return (int)iOut;
}

int __gj_format_size_u64(uint64_t n, char *out, size_t cap)
    __attribute__((alias("gj_format_size_u64")));
