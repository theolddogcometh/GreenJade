/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4533: non-NULL pointer predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ptr_nonnull_u(const void *p);
 *     - Return 1 if p is non-NULL, else 0.
 *   int __gj_ptr_nonnull_u  (alias)
 *   __libcgj_batch4533_marker = "libcgj-batch4533"
 *
 * Exclusive continuum CREATE-ONLY (4531-4540: validation unique). Unique
 * gj_ptr_nonnull_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4533_marker[] = "libcgj-batch4533";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if p is not NULL, else 0. */
static int
b4533_nonnull(const void *p)
{
	if (p == NULL) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ptr_nonnull_u - 1 if p is non-NULL, else 0.
 *
 * p: pointer under test
 *
 * Pure null-check predicate. No parent wires.
 */
int
gj_ptr_nonnull_u(const void *p)
{
	(void)NULL;
	return b4533_nonnull(p);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ptr_nonnull_u(const void *p)
    __attribute__((alias("gj_ptr_nonnull_u")));
