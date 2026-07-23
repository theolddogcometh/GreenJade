/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4534: pointer equality predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ptr_eq_u(const void *a, const void *b);
 *     - Return 1 if a and b are the same address, else 0.
 *   int __gj_ptr_eq_u  (alias)
 *   __libcgj_batch4534_marker = "libcgj-batch4534"
 *
 * Exclusive continuum CREATE-ONLY (4531-4540: validation unique). Unique
 * gj_ptr_eq_u surface only; no multi-def. Distinct from
 * gj_ptr_diff_bytes (batch979). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4534_marker[] = "libcgj-batch4534";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if a and b compare equal as addresses, else 0. */
static int
b4534_eq(const void *pA, const void *pB)
{
	if (pA == pB) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ptr_eq_u - 1 if a and b are the same pointer address, else 0.
 *
 * a: first pointer
 * b: second pointer
 *
 * NULL equals NULL (returns 1). No parent wires.
 */
int
gj_ptr_eq_u(const void *pA, const void *pB)
{
	(void)NULL;
	return b4534_eq(pA, pB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ptr_eq_u(const void *pA, const void *pB)
    __attribute__((alias("gj_ptr_eq_u")));
