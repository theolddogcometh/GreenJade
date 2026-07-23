/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1794: semver triple integer compare.
 *
 * Surface (unique symbols):
 *   int gj_semver_cmp_triple(unsigned a_maj, unsigned a_min, unsigned a_pat,
 *                            unsigned b_maj, unsigned b_min, unsigned b_pat);
 *     — Compare two major.minor.patch triples as unsigned integers.
 *       Returns -1 if a < b, 0 if equal, +1 if a > b. Lexicographic on
 *       (major, minor, patch).
 *   int __gj_semver_cmp_triple  (alias)
 *   __libcgj_batch1794_marker = "libcgj-batch1794"
 *
 * Version/semver exclusive wave (1791–1799). Clean-room freestanding
 * pure C (integer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party source
 * copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1794_marker[] = "libcgj-batch1794";

/* ---- freestanding helpers ---------------------------------------------- */

/* Compare two unsigneds: -1 / 0 / +1. */
static int
b1794_cmp_u(unsigned uA, unsigned uB)
{
	if (uA < uB) {
		return -1;
	}
	if (uA > uB) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_semver_cmp_triple — lexicographic compare of two version triples.
 */
int
gj_semver_cmp_triple(unsigned uAMaj, unsigned uAMin, unsigned uAPat,
                     unsigned uBMaj, unsigned uBMin, unsigned uBPat)
{
	int n;

	n = b1794_cmp_u(uAMaj, uBMaj);
	if (n != 0) {
		return n;
	}
	n = b1794_cmp_u(uAMin, uBMin);
	if (n != 0) {
		return n;
	}
	return b1794_cmp_u(uAPat, uBPat);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_semver_cmp_triple(unsigned uAMaj, unsigned uAMin, unsigned uAPat,
                           unsigned uBMaj, unsigned uBMin, unsigned uBPat)
    __attribute__((alias("gj_semver_cmp_triple")));
