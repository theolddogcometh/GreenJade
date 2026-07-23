/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4535: size_t fits in uint32_t predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_size_fits_u32_u(size_t n);
 *     - Return 1 if n <= UINT32_MAX, else 0.
 *   int __gj_size_fits_u32_u  (alias)
 *   __libcgj_batch4535_marker = "libcgj-batch4535"
 *
 * Exclusive continuum CREATE-ONLY (4531-4540: validation unique). Unique
 * gj_size_fits_u32_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4535_marker[] = "libcgj-batch4535";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if n is representable as uint32_t, else 0. */
static int
b4535_fits_u32(size_t uN)
{
	if ((uint64_t)uN <= (uint64_t)UINT32_MAX) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_size_fits_u32_u - 1 if n fits in uint32_t (n <= UINT32_MAX), else 0.
 *
 * n: size under test
 *
 * On 32-bit size_t this is always 1. No parent wires.
 */
int
gj_size_fits_u32_u(size_t uN)
{
	(void)NULL;
	return b4535_fits_u32(uN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_size_fits_u32_u(size_t uN)
    __attribute__((alias("gj_size_fits_u32_u")));
