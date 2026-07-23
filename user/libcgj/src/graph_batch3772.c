/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3772: single KMP automaton step (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_kmp_step_u(size_t j, uint8_t ch, const uint8_t *pat,
 *                        size_t n, const size_t *pi);
 *     - One KMP transition: current matched length j, next text byte ch,
 *       pattern pat[0..n) and its prefix table pi (from gj_kmp_prefix_u).
 *       Returns new matched length in 0..n. When result equals n, a full
 *       match ends at the current text position. NULL pat/pi with n>0
 *       → 0. j is clamped to n (j>n treated as n). n==0 → 0.
 *   size_t __gj_kmp_step_u  (alias)
 *   __libcgj_batch3772_marker = "libcgj-batch3772"
 *
 * Exclusive continuum CREATE-ONLY (3771-3780). Distinct from
 * gj_kmp_search_first (batch1222) and gj_kmp_find (batch216) — single
 * state step only; unique _u surface. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3772_marker[] = "libcgj-batch3772";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * KMP automaton step. Caller guarantees pat/pi non-NULL when n>0 and
 * j <= n.
 */
static size_t
b3772_step(size_t uJ, uint8_t u8Ch, const uint8_t *pPat, size_t uN,
           const size_t *pPi)
{
	(void)uN;
	while (uJ > 0u && u8Ch != pPat[uJ]) {
		uJ = pPi[uJ - 1u];
	}
	if (u8Ch == pPat[uJ]) {
		uJ++;
	}
	return uJ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kmp_step_u — one KMP matched-length transition on byte ch.
 *
 * j:   current matched prefix length (0..n)
 * ch:  next text byte
 * pat: pattern of length n
 * n:   pattern length
 * pi:  prefix table of length n (pi[i] < i+1)
 *
 * Returns new matched length in 0..n. Full match when return == n.
 * No parent wires.
 */
size_t
gj_kmp_step_u(size_t j, uint8_t ch, const uint8_t *pat, size_t n,
              const size_t *pi)
{
	(void)NULL;
	if (n == 0u) {
		return 0u;
	}
	if (pat == NULL || pi == NULL) {
		return 0u;
	}
	if (j > n) {
		j = n;
	}
	/* Already at full match: fall back via pi[n-1] before consuming. */
	if (j == n) {
		j = pi[n - 1u];
	}
	return b3772_step(j, ch, pat, n, pi);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_kmp_step_u(size_t j, uint8_t ch, const uint8_t *pat, size_t n,
                       const size_t *pi)
    __attribute__((alias("gj_kmp_step_u")));
