/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1799: ABI full-triple equality predicate.
 *
 * Surface (unique symbols):
 *   int gj_abi_compat_full_p(unsigned a_maj, unsigned a_min, unsigned a_pat,
 *                            unsigned b_maj, unsigned b_min, unsigned b_pat);
 *     — Return 1 if a and b have equal major, minor, and patch; else 0.
 *       Strict triple identity for full ABI compatibility checks.
 *   int __gj_abi_compat_full_p  (alias)
 *   __libcgj_batch1799_marker = "libcgj-batch1799"
 *
 * Version/semver exclusive wave (1791–1799). Clean-room freestanding
 * pure C (integer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party source
 * copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1799_marker[] = "libcgj-batch1799";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff all three components match. */
static int
b1799_triple_eq(unsigned uAMaj, unsigned uAMin, unsigned uAPat,
                unsigned uBMaj, unsigned uBMin, unsigned uBPat)
{
	if (uAMaj != uBMaj) {
		return 0;
	}
	if (uAMin != uBMin) {
		return 0;
	}
	if (uAPat != uBPat) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_abi_compat_full_p — 1 if major.minor.patch triples are equal.
 */
int
gj_abi_compat_full_p(unsigned uAMaj, unsigned uAMin, unsigned uAPat,
                     unsigned uBMaj, unsigned uBMin, unsigned uBPat)
{
	return b1799_triple_eq(uAMaj, uAMin, uAPat, uBMaj, uBMin, uBPat);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_abi_compat_full_p(unsigned uAMaj, unsigned uAMin, unsigned uAPat,
                           unsigned uBMaj, unsigned uBMin, unsigned uBPat)
    __attribute__((alias("gj_abi_compat_full_p")));
