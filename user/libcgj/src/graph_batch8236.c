/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8236: memrchr found-in-window probe stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_memrchr_found_u_8236(size_t pos, size_t n);
 *     - Return 1 if n > 0 and pos < n (soft reverse-hit index valid),
 *       else 0.
 *   uint32_t __gj_memrchr_found_u_8236  (alias)
 *   __libcgj_batch8236_marker = "libcgj-batch8236"
 *
 * Exclusive continuum CREATE-ONLY (8231-8240: memcmp/memchr probe
 * stubs — memcmp_eq, memcmp_lt, memcmp_gt, memchr_found, memchr_miss,
 * memrchr_found, mem_zero_len, mem_overlap, mem_align_ok,
 * batch_id_8240). Unique gj_memrchr_found_u_8236 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8236_marker[] = "libcgj-batch8236";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8236_found(size_t uPos, size_t uN)
{
	if (uN == 0u)
		return 0u;
	if (uPos < uN)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memrchr_found_u_8236 - 1 if soft memrchr index is inside [0, n).
 *
 * pos: candidate reverse-search hit index (offset into window)
 * n:   window length in octets
 *
 * Soft found probe for reverse memrchr results; does not scan memory
 * or call libc memrchr. No parent wires.
 */
uint32_t
gj_memrchr_found_u_8236(size_t uPos, size_t uN)
{
	(void)NULL;
	return b8236_found(uPos, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_memrchr_found_u_8236(size_t uPos, size_t uN)
    __attribute__((alias("gj_memrchr_found_u_8236")));
