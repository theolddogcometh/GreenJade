/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8239: memory alignment ok probe stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mem_align_ok_u_8239(size_t align);
 *     - Return 1 if align is a non-zero power of two, else 0.
 *   uint32_t __gj_mem_align_ok_u_8239  (alias)
 *   __libcgj_batch8239_marker = "libcgj-batch8239"
 *
 * Exclusive continuum CREATE-ONLY (8231-8240: memcmp/memchr probe
 * stubs — memcmp_eq, memcmp_lt, memcmp_gt, memchr_found, memchr_miss,
 * memrchr_found, mem_zero_len, mem_overlap, mem_align_ok,
 * batch_id_8240). Unique gj_mem_align_ok_u_8239 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8239_marker[] = "libcgj-batch8239";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8239_align_ok(size_t uAlign)
{
	if (uAlign == 0u)
		return 0u;
	/* Classic power-of-two test: exactly one bit set. */
	if ((uAlign & (uAlign - 1u)) == 0u)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_align_ok_u_8239 - 1 if align is a non-zero power of two.
 *
 * align: candidate alignment in octets
 *
 * Soft alignment probe for mem* helpers; does not call libc. No
 * parent wires.
 */
uint32_t
gj_mem_align_ok_u_8239(size_t uAlign)
{
	(void)NULL;
	return b8239_align_ok(uAlign);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mem_align_ok_u_8239(size_t uAlign)
    __attribute__((alias("gj_mem_align_ok_u_8239")));
