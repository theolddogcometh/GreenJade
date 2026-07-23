/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3338: length of a uint32_t dense list
 * (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   size_t gj_list_u32_len_u(const uint32_t *a, size_t n);
 *     - Report the live element count of a dense u32 list. Returns n
 *       when a is non-NULL; returns 0 when a is NULL (invalid list).
 *   size_t __gj_list_u32_len_u  (alias)
 *   __libcgj_batch3338_marker = "libcgj-batch3338"
 *
 * Exclusive continuum CREATE-ONLY (3331-3340). Distinct from
 * gj_list_len_n (batch2337 intrusive) - unique gj_list_u32_len_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3338_marker[] = "libcgj-batch3338";

/* ---- freestanding helpers ---------------------------------------------- */

/* Valid dense list reports its count; NULL base is empty/invalid. */
static size_t
b3338_len(const uint32_t *pA, size_t cN)
{
	if (pA == NULL) {
		return 0u;
	}
	return cN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_list_u32_len_u - live element count of a dense uint32_t list.
 *
 * a: list base (NULL -> 0)
 * n: caller-supplied live element count
 *
 * Returns n when a is non-NULL, else 0. Does not walk memory.
 * No parent wires. Does not call libc.
 */
size_t
gj_list_u32_len_u(const uint32_t *a, size_t n)
{
	(void)NULL;
	return b3338_len(a, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_list_u32_len_u(const uint32_t *a, size_t n)
    __attribute__((alias("gj_list_u32_len_u")));
