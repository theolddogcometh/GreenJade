/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8231: memcmp equal-order probe stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_memcmp_eq_u_8231(int32_t r);
 *     - Return 1 if r == 0 (equal order), else 0.
 *   uint32_t __gj_memcmp_eq_u_8231  (alias)
 *   __libcgj_batch8231_marker = "libcgj-batch8231"
 *
 * Exclusive continuum CREATE-ONLY (8231-8240: memcmp/memchr probe
 * stubs — memcmp_eq, memcmp_lt, memcmp_gt, memchr_found, memchr_miss,
 * memrchr_found, mem_zero_len, mem_overlap, mem_align_ok,
 * batch_id_8240). Unique gj_memcmp_eq_u_8231 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8231_marker[] = "libcgj-batch8231";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8231_is_eq(int32_t i32R)
{
	if (i32R == 0)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memcmp_eq_u_8231 - 1 if memcmp-style result is equal.
 *
 * r: raw memcmp / compare return value
 *
 * Returns 1 when r == 0, else 0. Soft equality probe on order; does
 * not call libc memcmp. No parent wires.
 */
uint32_t
gj_memcmp_eq_u_8231(int32_t i32R)
{
	(void)NULL;
	return b8231_is_eq(i32R);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_memcmp_eq_u_8231(int32_t i32R)
    __attribute__((alias("gj_memcmp_eq_u_8231")));
