/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8235: memchr miss-in-window probe stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_memchr_miss_u_8235(size_t pos, size_t n);
 *     - Return 1 if n == 0 or pos >= n (soft miss / not in window),
 *       else 0.
 *   uint32_t __gj_memchr_miss_u_8235  (alias)
 *   __libcgj_batch8235_marker = "libcgj-batch8235"
 *
 * Exclusive continuum CREATE-ONLY (8231-8240: memcmp/memchr probe
 * stubs — memcmp_eq, memcmp_lt, memcmp_gt, memchr_found, memchr_miss,
 * memrchr_found, mem_zero_len, mem_overlap, mem_align_ok,
 * batch_id_8240). Unique gj_memchr_miss_u_8235 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8235_marker[] = "libcgj-batch8235";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8235_miss(size_t uPos, size_t uN)
{
	if (uN == 0u)
		return 1u;
	if (uPos >= uN)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memchr_miss_u_8235 - 1 if soft memchr index is outside [0, n).
 *
 * pos: candidate hit index (offset into window)
 * n:   window length in octets
 *
 * Soft miss probe (complement of found); does not scan memory or call
 * libc memchr. No parent wires.
 */
uint32_t
gj_memchr_miss_u_8235(size_t uPos, size_t uN)
{
	(void)NULL;
	return b8235_miss(uPos, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_memchr_miss_u_8235(size_t uPos, size_t uN)
    __attribute__((alias("gj_memchr_miss_u_8235")));
