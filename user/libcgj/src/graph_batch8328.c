/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8328: qsort trivial-domain predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_qsort_trivial_u_8328(size_t n);
 *     - Return 1 if a qsort of n elements is trivial (n < 2: empty or
 *       singleton already ordered); else 0.
 *   uint32_t __gj_qsort_trivial_u_8328  (alias)
 *   __libcgj_batch8328_marker = "libcgj-batch8328"
 *
 * Exclusive continuum CREATE-ONLY (8321-8330: qsort/bsearch compare
 * stubs). Complement of gj_qsort_need_u_8327. Unique _u_8328 surface;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8328_marker[] = "libcgj-batch8328";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff n < 2 (already ordered; qsort is a no-op). */
static uint32_t
b8328_trivial(size_t cN)
{
	if (cN < 2u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_qsort_trivial_u_8328 - 1 if qsort of n elements is trivial.
 *
 * n: element count of the candidate array
 *
 * Returns 1 when n < 2, else 0. Does not sort. No parent wires.
 */
uint32_t
gj_qsort_trivial_u_8328(size_t cN)
{
	(void)NULL;
	return b8328_trivial(cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_qsort_trivial_u_8328(size_t cN)
    __attribute__((alias("gj_qsort_trivial_u_8328")));
