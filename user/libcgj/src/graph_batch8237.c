/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8237: zero-length memory window probe stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mem_zero_len_u_8237(size_t n);
 *     - Return 1 if n == 0 (empty window), else 0.
 *   uint32_t __gj_mem_zero_len_u_8237  (alias)
 *   __libcgj_batch8237_marker = "libcgj-batch8237"
 *
 * Exclusive continuum CREATE-ONLY (8231-8240: memcmp/memchr probe
 * stubs — memcmp_eq, memcmp_lt, memcmp_gt, memchr_found, memchr_miss,
 * memrchr_found, mem_zero_len, mem_overlap, mem_align_ok,
 * batch_id_8240). Unique gj_mem_zero_len_u_8237 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8237_marker[] = "libcgj-batch8237";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8237_is_zero(size_t uN)
{
	if (uN == 0u)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_zero_len_u_8237 - 1 if memory window length is zero.
 *
 * n: window length in octets
 *
 * Soft empty-window probe for memcmp/memchr edge cases; does not
 * touch memory or call libc. No parent wires.
 */
uint32_t
gj_mem_zero_len_u_8237(size_t uN)
{
	(void)NULL;
	return b8237_is_zero(uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mem_zero_len_u_8237(size_t uN)
    __attribute__((alias("gj_mem_zero_len_u_8237")));
