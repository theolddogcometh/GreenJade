/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3771: KMP prefix / LPS table build (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_kmp_prefix_u(const uint8_t *pat, size_t n, size_t *pi);
 *     - Fill pi[0..n) with the KMP longest-prefix-suffix (π) table for
 *       pat[0..n). pi[i] is the longest proper prefix of pat[0..i] that
 *       is also a suffix of pat[0..i]. Returns n on success.
 *       n==0 → 0 (no writes). NULL pat/pi with n>0, or n > 4096 →
 *       (size_t)-1.
 *   size_t __gj_kmp_prefix_u  (alias)
 *   __libcgj_batch3771_marker = "libcgj-batch3771"
 *
 * Exclusive continuum CREATE-ONLY (3771-3780: kmp_prefix_u / kmp_step_u /
 * boyer_skip_u / rabin_hash_u / rabin_roll_u / lcs_cell_u / edit_cell_u /
 * lcs_len_step_u / z_box_u / batch_id_3780). Distinct from
 * gj_kmp_build_lps (batch1221, int pi) and gj_kmp_find (batch216) —
 * unique size_t _u surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

enum { B3771_MAX_N = 4096 };

const char __libcgj_batch3771_marker[] = "libcgj-batch3771";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Classic KMP π / LPS fill. Caller guarantees pat/pi non-NULL and n>0.
 */
static void
b3771_prefix(const uint8_t *pPat, size_t uN, size_t *pPi)
{
	size_t uI;
	size_t uLen;

	pPi[0] = 0u;
	uLen = 0u;
	uI = 1u;
	while (uI < uN) {
		if (pPat[uI] == pPat[uLen]) {
			uLen++;
			pPi[uI] = uLen;
			uI++;
		} else if (uLen != 0u) {
			uLen = pPi[uLen - 1u];
		} else {
			pPi[uI] = 0u;
			uI++;
		}
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kmp_prefix_u — fill size_t KMP prefix/LPS table for pat[0..n).
 *
 * pat: pattern bytes (not necessarily NUL-terminated)
 * n:   pattern length (0..4096)
 * pi:  caller-owned output of at least n size_t slots
 *
 * Returns n on success, (size_t)-1 on bad args / oversize.
 * No parent wires.
 */
size_t
gj_kmp_prefix_u(const uint8_t *pat, size_t n, size_t *pi)
{
	(void)NULL;
	if (n > (size_t)B3771_MAX_N) {
		return (size_t)-1;
	}
	if (n == 0u) {
		return 0u;
	}
	if (pat == NULL || pi == NULL) {
		return (size_t)-1;
	}
	b3771_prefix(pat, n, pi);
	return n;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_kmp_prefix_u(const uint8_t *pat, size_t n, size_t *pi)
    __attribute__((alias("gj_kmp_prefix_u")));
