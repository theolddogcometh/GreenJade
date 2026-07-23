/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1798: ABI major-only compatibility predicate.
 *
 * Surface (unique symbols):
 *   int gj_abi_compat_major_p(unsigned a_maj, unsigned a_min, unsigned a_pat,
 *                             unsigned b_maj, unsigned b_min, unsigned b_pat);
 *     — Return 1 if a and b share the same major version (ABI major
 *       compatibility hint); else 0. Minor/patch arguments are accepted
 *       for a uniform triple API and are intentionally ignored.
 *   int __gj_abi_compat_major_p  (alias)
 *   __libcgj_batch1798_marker = "libcgj-batch1798"
 *
 * Version/semver exclusive wave (1791–1799). Clean-room freestanding
 * pure C (integer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party source
 * copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1798_marker[] = "libcgj-batch1798";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff majors equal. Minor/patch unused (API symmetry with full_p). */
static int
b1798_same_major(unsigned uAMaj, unsigned uBMaj)
{
	return (uAMaj == uBMaj) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_abi_compat_major_p — 1 if major components match.
 */
int
gj_abi_compat_major_p(unsigned uAMaj, unsigned uAMin, unsigned uAPat,
                      unsigned uBMaj, unsigned uBMin, unsigned uBPat)
{
	/* Silence unused-parameter under -Werror without changing semantics. */
	(void)uAMin;
	(void)uAPat;
	(void)uBMin;
	(void)uBPat;
	return b1798_same_major(uAMaj, uBMaj);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_abi_compat_major_p(unsigned uAMaj, unsigned uAMin, unsigned uAPat,
                            unsigned uBMaj, unsigned uBMin, unsigned uBPat)
    __attribute__((alias("gj_abi_compat_major_p")));
